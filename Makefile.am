AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -pthread -g -Wall
AM_CPPFLAGS = $(SIGC_CFLAGS)

noinst_LIBRARIES = libconfig.a libmessages.a libpidfile.a libserial.a libsocket.a libprotocol.a libio.a libimgdata.a

if HAVE_OPENGL
noinst_LIBRARIES += libglviewer.a
endif 

libconfig_a_SOURCES = config.cc
libmessages_a_SOURCES = messages.cc
libpidfile_a_SOURCES = pidfile.cc
libserial_a_SOURCES = serial.cc
libsocket_a_SOURCES = socket.cc
libprotocol_a_SOURCES = protocol.cc socket.cc
libio_a_SOURCES = io.cc

libimgdata_a_SOURCES = imgdata.cc
libimgdata_a_CFLAGS = $(IMGDATA_CFLAGS)
#libimgdata_a_LIBADD = $(IMGDATA_LIBS)

if HAVE_OPENGL
libglviewer_a_CPPFLAGS = $(OPENGL_CFLAGS) $(AM_CPPFLAGS)
libglviewer_a_CFLAGS = $(OPENGL_CFLAGS) 
libglviewer_a_SOURCES = glviewer.cc
endif

noinst_HEADERS = config.h format.h messages.h pidfile.h protocol.h pthread++.h serial.h socket.h io.h glviewer.h imgdata.h
