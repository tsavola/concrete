AC_PREREQ([2.68])
AC_INIT([concrete], [0], [timo.savola@iki.fi])
AC_CONFIG_SRCDIR([Makefile])
AC_CONFIG_FILES([config.mk auto.mk])

AC_ARG_ENABLE([debug], AC_HELP_STRING([--enable-debug], [disable optimization]))

if test x$enable_debug != xyes; then
	TARGET=optimize
else
	TARGET=debug
fi

AC_SUBST(TARGET)

AC_PROG_CXX

AC_CHECK_LIB(event_core, event_base_new)

AC_ARG_WITH([libanl], AC_HELP_STRING([--without-libanl], [don't use getaddrinfo_a]))

if test x$with_libanl != xno; then
	AC_CHECK_LIB(anl, getaddrinfo_a)
fi

if test x$ac_cv_lib_anl_getaddrinfo_a = xyes; then
	LIBRARY_RESOLVER=libanl
else
	AC_CHECK_LIB(event_extra, evdns_base_new)

	if test x$ac_cv_lib_event_extra_evdns_base_new = xyes; then
		LIBRARY_RESOLVER=libevent
	else
		echo "libanl nor libevent_extra 2 found"
		exit -1
	fi
fi

AC_SUBST(LIBRARY_RESOLVER)

AC_CHECK_HEADERS([fcntl.h netdb.h stddef.h strings.h sys/socket.h unistd.h])

AC_HEADER_STDBOOL
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset socket strchr strerror strncasecmp strtoul])

AC_OUTPUT
