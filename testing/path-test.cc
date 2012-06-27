/*
 path-test.cc -- Test path++ library
 Copyright (C) 2010 Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>
 
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

#include <string>
#include "path++.h"

int main(int /* argc */, char *argv[]) {
	printf("path-test.cc:: testing: %s\n", argv[0]);
	
	Path p1("/absolute/path/file1");
	Path p2("relative/path/file1.ext");
	Path p3("/tmp/../tmp/");
	Path p4(argv[0]);
	
	printf("path-test.cc:: p1: '%s'\n", p1.c_str());
	printf("path-test.cc:: p2: '%s'\n", p2.c_str());
	printf("path-test.cc:: p3: '%s'\n", p3.c_str());
	printf("path-test.cc:: p4: '%s'\n", p4.c_str());


	printf("path-test.cc:: isabs()\n");
	if (!p1.isabs() || p2.isabs() || !p3.isabs() || p4.isabs()) {
		printf("path-test.cc:: error\n");
		return -1;
	}		
	
	printf("path-test.cc:: basename()\n");
	if (p1.basename() != "file1" || 
			p2.basename() != "file1.ext" || 
			p3.basename() != string("")) {
		printf("path-test.cc:: error\n");
		printf("path-test.cc:: p1: '%s'\n", p1.basename().c_str());
		printf("path-test.cc:: p2: '%s'\n", p2.basename().c_str());
		printf("path-test.cc:: p3: '%s'\n", p3.basename().c_str());
		return -1;
	}
	
	printf("path-test.cc:: dirname()\n");
	if (p1.dirname() != "/absolute/path/" || 
			p2.dirname() != "relative/path/" || 
			p3.dirname() != "/tmp/../tmp/") {
		printf("path-test.cc:: error\n");
		printf("path-test.cc:: p1: '%s'\n", p1.dirname().c_str());
		printf("path-test.cc:: p2: '%s'\n", p2.dirname().c_str());
		printf("path-test.cc:: p3: '%s'\n", p3.dirname().c_str());
		return -1;
	}

	
	printf("path-test.cc:: exists()\n");
	if (!p3.exists() ||
			!p4.exists()) {
		printf("path-test.cc:: error\n");
		printf("path-test.cc:: p3: %d\n", p3.exists());
		printf("path-test.cc:: p4: %d\n", p4.exists());
		return -1;
	}
	
	printf("path-test.cc:: isdir()\n");
	if (!p3.isdir() ||
			p4.isdir()) {
		printf("path-test.cc:: error\n");
		printf("path-test.cc:: p3: %d\n", p3.isdir());
		printf("path-test.cc:: p4: %d\n", p4.isdir());
		return -1;
	}	

	printf("path-test.cc:: == / !=\n");
	if (p1 == p2 ||
			p4 != p4) {
		printf("path-test.cc:: error\n");
		printf("path-test.cc:: p1 == p2: %d\n", p1 == p2);
		printf("path-test.cc:: p4 != p4: %d\n", p4 != p4);
		return -1;
	}
	
	printf("path-test.cc:: +\n");
	if (p1 + p2 != "/absolute/path/file1/relative/path/file1.ext" ||
			p2 + p3 != p3) {
		printf("path-test.cc:: error\n");
		return -1;
	}	
	
	printf("path-test.cc:: test ok!\n");
	return 0;
}
