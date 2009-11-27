#ifndef __IO_H__
#define __IO_H__

#include <string>
#include <stdio.h>
#include <unistd.h>

// Logging flags
#define IO_NOID         0x00000100      //!< Do not add loglevel string
#define IO_FATAL        0x00000200      //!< Fatal, quit immediately
#define IO_RETURN       0x00000400      //!< Give a non-zero return code

// Logging levels
#define IO_ERR          0x00000001 | IO_RETURN
#define IO_WARN         0x00000002
#define IO_INFO         0x00000003
#define IO_XNFO         0x00000004
#define IO_DEB1         0x00000005
#define IO_DEB2         0x00000006
#define IO_LEVEL_MASK   0x000000FF
#define IO_MAXLEVEL     0x00000006

class Io {
	int verb, level_mask;
	FILE *termfd;
	FILE *logfd;
	std::string logfile;
	
public:
	Io();
	Io(int l);
	~Io();

	int msg(int, const char*, ...);
	
	int setLogfile(std::string);
	
	int getVerb() { return verb; }
	int setVerb(int l) { verb = std::max(1, std::min(l, IO_MAXLEVEL)); return verb; }
	
	int incVerb() { return setVerb(verb+1); }
	int decVerb() { return setVerb(verb-1); }
};

#endif
