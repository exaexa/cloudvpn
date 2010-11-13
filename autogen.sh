#!/bin/sh

# simple autogen script that generates basic layout for autotools.
# not meant to be included in distribution.

COMMON_CPPFLAGS="-I\$(srcdir)/include/ -I/usr/local/include"
COMMON_CFLAGS="-Wall"
COMMON_LDFLAGS="-L/usr/local/lib"

OUT=Makefile.am
touch NEWS AUTHORS ChangeLog
echo > $OUT
cd plugins
PLUGINS=`echo *`
cd ..

echo "bin_PROGRAMS = cloudvpn" >>$OUT
echo "pkglib_LTLIBRARIES = `for i in ${PLUGINS}; do echo -n \"lib$i.la \" ; done`" >>$OUT
echo "noinst_HEADERS = `echo include/*.h`" >>$OUT

echo "cloudvpndir = src/" >>$OUT
echo "cloudvpn_SOURCES = `echo src/*.c`" >>$OUT
echo "cloudvpn_CPPFLAGS = ${COMMON_CPPFLAGS}" >>$OUT
echo "cloudvpn_CFLAGS = ${COMMON_CFLAGS}" >>$OUT
echo "cloudvpn_LDFLAGS = ${COMMON_LDFLAGS}" >>$OUT
echo "cloudvpn_LDADD = -lev -lpthread -ldl " >>$OUT
[ -f src/Makefile.am.extra ] &&
	while read l ; do
		[ "$l" ] && echo "cloudvpn_${l}" >>$OUT
	done < src/Makefile.am.extra

for i in $PLUGINS ; do
	echo "lib${i}_ladir = plugins/${i}" >>$OUT
	echo "lib${i}_la_SOURCES = `echo plugins/$i/*.c`" >>$OUT
	echo "noinst_HEADERS += `echo plugins/$i/*.h |grep -v '*'`" >>$OUT
	echo "lib${i}_la_CPPFLAGS = -I\$(SRCDIR)/plugins/$i/ ${COMMON_CPPFLAGS}" >>$OUT
	echo "lib${i}_la_CFLAGS = ${COMMON_CFLAGS}" >>$OUT
	echo "lib${i}_la_LDFLAGS = ${COMMON_LDFLAGS}" >>$OUT
	echo "lib${i}_la_LIBADD = " >>$OUT
	[ -f src/$i/Makefile.am.extra ] &&
		while read l ; do
			[ "$l" ] && echo "${i}_${l}" >>$OUT
		done < src/$i/Makefile.am.extra
done

libtoolize --force && aclocal && autoconf && automake --add-missing

