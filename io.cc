/*
 io.cc -- input/output routines
 Copyright (C) 2008-2009 Tim van Werkhoven (t.i.m.vanwerkhoven@xs4all.nl)
 
 This file is part of FOAM.
 
 FOAM is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 FOAM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with FOAM.	If not, see <http://www.gnu.org/licenses/>.

 */
/*! 
 @file io.cc 
 @author Michiel van Noort
 @brief input/output routines

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <algorithm>
#include <fstream>

#include "io.h"

void Io::init(int l) {
	setVerb(l);
	logfd = NULL;
	termfd = stdout;
}

Io::~Io(void) {
	if (logfd && logfd != stdout && logfd != stderr)
		fclose(logfd);
}

int Io::setLogfile(std::string file) {
	logfile = file;
	logfd = fopen(logfile.c_str(), "a");
	msg(IO_DEB1, "Using logfile %s.", logfile.c_str());
}

int Io::msg(int type, const char *fmtstr, ...) {
	int level = type & IO_LEVEL_MASK;
	
	static const char *prefix[] = {"",  "err ", "warn", "info", "xnfo", "dbg1", "dbg2"};

	if (level <= verb) {
		va_list ap;
		va_start(ap, fmtstr);
		char *newfmt = "";

		if (type & IO_NOID) {
			newfmt = strcpy(new char[strlen(fmtstr) + 1], fmtstr);
		}
		else {
			newfmt = new char[strlen(fmtstr) + strlen(prefix[level]) + 5];
			sprintf(newfmt, "[%s] %s\n", prefix[level], fmtstr);
		}
		char *msg;
		vasprintf(&msg, newfmt, ap);
		
		fprintf(termfd, msg);
		fflush(termfd);
		
		if (logfd) {
			fprintf(logfd, msg);
			fflush(logfd);
		}
		
		free(msg);
		delete[] newfmt;
		va_end(ap);
	}
	
	if (type & IO_FATAL) exit(-1);
	if (type & IO_ERR) return -1;

	return 0;
}
