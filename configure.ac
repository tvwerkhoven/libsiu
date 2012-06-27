## @file configure.ac
## @author Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>

AC_PREREQ(2.59)
AC_INIT([LIBSIU], [0.1], [werkhoven@strw.leidenuniv.nl])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_FILES([
Makefile
src/Makefile
testing/Makefile
])
AC_CONFIG_HEADERS(autoconfig.h)
AC_CONFIG_SRCDIR(src/io.cc)

# @TODO Don't use -Werror because this conflicts over AM_PROG_AR in 1.11/1.12
AM_INIT_AUTOMAKE([foreign -Wall])

AC_COPYRIGHT([Copyright (C) 2007-2012 Guus Sliepen Guus Sliepen <guus@sliepen.eu.org> and Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>])

AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_RANLIB
# Does not work everywhere?
#AM_PROG_AR

### Check command line options ###############################################

AC_ARG_WITH([tests],
		AC_HELP_STRING([--without-tests], [do not build test programs]),
		[with_tests=$withval],
		[with_tests=yes])
AM_CONDITIONAL([WITH_TESTING], [test "x$with_tests" != "xno"])

AC_ARG_ENABLE([verbose],
		AC_HELP_STRING([--enable-verbose], [enable verbose debug output]),
		[have_verbose=$enableval],
		[have_verbose=no])
AS_IF([test "x$have_verbose" != "xno"],
		[AC_DEFINE([LIBSIU_VERBOSE], [1], [Toggle debugging.])],
		[AC_DEFINE([LIBSIU_VERBOSE], [0], [Toggle debugging.])])

AC_ARG_ENABLE([debug],
		AC_HELP_STRING([--enable-debug], [enable debug compilation]),
		[have_debug=$enableval],
		[have_debug=no])
AC_ARG_ENABLE([strict-debug],
		AC_HELP_STRING([--enable-strict-debug], [enable strict (agressive) debug output]),
		[have_str_debug=$enableval],
		[have_str_debug=no])
AM_CONDITIONAL([HAVE_STR_DEBUG], [test "x$have_str_debug" != "xno"])
AS_IF([test "x$have_str_debug" != "xno"],
		[
		AC_MSG_WARN([*** Enabling strict debugging. Continue at your own risk! (but in any case make sure to use strict debugging consistently)])
		have_debug=yes
		])
AM_CONDITIONAL([HAVE_DEBUG], [test "x$have_debug" != "xno"])

AC_ARG_ENABLE([profiling],
		AC_HELP_STRING([--enable-profiling], [enable profiling support]),
		[have_profiling=$enableval],
		[have_profiling=no])
AM_CONDITIONAL([HAVE_PROFILING], [test "x$have_profiling" != "xno"])

### Check mandatory libraries  ###############################################

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

### Check optional libraries #################################################

AC_MSG_NOTICE([*** Checking for libraries for ImgData.]);

PKG_CHECK_MODULES(GSL, [gsl], 
		[have_gsl=true; imgdata_types+="gsl "; AC_DEFINE([HAVE_GSL], [1], [ImgData supports GSL data.])],
		[have_gsl=false; AC_DEFINE([HAVE_GSL], [0], [ImgData does not supports GSL data.])])

AC_SEARCH_LIBS([ffopen], [cfitsio], 
		[have_cfitsio=true; imgdata_types+="fits "; AC_DEFINE([HAVE_CFITSIO], [1], [ImgData supports FITS data.])],
		[have_cfitsio=false; AC_DEFINE([HAVE_CFITSIO], [0], [ImgData does not supports FITS data.])])

AC_SEARCH_LIBS([deflate], [z],
		[have_z=true], [have_z=false])

AC_SEARCH_LIBS([IcsOpen], [ics],
		[have_libics=true], [have_libics=false])

AS_IF([test "x$have_z" = "xtrue" -a "x$have_libics" = "xtrue"],
		[
		imgdata_types+="ics "
		AC_DEFINE([HAVE_ICS], [1], [ImgData supports ICS data.])
		],
		[
		AC_DEFINE([HAVE_ICS], [0], [ImgData does not support ICS data.])
		])

AC_SUBST(IMGDATA_CFLAGS, ["$GSL_CFLAGS"])
AC_SUBST(IMGDATA_LIBS, ["$GSL_LIBS $LIBS"])

### Check OpenGL libraries ###################################################

AC_MSG_NOTICE([*** Checking for GUI libs (gtk, opengl etc.)]);

have_opengl=yes
AX_CHECK_GL
AS_IF([test "x$no_gl" = "xyes"], [have_opengl=no;glmissing+=" gl"])
#		AX_CHECK_GLU
#		AS_IF([test "x$no_glu" = "xyes"], [have_opengl=no;glmissing+=" glu"])
AX_CHECK_GLUT
AS_IF([test "x$no_glut" = "xyes"], [have_opengl=no;glmissing+=" glut"])

AM_CONDITIONAL([HAVE_OPENGL], [test "x$have_opengl" != "xno"])

PKG_CHECK_MODULES(GLEXTMM, gtkglextmm-1.2, [],
		[have_gui=no;guimissing+=' gtkglextmm-1.2'])

PKG_CHECK_MODULES(GTKMM, gtkmm-2.4, [],
		[have_gui=no;guimissing+=' gtkmm-2.4'])		

PKG_CHECK_MODULES(GLIBMM, glibmm-2.4, [],
		[have_gui=no;guimissing+=' glibmm-2.4'])		

AS_IF([test "x$have_gui" != "xno" -a "x$have_opengl" != "xno"],
		[
		AC_SUBST(GUI_CFLAGS, ["$GL_CFLAGS $GLU_CFLAGS $GLUT_CFLAGS $GLEXTMM_CFLAGS $GTKMM_CFLAGS $GLIBMM_LIBS"])
		AC_SUBST(GUI_LIBS, ["$GL_LIBS $GLU_LIBS $GLUT_LIBS $GLEXTMM_LIBS $GTKMM_LIBS $GLIBMM_LIBS"])
		])

# end OpenGL configuration

AC_MSG_NOTICE([*** ImgData support ............... $imgdata_types]);
AC_MSG_NOTICE([*** GUI libs ...................... $have_gui $guimissing]);
AC_MSG_NOTICE([*** OpenGL libs ................... $have_opengl $glmissing]);

AC_MSG_NOTICE([*** libsiu configuration options:])
AC_MSG_NOTICE([*** test programs: ................ $with_tests])
AC_MSG_NOTICE([*** verbose: ...................... $have_verbose])
AC_MSG_NOTICE([*** debug: ........................ $have_debug])
AC_MSG_NOTICE([*** strict debug: ................. $have_str_debug])
AC_MSG_NOTICE([*** profiling: .................... $have_profiling])


AC_OUTPUT
