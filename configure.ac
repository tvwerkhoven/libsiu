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

# Check command line options
AC_ARG_WITH([ui],
		AC_HELP_STRING([--with-ui], [enable ui libraries]),
		[test "$withval" = yes && withui=true])

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

# ui libraries need OpenGL
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
	
	if test "$opengl" = true ; then
		AC_MSG_NOTICE([*** OpenGL found, UI libraries enabled.]);
		OPENGL_CFLAGS+="$GL_CFLAGS $GLU_CFLAGS $GLUT_CFLAGS $GLEXTMM_CFLAGS"
		OPENGL_LIBS+="$GL_LIBS $GLU_LIBS $GLUT_LIBS $GLEXTMM_LIBS"
		AC_SUBST(OPENGL_LIBS)
		AC_SUBST(OPENGL_CFLAGS)
	else
		AC_MSG_NOTICE([*** OpenGL not found, UI libraries enabled.]);
		AC_MSG_NOTICE([*** OpenGL missing: $openglmissing]);
	fi
	# end OpenGL configuration
fi

AM_CONDITIONAL([HAVE_OPENGL], [test "$opengl" = true])

AC_OUTPUT
