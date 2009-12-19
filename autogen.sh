#!/bin/sh

# simple autogen script that generates basic layout for autotools.
# not meant to be included in distribution.

COMMON_CPPFLAGS="-I\$(srcdir)/common/ -I/usr/local/include"
COMMON_LDFLAGS="-L/usr/local/libs"

OUT=Makefile.am
touch NEWS AUTHORS ChangeLog
echo > $OUT
cd src
PROGS=`echo *`
cd ../connectors
CONNECTORS=`echo *`
cd ..

echo "bin_PROGRAMS = ${PROGS}" >>$OUT
echo "lib_LTLIBRARIES = `for i in ${CONNECTORS}; do echo -n \"lib$i.la \" ; done`" >>$OUT
echo "noinst_LIBRARIES = libcommon.a" >>$OUT
echo "libcommon_a_SOURCES = `echo common/*.cpp`" >>$OUT
echo "noinst_HEADERS = `echo common/*.h`" >>$OUT
echo "libcommon_adir = common/" >>$OUT
echo "libcommon_a_CPPFLAGS = ${COMMON_CPPFLAGS}" >>$OUT

for i in $PROGS ; do
	echo "${i}dir = src/$i/" >>$OUT
	echo "${i}_SOURCES = `echo src/$i/*.cpp`" >>$OUT
	echo "noinst_HEADERS += `echo src/$i/*.h |grep -v '*'`" >>$OUT
	echo "${i}_CPPFLAGS = -I\$(srcdir)/src/$i/ ${COMMON_CPPFLAGS}" >>$OUT
	echo "${i}_LDADD = libcommon.a" >>$OUT
	echo "${i}_LDFLAGS = ${COMMON_LDFLAGS}" >>$OUT #empty for future use.
	[ -f src/$i/Makefile.am.extra ] &&
		while read l ; do
			[ "$l" ] && echo "${i}_${l}" >>$OUT
		done < src/$i/Makefile.am.extra
done

for i in $CONNECTORS ; do
	echo "lib${i}_ladir = connectors/${i}" >>$OUT
	echo "lib${i}_la_SOURCES = `echo connectors/$i/*.cpp`" >>$OUT
	echo "noinst_HEADERS += `echo src/$i/*.h |grep -v '*'`" >>$OUT
	echo "lib${i}_la_CPPFLAGS = -I\$(SRCDIR)/src/$i/ ${COMMON_CPPFLAGS}" >>$OUT
	echo "lib${i}_la_LDFLAGS = ${COMMON_LDFLAGS}" >>$OUT #empty for future use.
	[ -f src/$i/Makefile.am.extra ] &&
		while read l ; do
			[ "$l" ] && echo "${i}_${l}" >>$OUT
		done < src/$i/Makefile.am.extra
done

libtoolize --force && aclocal && autoconf && automake --add-missing

