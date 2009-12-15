
/*
 * CloudVPN
 *
 * This program is a free software: You can redistribute and/or modify it
 * under the terms of GNU GPLv3 license, or any later version of the license.
 * The program is distributed in a good hope it will be useful, but without
 * any warranty - see the aforementioned license for more details.
 * You should have received a copy of the license along with this program;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#include "route.h"

#define LOGNAME "cloud/route"
#include "log.h"
#include "conf.h"
#include "gate.h"
#include "network.h"
#include "timestamp.h"

#include <set>
#include <map>
using namespace std;

/*
 * utils
 */

#include <stdlib.h>

void init_random()
{
	timestamp_update();
	srand (timestamp() ^ (timestamp() / 1000000) );
}

#define rand_byte (rand()%256)

uint32_t new_packet_uid()
{
	uint32_t r = rand_byte;

	for (register int i = 0;i < 3;++i) r = (r << 8) | rand_byte;

	return r;
}


/*
 * ID cache
 */

static set<uint32_t> idcache;
static size_t idcache_max_size = 32768;
static int idcache_reduce_halftime = 1000000;
static uint64_t next_idcache_reduce = 0;

static void idcache_init()
{
	int t;
	if (!config_get_int ("packet_id_cache_size", t) ) t = 32768;
	Log_info ("ID cache max size is %d", t);
	idcache_max_size = t;

	if (!config_get_int ("packet_id_cache_time", t) ) t = 1000000;
	Log_info ("ID cache reduction halftime is %d", t);
	idcache_reduce_halftime = t;
}

static void idcache_reduce()
{
	//as we only need single random bits, so
	//we can usually spare some minor generator effort
	int randavail = 0, randd = 0;

	set<uint32_t>::iterator i, t;

	for (i = idcache.begin(); i != idcache.end();) {

		if (!randavail) {
			randd = rand();
			randavail = RAND_MAX;
		}

		t = i;
		++t;
		if (randd&1) idcache.erase (i);
		i = t;

		randavail >>= 1;
		randd >>= 1;
	}

	next_idcache_reduce = timestamp() + idcache_reduce_halftime;
}

static inline void idcache_periodic_reduce()
{
	if (next_idcache_reduce < timestamp() )
		idcache_reduce();
}

static inline void idcache_add_id (uint32_t id)
{
	idcache.insert (id);
	if (idcache.size() > idcache_max_size) idcache_reduce();
}

static inline bool id_already_seen (uint32_t id)
{
	return idcache.count (id) ? true : false;
}

/*
 * route tree
 */


/*
 * common stuff
 */

static int multipath_percentage = 0;
static int route_dirty = 0;
static int route_report_ping_diff_percent = 20;
static int route_max_dist = 64;
static int default_ttl = 128;
static int hop_penalization = 50;
static bool shared_uplink = false;

void route_periodic_update()
{
	idcache_periodic_reduce();
	route_update();
}

uint16_t new_packet_ttl()
{
	return default_ttl;
}

static void report_route();

void route_init()
{
	idcache_init();
	route.clear();
	reported_route.clear();
	route_dirty = 0;

	init_random();

	route_init_multi();

	int t;

	if (!config_get_int ("ping_change_to_report", t) ) t = 20;
	Log_info ("only ping changes above %d%% will be reported to peers",t);
	route_report_ping_diff_percent = t;

	if (!config_get_int ("route_max_dist", t) ) t = 64;
	Log_info ("maximal node distance is %d", t);
	route_max_dist = t;

	if (!config_get_int ("default_ttl", t) ) t = 128;
	Log_info ("default TTL is %d", t);
	default_ttl = t;

	if (!config_get_int ("route_hop_penalization", t) ) t = 0;
	Log_info ("hop penalization is %d%%", t);
	hop_penalization = t;

	if (!config_get_int ("multipath", t) ) t = 0;
	Log_info ("multipath percentage is %d%%", t);
	hop_penalization = t;

	if (shared_uplink = config_is_true ("shared_uplink") )
		Log_info ("sharing uplink for broadcasts");
}

void route_shutdown()
{
	route.clear();
	reported_route.clear();
}

