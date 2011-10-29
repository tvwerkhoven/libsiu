/*
 io.h -- input/output routines
 Copyright (C) 2008--2010 Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 Copyright (C) 2009 Michiel van Noort
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef HAVE_IO_H
#define HAVE_IO_H

#include <string>
#include <cstdio>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <deque>
#include <queue>

#include "path++.h"
#include "pthread++.h"

// Logging flags
#define IO_NOID         0x00000100      //!< Do not add loglevel string
#define IO_FATAL        0x00000200      //!< Fatal, quit immediately
#define IO_RETURN       0x00000400      //!< Give a non-zero return code
#define IO_NOLF         0x00000800      //!< Do not add linefeed
#define IO_THR          0x00001000      //!< Show thread id prefix

// Logging levels
#define IO_ERR          0x00000001 | IO_RETURN
#define IO_WARN         0x00000002
#define IO_INFO         0x00000003
#define IO_XNFO         0x00000004
#define IO_DEB1         0x00000005
#define IO_DEB2         0x00000006
#define IO_LEVEL_MASK   0x000000FF
#define IO_MAXLEVEL     0x00000006

using namespace std;

class IoMessage {
public:
	IoMessage(const int t, const string &m): type(t), msg(m) { gettimeofday(&tv, NULL); }
	~IoMessage() { fprintf(stderr, "~IoMessage()\n"); }
	
	const int type;										//!< Type of message
	struct timeval tv;								//!< Time at which message is logged
	const string msg;									//!< Message for log entry
};

class Io {
	int verb, level_mask;								//!< Verbosity that we display
	FILE *termfd;
	FILE *logfd;
	Path logfile;												//!< File to log to
	uint32_t defmask;										//!< Default type mask
	
//	typedef struct logmsg {
//		logmsg(string m, int t) msg(m), type(t) { gettimeofday(&tv); }
//		int type;													//!< Type of message
//		struct timeval tv;								//!< Time at which message is logged
//		string msg;												//!< Message for log entry
//	} logmsg_t;													//!< Datatype used to store log messages
//	
//	std::vector<logmsg_t> msgbuf;				//!< Message buffer
	queue< IoMessage *> msgbuf; 				//!< Message buffer
	pthread::mutex log_mutex;						//!< msgbuf access mutex
	
	pthread::thread handler_thr;				//!< Thread that handles messages
	pthread::cond handler_cond;
	pthread::mutex handler_mutex;
	
	void handler();											//!< Handler function, prints & saves log messages
	bool do_log;												//!< Flag controlling handler() shutdown
	
	size_t lockfail;										//!< Lost messages due to lock fails
	
	int parse_msg(const int type, const string &message);
	
public:
	Io(const int l=IO_MAXLEVEL);
	~Io();

	int msg(const int, const char*, ...);
	int msg(const int, const std::string);
	
	int setLogfile(const Path&);
	Path getLogfile() const { return logfile; }
	
	int getVerb() const { return verb; }
	int setVerb(const int l) { verb = max(1, min(l, IO_MAXLEVEL)); return verb; }
	int setVerb(string l) { return setVerb((int) strtoll(l.c_str(), NULL, 0)); }
	
	uint32_t setdefmask(const uint32_t m) { defmask = m; return m; }
	uint32_t getdefmask() const { return defmask; }
	
		
	int incVerb() { return setVerb(verb+1); }
	int decVerb() { return setVerb(verb-1); }
};

#endif // HAVE_IO_H
