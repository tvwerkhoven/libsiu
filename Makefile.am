AM_CXXFLAGS = -pthread
AM_CPPFLAGS = $(SIGC_CFLAGS)

noinst_LIBRARIES = libconfig.a libmessages.a libpidfile.a libserial.a libsocket.a libprotocol.a

libconfig_a_SOURCES = config.cc
libmessages_a_SOURCES = messages.cc
libpidfile_a_SOURCES = pidfile.cc
libserial_a_SOURCES = serial.cc
libsocket_a_SOURCES = socket.cc
libprotocol_a_SOURCES = protocol.cc socket.cc

noinst_HEADERS = config.h format.h messages.h pidfile.h protocol.h pthread++.h serial.h socket.h