void route_set_dirty()
{
	route_dirty=1;
}

inline uint64_t penalized_ping (uint64_t ping, uint64_t dist)
{
	if (hop_penalization)
		return ping* (100 + (dist*hop_penalization) ) / 100;
	else	return ping;
}

void route_update()
{
	if (!route_dirty) return;
	route_dirty = 0;

	map<int, connection>& cons = comm_connections();
	map<int, connection>::iterator i;
	map<address, connection::remote_route>::iterator j;
	map<int, gate>&gates = gate_gates();
	map<int, gate>::iterator g;
	list<address>::iterator k;

	report_route();
}

static void send_packet_to_id (int to,
                               uint32_t id, uint16_t ttl, uint32_t inst,
                               uint16_t dof, uint16_t ds,
                               uint16_t sof, uint16_t ss,
                               uint16_t s, const uint8_t*buf)
{
	if (to < 0) {
		map<int, gate>::iterator g =
		    gate_gates().find (- (to + 1) );
		if (g == gate_gates().end() ) return;
		g->second.send_packet (inst, dof, ds, sof, ss, s, buf);
	} else if (ttl) {
		map<int, connection>::iterator c =
		    comm_connections().find (to);
		if (c == comm_connections().end() ) return;
		c->second.write_packet (id, ttl - 1,
		                        inst, dof, ds, sof, ss,
		                        s, buf);
	}
}

template<class iter> static iter random_select (iter s, iter e)
{	//OMG. STL can't have SGI extensions everywhere!
	int n;
	for (n = 0;e != s;--e, ++n);
	n = rand() % n;
	for (;n > 0;++s, --n);
	return s;
}

void route_packet (uint32_t id, uint16_t ttl, uint32_t inst,
                   uint16_t dof, uint16_t ds,
                   uint16_t sof, uint16_t ss,
                   uint16_t s, const uint8_t*buf, int from)
{
	if ( (s < dof + ds) || (s < sof + ss) ) return; //invalid one

	if (!ttl) return; //don't spread this any further

	if (id_already_seen (id) ) return; //check duplicates
	idcache_add_id (id);

	route_update();

	address a (inst, buf + dof, ds);

	set<int> sendlist;

	{ //bracket cuz of variable scope

		/*
		 * note that this could be done with filter() of simple foreach cmp,
		 * but exploiting the tree properties is much faster. isn't it.
		 *
		 * (if lower_bound doesn't work logaritmic here, we are at least
		 * prepared for the time it will)
		 */

		map<address, route_info>::iterator i;
		size_t addrlen = a.addr.size();
		address p;

		//select shorter prefixes (abc sending to ab)
		for (size_t al = 0;al < addrlen;++al) {
			i = route.find (address
			                (a.inst, a.addr.begin().base(), al) );
			if (i == route.end() ) continue;
			sendlist.insert (i->second.id);
		}

		//select all longer or equal addresses (abc sends to abcd)
		i = route.lower_bound (a);
		for (; (i != route.end() ) && (!i->first.cmp (a, true) );++i)
			sendlist.insert (i->second.id);

		//sending to gates doesnt cost us anything - so try all.
		map<int, gate>::iterator
		gi = gate_gates().begin(),
		     ge = gate_gates().end();

		list<address>::iterator ai, ae;

		for (;gi != ge;++gi) {
			ai = gi->second.local.begin();
			ae = gi->second.local.end();
			for (;ai != ae;++ai) if (ai->cmp (a, true) )
					sendlist.insert (- (gi->first) - 1);
		}

		sendlist.erase (from); //don't send back

		set<int>::iterator k, ke; //now send to all destinations
		k = sendlist.begin();
		ke = sendlist.end();
		for (;k != ke;++k) if ( (*k < 0) || (ttl > 0) )
				send_packet_to_id (*k, id, ttl - 1, inst,
				                   dof, ds, sof, ss, s, buf);

		if (sendlist.size() ) return;
		//otherwise packet is lost and needs...

	}

	// the broadcast part!

	map<int, connection>::iterator
	i = comm_connections().begin(),
	    e = comm_connections().end();

	if (shared_uplink) {
		random_select (i, e)->second
		.write_packet (id, ttl - 1, inst,
		               dof, ds, sof, ss, s, buf);
		return;

	}

	for (;i != e;++i) {
		if (i->first == from) continue; //dont send back
		if (i->second.state != cs_active) continue; //ready only

		i->second.write_packet (id, ttl - 1, inst,
		                        dof, ds, sof, ss, s, buf);
	}
}

