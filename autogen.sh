#!/bin/sh

COMMON_CPPFLAGS="-Icommon/ -I/usr/local/include"
COMMON_LDFLAGS="-L/usr/local/libs"

OUT=Makefile.am
touch NEWS AUTHORS ChangeLog
echo > $OUT
cd src
PROGS=`echo *`
cd ..

echo "bin_PROGRAMS = ${PROGS}" >>$OUT
echo "noinst_LIBRARIES = libcommon.a" >>$OUT
echo "libcommon_a_SOURCES = `echo common/*.cpp`" >>$OUT
echo "noinst_HEADERS = `echo common/*.h`" >>$OUT
echo "libcommon_adir = common/" >>$OUT
echo "libcommon_a_CPPFLAGS = ${COMMON_CPPFLAGS}" >>$OUT

for i in $PROGS ; do
	echo "${i}dir = src/$i/" >>$OUT
	echo "${i}_SOURCES = `echo src/$i/*.cpp`" >>$OUT
	echo "noinst_HEADERS += `echo src/$i/*.h |grep -v '*'`" >>$OUT
	echo "${i}_CPPFLAGS = -Isrc/$i/ ${COMMON_CPPFLAGS}" >>$OUT
	echo "${i}_LDADD = libcommon.a" >>$OUT
	echo "${i}_LDFLAGS = ${COMMON_LDFLAGS}" >>$OUT #empty for future use.
	[ -f src/$i/Makefile.am.extra ] &&
		while read l ; do
			[ "$l" ] && echo "${i}_${l}" >>$OUT
		done < src/$i/Makefile.am.extra
done

aclocal && autoconf && automake --add-missing
