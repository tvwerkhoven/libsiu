if TESTING

AM_CXXFLAGS = -pipe -Wall -Wextra \
		-Wfatal-errors -Wno-unused-variable \
		-ggdb

AM_CXXFLAGS += -I${top_srcdir}/src/ -L${top_srcdir}/src/

noinst_PROGRAMS = imgdata-test io-test io-test2 config-test csv-test path-test parse-test perflogger-test protocol-test protocol-thread-test pthread-test sighandle-test time-test

imgdata_test_SOURCES = imgdata-test.cc
imgdata_test_LDADD = ${top_srcdir}/src/libimgdata.a \
		${top_srcdir}/src/libio.a \
		$(GSL_LIBS)
imgdata_test_CPPFLAGS = $(GSL_CFLAGS) $(AM_CPPFLAGS)

io_test_SOURCES = io-test.cc
io_test_LDADD = ${top_srcdir}/src/libio.a

io_test2_SOURCES = io-test2.cc
io_test2_LDADD = ${top_srcdir}/src/libio.a

config_test_SOURCES = config-test.cc
config_test_LDADD = ${top_srcdir}/src/libconfig.a

csv_test_SOURCES = csv-test.cc
csv_test_LDADD = ${top_srcdir}/src/libcsv.a \
		$(GSL_LIBS)
csv_test_CPPFLAGS = $(GSL_CFLAGS) $(AM_CPPFLAGS)

path_test_SOURCES = path-test.cc
path_test_LDADD = ${top_srcdir}/src/libpath.a

parse_test_SOURCES = parse-test.cc

perflogger_test_SOURCES = perflogger-test.cc
perflogger_test_CPPFLAGS = $(SIGC_CFLAGS) $(AM_CPPFLAGS)
perflogger_test_LDADD = ${top_srcdir}/src/libperflogger.a \
		$(SIGC_LIBS)

protocol_test_SOURCES = protocol-test.cc
protocol_test_CPPFLAGS = $(SIGC_CFLAGS) $(AM_CPPFLAGS)
protocol_test_LDADD = ${top_srcdir}/src/libprotocol.a \
		${top_srcdir}/src/libsocket.a \
		$(SIGC_LIBS)

protocol_thread_test_SOURCES = protocol-thread-test.cc
protocol_thread_test_CPPFLAGS = $(SIGC_CFLAGS) $(AM_CPPFLAGS)
protocol_thread_test_LDADD = ${top_srcdir}/src/libprotocol.a \
		${top_srcdir}/src/libsocket.a \
		${top_srcdir}/src/libsighandle.a \
		$(SIGC_LIBS)

pthread_test_SOURCES = pthread-test.cc
pthread_test_CPPFLAGS = $(SIGC_CFLAGS) $(AM_CPPFLAGS)

sighandle_test_SOURCES = sighandle-test.cc
sighandle_test_CPPFLAGS = $(SIGC_CFLAGS) $(AM_CPPFLAGS)
sighandle_test_LDADD = ${top_srcdir}/src/libsighandle.a \
		$(SIGC_LIBS)

time_test_SOURCES = time-test.cc
time_test_LDADD = ${top_srcdir}/src/libtime.a

if HAVE_OPENGL
noinst_PROGRAMS += opengl-test

opengl_test_SOURCES = opengl-test.cc
opengl_test_CPPFLAGS = $(OPENGL_CFLAGS) $(AM_CPPFLAGS)
opengl_test_CFLAGS = $(OPENGL_CFLAGS) 
opengl_test_LDADD = $(OPENGL_LIBS) ${top_srcdir}/src/libglviewer.a
endif

TESTS = $(noinst_PROGRAMS)

endif TESTING
