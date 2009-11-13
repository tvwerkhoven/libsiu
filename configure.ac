## @file configure.ac
## @author Tim van Werkhoven (t.i.m.vanwerkhoven@xs4all.nl)

AC_INIT([LIBSIU], [0.1], [t.i.m.vanwerkhoven@xs4all.nl])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_FILES([
Makefile
])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_RANLIB

# These libraries are crucial for all targets
AC_SEARCH_LIBS([sin],
		[m],
		[],
		[AC_MSG_ERROR([Cannot build libraries without libm])])

AC_SEARCH_LIBS([pthread_create],
		[pthread],
		[],
		[AC_MSG_ERROR([Cannot build libraries without pthreads])])

PKG_CHECK_MODULES(SIGC, [sigc++-2.0 >= 2.0], [],
		[AC_MSG_ERROR([Cannot build libraries without sigc++])])

AC_SUBST(SIGC_CFLAGS)
AC_SUBST(SIGC_LIBS)


AC_OUTPUT
