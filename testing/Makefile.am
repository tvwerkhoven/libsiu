if TESTING

AM_CXXFLAGS = -pipe -Wall -Wextra \
		-Wfatal-errors -Wno-unused-variable \
		-O3 -ftree-vectorize \
		-g

AM_CPPFLAGS = -I.. -L..

noinst_PROGRAMS = io-test config-test pthread-test path-test

io_test_SOURCES = io-test.cc
io_test_LDADD = ../libio.a
config_test_SOURCES = config-test.cc
config_test_LDADD = ../libconfig.a
pthread_test_SOURCES = pthread-test.cc
pthread_test_CPPFLAGS = $(SIGC_CFLAGS) $(AM_CPPFLAGS)
path_test_SOURCES = path-test.cc
path_test_CPPFLAGS = ../libpath.a


endif TESTING
