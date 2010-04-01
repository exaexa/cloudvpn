
AC_PREREQ(2.6)
AC_INIT([cloudvpn], [20100401])
AC_CONFIG_AUX_DIR(.) # because of libtoolize

AM_INIT_AUTOMAKE()
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])

AM_PROG_CC_C_O
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_LIBTOOL

AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/socket.h sys/time.h unistd.h])

AC_FUNC_CHOWN
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday memset select socket strchr strerror])

AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AC_OUTPUT(Makefile)