map<address, route_info>& route_get ()
{
	return route;
}

void route_report_to_connection (connection&c)
{
	/*
	 * note that route_update is NOT wanted here!
	 */

	size_t size = 0;
	map<address, route_info>::iterator r;
	for (r = reported_route.begin();r != reported_route.end();++r)
		size += r->first.addr.size() + 14;

	vector<uint8_t> data (size);
	uint8_t *datap = data.begin().base();

	for (r = reported_route.begin(); (r != reported_route.end() ); ++r) {
		* (uint32_t*) (datap) =
		    htonl ( (uint32_t) (r->second.ping) );
		* (uint32_t*) (datap + 4) =
		    htonl ( (uint32_t) (r->second.dist) );
		* (uint32_t*) (datap + 8) =
		    htonl ( (uint32_t) (r->first.inst) );
		* (uint16_t*) (datap + 12) =
		    htons ( (uint16_t) (r->first.addr.size() ) );
		sq_memcpy (datap + 14, r->first.addr.begin().base(),
		           r->first.addr.size() );
		datap += 14 + r->first.addr.size();
	}
	c.write_route_set (data.begin().base(), size);
}

static void report_route()
{
	/*
	 * called by route_update.
	 * determines which route information needs updating,
	 * and sends the diff info to remote connections
	 */

	map<address, route_info>::iterator r, oldr;
	list<pair<address, route_info> > report;

	for (r = route.begin(), oldr = reported_route.begin();
	        (r != route.end() ) && (oldr != reported_route.end() );) {

		if (r->first == oldr->first) { // hwaddresses match, check ping and distance
			if ( ( (unsigned int) route_report_ping_diff <

			        ( (r->second.ping > oldr->second.ping) ?
			          r->second.ping - oldr->second.ping :
			          oldr->second.ping - r->second.ping) )

			        || (r->second.dist != oldr->second.dist) )
				report.push_back (*r);
			++r;
			++oldr;
		} else if (r->first < oldr->first) { //not in old route
			report.push_back (*r);
			++r;
		} else { //not in new route
			report.push_back (pair<address, route_info>
			                  (oldr->first, route_info (0, 0, 0) ) );
			++oldr;
		}
	}
	while (r != route.end() ) { //rest of new routes
		report.push_back (*r);
		++r;
	}
	while (oldr != reported_route.end() ) {
		report.push_back (pair<address, route_info> (oldr->first, route_info (0, 0, 0) ) );
		++oldr;
	}

	/*
	 * now create the data to report, and apply the changes into rep. r.
	 */

	if (report.begin() == report.end() ) return; //nothing to report

	size_t size = 0;
	list<pair<address, route_info> >::iterator rep;
	for (rep = report.begin();rep != report.end();++rep)
		size += rep->first.addr.size() + 14;

	vector<uint8_t> data (size);
	uint8_t *datap = data.begin().base();

	for (rep = report.begin();
	        (rep != report.end() ); ++rep) {

		if (rep->second.ping) reported_route[rep->first] = rep->second;
		else reported_route.erase (rep->first);

		* (uint32_t*) (datap) =
		    htonl ( (uint32_t) (rep->second.ping) );
		* (uint32_t*) (datap + 4) =
		    htonl ( (uint32_t) (rep->second.dist) );
		* (uint32_t*) (datap + 8) =
		    htonl ( (uint32_t) (rep->first.inst) );
		* (uint16_t*) (datap + 12) =
		    htons ( (uint16_t) (rep->first.addr.size() ) );
		sq_memcpy (datap + 14, rep->first.addr.begin().base(),
		           rep->first.addr.size() );
		datap += 14 + rep->first.addr.size();
	}
	comm_broadcast_route_update (data.begin().base(), size);
}

