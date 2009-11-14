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
#include <sys/socket.h>
#include <string.h>

#include "io.h"

Io::Io() {
	Io(2);
}

Io::Io(int l) {
	level_mask = ~(0xFFFFFFFF << l);
	f = stderr;
	swap_endian = 0;
}


Io::~Io(void) {
	if(f && f != stdout && f != stderr)
		fclose(f);
}

void Io::reconf(int level) {
	verb = max(level, 0);
	level_mask = ~(0xFFFFFFFF << verb);
}

int Io::msg(int type, const char *formatstring, ...) {
	const char *message[] = {"",  "error", "warn", "info", "xinfo", "debug1", "debug2", "", ""};
	int level = type & level_mask;

	if(level) {
		va_list ap;
		va_start(ap, formatstring);
		char *newformatstring;

		if(!(type & IO_NOID)) {
			newformatstring = new char[strlen(formatstring) + strlen(message[level]) + 5];
			sprintf(newformatstring, "[%s] %s\n", message[level], formatstring);
		} else
			newformatstring = strcpy(new char[strlen(formatstring) + 1], formatstring);

		if(f) {
			vfprintf(f, newformatstring, ap);
			fflush(f);
		} else { // network store...
			char *str = new char[4096];		// fixed length: not so nice...
			vsprintf(str, newformatstring, ap);
			int size = strlen(str) + 1 + sizeof(int);
			unsigned char *buf = new unsigned char[size];
			delete[] str;
			delete[] buf;
		}

		delete[] newformatstring;
		va_end(ap);

		if(type & IO_FATAL)
			exit(1);
		if(type & IO_ERR)
			return -1;
	}

	return 0;
}
