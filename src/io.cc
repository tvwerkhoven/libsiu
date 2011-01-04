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

#include "pthread++.h"
#include "path++.h"
#include "format.h"
#include "io.h"

const std::string PREFIX[] = {"",  "err ", "warn", "info", "xnfo", "dbg1", "dbg2"};

void Io::init(const int l) {
	setVerb(l);
	logfd = NULL;
	termfd = stdout;
}

Io::~Io(void) {
	if (logfd && logfd != stdout && logfd != stderr)
		fclose(logfd);
}

int Io::setLogfile(Path &file) {
	logfile = file;
	logfd = fopen(logfile.c_str(), "a");
	if (!logfd) return -1;
	return 0;
}

int Io::msg(int type, const std::string message) {
	// Apply default mask
	type = type | defmask;
	
	// Separate level from type mask	
	int level = type & IO_LEVEL_MASK;

	if (level <= verb) {
		string tmpmsg;

		// Build prefix
		if (!(type & IO_NOID))
			tmpmsg += "[" + PREFIX[level] + "] ";
		if (type & IO_THR)
			tmpmsg += format("(%x) ", (int) pthread_self());
		
		// Add message
		tmpmsg = tmpmsg + message;
		
		// Add postfix
		if (!(type & IO_NOLF))
			tmpmsg += "\n";

		fputs(tmpmsg.c_str(), termfd);
		fflush(termfd);
		
		if (logfd) {
			fputs(tmpmsg.c_str(), logfd);
			fflush(logfd);
		}
	}
	
	if (type & IO_FATAL) exit(-1);
	if (type & IO_ERR) return -1;
	
	return 0;
}

int Io::msg(int type, const char *fmtstr, ...) {
	// Separate level from type mask
	int level = type & IO_LEVEL_MASK;
	
	if (level <= verb) {
		va_list va;
		va_start(va, fmtstr);
		std::string result = vformat(fmtstr, va);
		va_end(va);
	
		msg(type, result);
	}

	if (type & IO_FATAL) exit(-1);
	if (type & IO_ERR) return -1;
	
	return 0;	
}
