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
	level_mask = ~(0xFFFFFFFF << l);
	f = stderr;
	flog = NULL;
}


Io::~Io(void) {
	if(f && f != stdout && f != stderr)
		fclose(f);
	if(flog && flog != stdout && flog != stderr)
		fclose(flog);
}

int Io::setLogfile(std::string file) {
	logfile = file;
	flog = fopen(logfile.c_str(), "a");
}

int Io::msg(int type, const char *formatstring, ...) {
	const char *message[] = {"",  "error", "warn", "info", "xinfo", "debug1", "debug2", "", ""};
	int level = type & level_mask;

	if(level) {
		va_list ap;
		va_start(ap, formatstring);
		char *newformatstring = "";

		if(!(type & IO_NOID)) {
			newformatstring = new char[strlen(formatstring) + strlen(message[level]) + 5];
			sprintf(newformatstring, "[%s] %s\n", message[level], formatstring);
		} else
			newformatstring = strcpy(new char[strlen(formatstring) + 1], formatstring);
		char *msg;
		vasprintf(&msg, newformatstring, ap);
		if (f) {
			fprintf(f, msg);
			fflush(f);
		}
		if (flog) {
			fprintf(flog, msg);
			fflush(flog);
		}
		free(msg);
		delete[] newformatstring;
		va_end(ap);

	}
	if(type & IO_FATAL)
		exit(-1);
	if(type & IO_ERR)
		return -1;

	return 0;
}
