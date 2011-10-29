/*
 io.cc -- input/output routines
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
/*! 
 @file io.cc 
 @author Michiel van Noort
 @brief input/output routines

 @todo Review thread safety, might be some problems in msg()...
*/

#include <cstdio>
#include <string>
#include <sigc++/signal.h>
#include "pthread++.h"
#include "path++.h"
#include "format.h"
#include "io.h"

const std::string PREFIX[] = {"",  "err ", "warn", "info", "xnfo", "dbg1", "dbg2"};

Io::Io(const int l): verb(l), termfd(stdout), logfd(NULL), defmask(0), do_log(true), totmsg(0), lockfail(0), buffull(0) { 
	verb = max(1, min(l, IO_MAXLEVEL)); 

	// Start handler thread
	{
		pthread::mutexholder h(&handler_mutex);
		handler_thr.create(sigc::mem_fun(*this, &Io::handler));
		handler_cond.wait(handler_mutex);
	}
}

Io::~Io(void) {
	parse_msg(IO_INFO, format("Stopping Io, total messages: %zu, lockfail lost: %zu, buffer lost: %zu\n", totmsg, lockfail, buffull));
	// Stop handler() thread
	do_log = false;

	if (logfd && logfd != stdout && logfd != stderr)
		fclose(logfd);
	
	pthread::mutexholder h(&handler_runmutex);
	handler_thr.cancel();
	handler_thr.join();
}

void Io::handler() {
	pthread::setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS);
	bool init=false;
	
	pthread::mutexholder h(&handler_runmutex);
	
	while (do_log) {
		// Signal main thread once that we started, we don't wait Io to finish before we even got to the main loop.
		if (!init) {
			pthread::mutexholder h(&handler_mutex);
			handler_cond.signal();
			init = true;
		}
		// Wait until there is a new message
		usleep(0.05 * 1e6);
		
		while (!msgbuf.empty()) {
			// Get mutex to delete with data
			pthread::mutexholder h(&log_mutex);
			
			// Print & store messages
			IoMessage *thismsg = msgbuf.front();
			parse_msg(thismsg->type, thismsg->msg);
			
			//fprintf(stderr, "Io::lockfail: %zu, got msg: %s\n", lockfail, thismsg->msg.c_str());
			delete thismsg;
			msgbuf.pop();
		}
	}
	
	// Flush one last time
	while (!msgbuf.empty()) {
		pthread::mutexholder h(&log_mutex);
		
		IoMessage *thismsg = msgbuf.front();
		parse_msg(thismsg->type, thismsg->msg);
		delete thismsg;
		msgbuf.pop();
	}
}

int Io::setLogfile(const Path &file) {
	logfile = file;
	logfd = fopen(logfile.c_str(), "a");
	if (!logfd)
		return -1;
	return 0;
}

int Io::parse_msg(const int type, const std::string &message) {
	// Apply default mask
	int mytype = (type | defmask);
	
	// Separate level from type mask	
	int level = mytype & IO_LEVEL_MASK;
	
	if (level <= verb) {
		std::string tmpmsg;
		
		// Build prefix unless IO_NOID is set
		if (!(mytype & IO_NOID))
			tmpmsg += "[" + PREFIX[level] + "] ";
		
		// Add thread ID if IO_THR is set
		if (mytype & IO_THR) {
			//! @todo Thread ID not supported anymore because of Io:: handling
//			// From: http://stackoverflow.com/questions/1759794/how-to-print-pthread-t
//			pthread_t pt = pthread_self();
//			unsigned char *ptc = (unsigned char*)(void*)(&pt);
//			tmpmsg += "(0x";
//			for (size_t i=0; i<sizeof(pt); i++)
//				tmpmsg += format("%02x", (unsigned)(ptc[i]));
//			tmpmsg += ")";
			tmpmsg += "(??) ";
		}
		
		// Add message to prefix
		tmpmsg = tmpmsg + message;
		
		// Add postfix unless IO_NOLF is set
		if (!(mytype & IO_NOLF))
			tmpmsg += "\n";
		
		// Print this to requested term & flush
		fputs(tmpmsg.c_str(), termfd);
		fflush(termfd);
		
		// If we're logging to a file, check this as well
		if (logfd) {
			fputs(tmpmsg.c_str(), logfd);
			fflush(logfd);
		}
	}
	
	return 0;
}

int Io::msg(const int type, const std::string message) {
	totmsg++;
	
	// Low priority messages get queued...
	if ((type & IO_LEVEL_MASK) > IO_WARN) {
		// Buffer full, discard this message
		if (msgbuf.size() > 100000) {
			buffull++;
			return 0;
		}

		pthread::mutexholdertry h(&log_mutex);
		if (h.havelock()) {
			msgbuf.push(new IoMessage(type, message));
		}
		else {
			lockfail++;
		}
	}
	// High priority messages are printed immediately.
	else {
		parse_msg(type, message);
	}
	
	if (type & (IO_FATAL)) exit(-1);
	if (type & (IO_ERR)) return -1;
	return 0;
}

int Io::msg(const int type, const char *fmtstr, ...) {
	// Separate level from type mask
	int level = type & IO_LEVEL_MASK;
	
	//! @todo Always log to file?
	if (level <= verb) {
		va_list va;
		va_start(va, fmtstr);
		char buf[1024];
		vsnprintf(buf, sizeof buf, fmtstr, va);
		va_end(va);
	
		msg(type, std::string(buf));
	}

	if (type & IO_FATAL) exit(-1);
	if (type & IO_ERR) return -1;
	
	return 0;	
}
