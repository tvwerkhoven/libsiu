## @file configure.ac
## @author Tim van Werkhoven (t.i.m.vanwerkhoven@xs4all.nl)

AC_PREREQ(2.59)
AC_INIT([LIBSIU], [0.1], [t.i.m.vanwerkhoven@xs4all.nl])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_FILES([
Makefile
src/Makefile
testing/Makefile
])
AC_CONFIG_HEADERS(autoconfig.h)
AC_CONFIG_SRCDIR(src/io.cc)

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_MAKE_SET
AC_PROG_INSTALL
AC_PROG_RANLIB

# Check command line options

# @todo CHECK THIS OPTION!
withui=true
AC_ARG_WITH([ui],
		AC_HELP_STRING([--without-ui], [enable ui libraries]),
		[test "$withval" = yes && withui=false])

# Check command line options
withtests=true
AC_ARG_WITH([tests],
		AC_HELP_STRING([--without-tests], [build test programs]),
		[test "$withval" = yes && withtests=false])
AM_CONDITIONAL([TESTING], [test "$withtests" = true])

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

# ImgData wants FITS, ICS and GSL please

PKG_CHECK_MODULES(GSL, [gsl], 
	[have_gsl=true; imgdata_types+="gsl "; AC_DEFINE([HAVE_GSL], [1], [ImgData supports GSL data.])],
	[have_gsl=false; AC_DEFINE([HAVE_GSL], [0], [ImgData does not supports GSL data.])])

AC_SEARCH_LIBS([ffopen], [cfitsio], 
	[have_fitsio=true; imgdata_types+="fits "; AC_DEFINE([HAVE_FITS], [1], [ImgData supports FITS data.])],
	[have_fitsio=false; AC_DEFINE([HAVE_FITS], [0], [ImgData does not supports FITS data.])])

AC_SEARCH_LIBS([deflate], [z],
		[have_z=true],
		[have_z=false])

AC_SEARCH_LIBS([IcsOpen], [ics],
		[have_ics=true],
		[have_ics=false])

if test "$have_z" = true -a "$have_ics" = true; then
	have_ics=true
	imgdata_types+="ics "
	AC_DEFINE([HAVE_ICS], [1], [ImgData supports ICS data.])
else
	have_ics=false
	AC_DEFINE([HAVE_ICS], [0], [ImgData does not support ICS data.])
fi

IMGDATA_CFLAGS="$GSL_CFLAGS"
IMGDATA_LIBS="$GSL_LIBS"
AC_SUBST(IMGDATA_CFLAGS)
AC_SUBST(IMGDATA_LIBS)

# UI libraries need OpenGL
if test "$withui" = true; then
	AC_MSG_NOTICE([*** UI libraries enabled, checking for OpenGL.]);
	AX_CHECK_GL
	AX_CHECK_GLU
	AX_CHECK_GLUT

	if test "$no_gl" = yes; then openglmissing+=" gl"; fi
	if test "$no_glu" = yes; then openglmissing+=" glu"; fi
	if test "$no_glut" = yes; then openglmissing+=" glut"; fi

	if test "$no_gl" = yes || test  "$no_glu" = yes || test "$no_glut" = yes ; then
		opengl=false;
		AC_MSG_WARN([*** OpenGL not found, not compiling UI libraries.]);
	else
		opengl=true;
	fi

	PKG_CHECK_MODULES(GLEXTMM, gtkglextmm-1.2, [],
		[AC_MSG_WARN([*** gtkglextmm-1.2 not found, not compiling UI libraries.]);opengl=false;openglmissing+=' gtkglextmm-1.2'])

	PKG_CHECK_MODULES(GTKMM, gtkmm-2.4, [],
		[AC_MSG_WARN([*** gtkmm-2.4 not found, not compiling UI libraries.]);opengl=false;openglmissing+=' gtkmm-2.4'])		

	PKG_CHECK_MODULES(GLIBMM, glibmm-2.4, [],
		[AC_MSG_WARN([*** glibmm-2.4 not found, not compiling UI libraries.]);opengl=false;openglmissing+=' glibmm-2.4'])		
	
	if test "$opengl" = true ; then
		AC_MSG_NOTICE([*** OpenGL found, UI libraries enabled.]);
		OPENGL_CFLAGS+="$GL_CFLAGS $GLU_CFLAGS $GLUT_CFLAGS $GLEXTMM_CFLAGS $GTKMM_CFLAGS $GLIBMM_LIBS"
		OPENGL_LIBS+="$GL_LIBS $GLU_LIBS $GLUT_LIBS $GLEXTMM_LIBS $GTKMM_LIBS $GLIBMM_LIBS"
		AC_SUBST(OPENGL_LIBS)
		AC_SUBST(OPENGL_CFLAGS)
	else
		AC_MSG_NOTICE([*** OpenGL not found, UI libraries enabled.]);
		AC_MSG_NOTICE([*** OpenGL missing: $openglmissing]);
	fi
	# end OpenGL configuration
fi

AM_CONDITIONAL([HAVE_OPENGL], [test "$opengl" = true])

AC_MSG_NOTICE([*** ImgData will support: $imgdata_types.]);

AC_OUTPUT
