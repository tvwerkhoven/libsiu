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

int main() {
	Path p1("/absolute/path/file1");
	Path p2("relative/path/file1");
	Path p3("just_a_file");
	Path p3("just_a_file.extension");

	printf("isabs(%s): %d\n", p1.path;, p1.isabs());
	printf("isabs(%s): %d\n", p2.path;, p2.isabs());
	printf("isabs(%s): %d\n", p3.path;, p3.isabs());
	
	return 0;
}
