## @file common.mk
## @author Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>
## Common makefile directives for all subdirs.

### Common flags
AM_CXXFLAGS = -Wall -Wextra -Wfatal-errors
AM_CPPFLAGS = $(SIGC_CFLAGS) -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS

### Debug options (first check strict debug, then regular. Add flags accordingly)
if HAVE_STR_DEBUG
AM_CPPFLAGS += -D_GLIBCXX_DEBUG \
		-D_GLIBCXX_DEBUG_PEDANTIC \
		-D_GLIBCXX_FULLY_DYNAMIC_STRING \
		-DGLIBCXX_FORCE_NEW
endif

if HAVE_DEBUG
AM_CXXFLAGS += -ggdb -g3 -O0 -fno-inline
# For mudflap, see http://www.stlinux.com/devel/debug/mudflap
#AM_CXXFLAGS += -fmudflap -fmudflapth -funwind-tables
#AM_LDFLAGS = -lmudflapth -rdynamic 
else
AM_CXXFLAGS += -O3 -ftree-vectorize
endif

### Profiling options
if HAVE_PROFILING
AM_CXXFLAGS += -pg
endif HAVE_PROFILING
