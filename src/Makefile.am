include $(top_srcdir)/common.mk

noinst_LIBRARIES = libconfig.a libcsv.a libmessages.a libperflogger.a libpidfile.a libserial.a libsighandle.a libsocket.a libprotocol.a libio.a libimgdata.a libpath.a libtime.a

if HAVE_OPENGL
noinst_LIBRARIES += libglviewer.a
endif 

libpath_a_SOURCES = path++.cc
libconfig_a_SOURCES = config.cc
libcsv_a_SOURCES = csv.cc path++.cc
libcsv_a_CPPFLAGS = $(GSL_CFLAGS) $(AM_CPPFLAGS)
libmessages_a_SOURCES = messages.cc
libperflogger_a_SOURCES = perflogger.cc
libpidfile_a_SOURCES = pidfile.cc
libsighandle_a_SOURCES = sighandle.cc
libserial_a_SOURCES = serial.cc
libsocket_a_SOURCES = socket.cc
libprotocol_a_SOURCES = protocol.cc socket.cc
libio_a_SOURCES = io.cc
libtime_a_SOURCES = time++.cc

libimgdata_a_SOURCES = imgdata.cc
libimgdata_a_CPPFLAGS = $(IMGDATA_CFLAGS) $(AM_CPPFLAGS)
#libimgdata_a_LIBADD = $(IMGDATA_LIBS)


if HAVE_OPENGL
libglviewer_a_SOURCES = glviewer.cc
libglviewer_a_CPPFLAGS = $(GUI_CFLAGS) $(AM_CPPFLAGS)
libglviewer_a_CFLAGS = $(GUI_CFLAGS) $(AM_CFLAGS)
endif

noinst_HEADERS = config.h csv.h format.h messages.h pidfile.h protocol.h pthread++.h serial.h sighandle.h socket.h io.h glviewer.h imgdata.h path++.h perflogger.h time++.h types.h utils.h 



