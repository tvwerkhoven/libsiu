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

	printf("isabs(%s): %d\n", p1.as_cstr(), p1.isabs());
	printf("isabs(%s): %d\n", p2.as_cstr(), p2.isabs());
	printf("isabs(%s): %d\n", p3.as_cstr(), p3.isabs());
	printf("isabs(%s): %d\n", p4.as_cstr(), p4.isabs());
	
	printf("---\n");

	printf("basename(%s): %s\n", p1.as_cstr(), p1.basename().as_cstr());
	printf("basename(%s): %s\n", p2.as_cstr(), p2.basename().as_cstr());
	printf("basename(%s): %s\n", p3.as_cstr(), p3.basename().as_cstr());
	printf("basename(%s): %s\n", p4.as_cstr(), p4.basename().as_cstr());

	printf("---\n");
	
	printf("dirname(%s): %s\n", p1.as_cstr(), p1.dirname().as_cstr());
	printf("dirname(%s): %s\n", p2.as_cstr(), p2.dirname().as_cstr());
	printf("dirname(%s): %s\n", p3.as_cstr(), p3.dirname().as_cstr());
	printf("dirname(%s): %s\n", p4.as_cstr(), p4.dirname().as_cstr());

	printf("---\n");

	printf("exists(%s): %d\n", p1.as_cstr(), p1.exists());
	printf("exists(%s): %d\n", p2.as_cstr(), p2.exists());
	printf("exists(%s): %d\n", p3.as_cstr(), p3.exists());
	printf("exists(%s): %d\n", p4.as_cstr(), p4.exists());

	printf("---\n");
	
	printf("isdir(%s): %d\n", p1.as_cstr(), p1.isdir());
	printf("isdir(%s): %d\n", p2.as_cstr(), p2.isdir());
	printf("isdir(%s): %d\n", p3.as_cstr(), p3.isdir());
	printf("isdir(%s): %d\n", p4.as_cstr(), p4.isdir());

	printf("---\n");
	
	printf("%s + %s = %s", p3.as_cstr(), p4.as_cstr(), (p3+p4).as_cstr());
	printf("%s + %s = %s", p1.as_cstr(), p2.as_cstr(), (p1+p2).as_cstr());

	printf("---\n");
	
	printf("%s += %s = %s", p3.as_cstr(), p4.as_cstr(), (p3+=p4).as_cstr());
	printf("%s += %s = %s", p1.as_cstr(), p2.as_cstr(), (p1+=p2).as_cstr());
	
	printf("---\n");

	printf("%s.append(%s): ", p3.as_cstr(), p4.as_cstr());
	printf("%s\n", (p3.append(p4)).as_cstr());
	printf("now %s.append(%s)\n", p3.as_cstr(), p4.as_cstr());
	
	return 0;
}
