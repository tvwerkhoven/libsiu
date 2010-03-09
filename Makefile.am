AUTOMAKE_OPTIONS = foreign

ACLOCAL_AMFLAGS = -I m4

AM_CXXFLAGS = -pthread
AM_CPPFLAGS = $(SIGC_CFLAGS)

noinst_LIBRARIES = libconfig.a libmessages.a libpidfile.a libserial.a libsocket.a libprotocol.a libio.a 

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

if HAVE_OPENGL
libglviewer_a_CPPFLAGS = $(OPENGL_CFLAGS) $(AM_CPPFLAGS)
libglviewer_a_CFLAGS = $(OPENGL_CFLAGS) 
libglviewer_a_SOURCES = glviewer.cc
#libglviewer_a_LIBADD = $(OPENGL_LIBS)
endif

noinst_HEADERS = config.h format.h messages.h pidfile.h protocol.h pthread++.h serial.h socket.h io.h glviewer.h
