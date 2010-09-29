/*
 path-test.cc -- Test path++ library
 Copyright (C) 2010 Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
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

int main(int argc, char *argv[]) {
	printf("testing: %s\n", argv[0]);
	
	Path p1("/absolute/path/file1");
	Path p2("relative/path/file1.ext");
	Path p3("/only/path/");
	Path p4(argv[0]);

	printf("isabs(%s): %d\n", p1.path.c_str(), p1.isabs());
	printf("isabs(%s): %d\n", p2.path.c_str(), p2.isabs());
	printf("isabs(%s): %d\n", p3.path.c_str(), p3.isabs());
	printf("isabs(%s): %d\n", p4.path.c_str(), p4.isabs());
	
	printf("---\n");

	printf("basename(%s): %s\n", p1.path.c_str(), p1.basename().c_str());
	printf("basename(%s): %s\n", p2.path.c_str(), p2.basename().c_str());
	printf("basename(%s): %s\n", p3.path.c_str(), p3.basename().c_str());
	printf("basename(%s): %s\n", p4.path.c_str(), p4.basename().c_str());

	printf("---\n");
	
	printf("dirname(%s): %s\n", p1.path.c_str(), p1.dirname().c_str());
	printf("dirname(%s): %s\n", p2.path.c_str(), p2.dirname().c_str());
	printf("dirname(%s): %s\n", p3.path.c_str(), p3.dirname().c_str());
	printf("dirname(%s): %s\n", p4.path.c_str(), p4.dirname().c_str());

	printf("---\n");

	printf("exists(%s): %d\n", p1.path.c_str(), p1.exists());
	printf("exists(%s): %d\n", p2.path.c_str(), p2.exists());
	printf("exists(%s): %d\n", p3.path.c_str(), p3.exists());
	printf("exists(%s): %d\n", p4.path.c_str(), p4.exists());

	printf("---\n");
	
	printf("isdir(%s): %d\n", p1.path.c_str(), p1.isdir());
	printf("isdir(%s): %d\n", p2.path.c_str(), p2.isdir());
	printf("isdir(%s): %d\n", p3.path.c_str(), p3.isdir());
	printf("isdir(%s): %d\n", p4.path.c_str(), p4.isdir());
	
	return 0;
}
