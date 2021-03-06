/*
 path++.h -- easy class-based path handling for C++
 Copyright (C) 2010  Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>
 
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
#include <stdio.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <unistd.h>

#include "path++.h"

using namespace std;

/* 
 * Constructors / destructor
 */

Path::Path(const string &p): path(p), sep("/"), extsep(".") { ; }
//! @todo get seperators from OS
Path::Path(const Path &p): path(p.str()), sep("/"), extsep(".") { ; }
Path::Path(): path(""), sep("/"), extsep(".") { ; }

Path::~Path() { ; }

/* 
 * Operator overloading
 */

Path Path::operator+(const Path &rhs) const {
	// + returns 'this' concatenated with 'rhs'. Should not change 'this', so make a new object
	Path out(*this);
	return out.append(rhs);
}

Path Path::operator+(const string &rhs) const {
	// + returns 'this' concatenated with 'rhs'. Should not change 'this', so make a new object
	Path out(*this);
	return out.append(rhs);
}

Path Path::operator+=(const Path &rhs) { return append(rhs); }
Path Path::operator+=(const string &rhs) { return append(rhs); }
Path Path::operator=(const string &rhs) { return operator=(Path(rhs)); }

/* 
 * Public methods
 */

Path Path::append(const string &p1) {
	// If p1 is nothing, return *this
	if (p1.length() == 0)
		return *this;
	// If our current path is empty, simply replace it with the appendix
	if (path.length() == 0) {
		path = p1;
		return *this;
	}
	
	// If p1 is absolute, replace our path with the new path, i.e. /some/path + /absolute/path = /absolute/path
	if (p1.substr(0,1) == sep) {
		path = p1;
		return *this;
	}
	
	// If p1 is not absolute (see above), add them together with a seperator if necessary
	if (path.substr(path.length()-1) == sep)
		path += p1;
	else
		path = path + sep + p1;
	
	return *this;
}

bool Path::stat(const mode_t test_mode) const {
	if (!exists())
		return false;
	
	struct stat buf;
	::stat(this->c_str(), &buf);
	return (buf.st_mode & test_mode);
}

bool Path::access(int test_mode) const {
	return ::access(this->c_str(), test_mode);
}
