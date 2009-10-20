
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

#include "log.h"
#include "conf.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <syslog.h>
#include <string.h>

#define output_file stdout

bool syslog_mode = false;


/*
 * Log_info is verbose enough by default. Debug not shown.
 */
static int log_level = LOG_INFO;

void log_setlevel (int l)
{
	log_level = l;
}

static const char* loglevel_mark (int level)
{
	switch (level) {
		case LOG_DEBUG:
			return "debug ";
		case LOG_INFO:
			return "(info) ";
		case LOG_WARN:
			return "* warning ";
		case LOG_ERROR:
			return "*** Error ";
		case LOG_FATAL:
			return "FATAL ";
		default:
			return "";
	}
}

#define DATEFORMAT "%Y-%m-%d %H:%M:%S: "

void log_init (const char*name) {
	if (config_is_true ("syslog") ) syslog_mode = true;
	if ( syslog_mode ) openlog(name, LOG_PID|LOG_CONS, LOG_DAEMON);
}

void Log (int lvl, const char*fmt, ...)
{
	if (lvl > log_level) return;
	char date_buf[65];
	time_t t = time (0);

	strftime (date_buf, 64, DATEFORMAT, localtime (&t) );
	fputs (date_buf, output_file);
	fputs (loglevel_mark (lvl), output_file);

	va_list ap;
	va_start (ap, fmt);
	vfprintf (output_file, fmt, ap);
	va_end (ap);

	fprintf (output_file, "\n");
	fflush (output_file);
}

void Log_full (int lvl, const char*file, int line,
		const char*fmt, ...)
{
	if (lvl > log_level) return;
	char date_buf[65];
	time_t t = time (0);

	strftime (date_buf, 64, DATEFORMAT, localtime (&t) );
	if ( !syslog_mode) fputs (date_buf, output_file);

	va_list ap;
	va_start (ap, fmt);
	log_print (output_file, file, line, lvl, fmt, ap);
	va_end (ap);

}

void log_print(FILE*output,const char*file, int line, int lvl, const char*fmt, va_list ap){
char buffer[256];
	if ( syslog_mode ) {
		int len = strlen(loglevel_mark (lvl) );
		strncpy (buffer, loglevel_mark (lvl), len);
		len += sprintf (buffer+len, "%s@%d: ", file, line);
		vsnprintf (buffer+len, 256-len, fmt, ap);
		syslog (lvl, buffer);
	} else {
		fputs (loglevel_mark (lvl), output);
		fprintf (output, "%s@%d:\t", file, line);
		vfprintf (output, fmt, ap);
		fprintf (output_file, "\n");
		fflush (output_file);
	}
}

