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

	printf("isabs(%s): %d\n", p1.getpath_c(), p1.isabs());
	printf("isabs(%s): %d\n", p2.getpath_c(), p2.isabs());
	printf("isabs(%s): %d\n", p3.getpath_c(), p3.isabs());
	printf("isabs(%s): %d\n", p4.getpath_c(), p4.isabs());
	
	printf("---\n");

	printf("basename(%s): %s\n", p1.getpath_c(), p1.basename_c());
	printf("basename(%s): %s\n", p2.getpath_c(), p2.basename_c());
	printf("basename(%s): %s\n", p3.getpath_c(), p3.basename_c());
	printf("basename(%s): %s\n", p4.getpath_c(), p4.basename_c());

	printf("---\n");
	
	printf("dirname(%s): %s\n", p1.getpath_c(), p1.dirname_c());
	printf("dirname(%s): %s\n", p2.getpath_c(), p2.dirname_c());
	printf("dirname(%s): %s\n", p3.getpath_c(), p3.dirname_c());
	printf("dirname(%s): %s\n", p4.getpath_c(), p4.dirname_c());

	printf("---\n");

	printf("exists(%s): %d\n", p1.getpath_c(), p1.exists());
	printf("exists(%s): %d\n", p2.getpath_c(), p2.exists());
	printf("exists(%s): %d\n", p3.getpath_c(), p3.exists());
	printf("exists(%s): %d\n", p4.getpath_c(), p4.exists());

	printf("---\n");
	
	printf("isdir(%s): %d\n", p1.getpath_c(), p1.isdir());
	printf("isdir(%s): %d\n", p2.getpath_c(), p2.isdir());
	printf("isdir(%s): %d\n", p3.getpath_c(), p3.isdir());
	printf("isdir(%s): %d\n", p4.getpath_c(), p4.isdir());

	printf("---\n");
	
	printf("%s + %s = %s", p3.getpath_c(), p4.getpath_c(), (p3+p4).getpath_c());
	printf("%s + %s = %s", p1.getpath_c(), p2.getpath_c(), (p1+p2).getpath_c());

	printf("---\n");
	
	printf("%s += %s = %s", p3.getpath_c(), p4.getpath_c(), (p3+=p4).getpath_c());
	printf("%s += %s = %s", p1.getpath_c(), p2.getpath_c(), (p1+=p2).getpath_c());
	
	printf("---\n");

	printf("%s.append(%s): ", p3.getpath_c(), p4.getpath_c());
	printf("%s\n", (p3.append(p4)).getpath_c());
	printf("now %s.append(%s)\n", p3.getpath_c(), p4.getpath_c());
	
	return 0;
}
