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

Io::Io() {
	Io(2);
}

Io::Io(int l) {
	setVerb(l);
	flog = NULL;
	setvbuf(stderr, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);
}


Io::~Io(void) {
	if(flog && flog != stdout && flog != stderr)
		fclose(flog);
}

int Io::setLogfile(std::string file) {
	logfile = file;
	flog = fopen(logfile.c_str(), "a");
}

int Io::msg(int type, const char *fmtstr, ...) {
	int level = type & IO_LEVEL_MASK;
	
	static const char *message[] = {"",  "error", "warn", "info", "xinfo", "debug1", "debug2"};
	static FILE *fdlevel[] = {NULL,  stderr, stderr, stdout, stdout, stdout, stdout};

	if (level <= verb) {
		va_list ap;
		va_start(ap, fmtstr);
		char *newfmt = "";

		if (type & IO_NOID) {
			newfmt = strcpy(new char[strlen(fmtstr) + 1], fmtstr);
		}
		else {
			newfmt = new char[strlen(fmtstr) + strlen(message[level]) + 5];
			sprintf(newfmt, "[%s] %s\n", message[level], fmtstr);
		}
		char *msg;
		vasprintf(&msg, newfmt, ap);
		fprintf(fdlevel[level], msg);
		fflush(fdlevel[level]);
		if (flog) {
			fprintf(flog, msg);
			fflush(flog);
		}
		free(msg);
		delete[] newfmt;
		va_end(ap);
	}
	
	if (type & IO_FATAL) exit(-1);
	if (type & IO_ERR) return -1;

	return 0;
}
