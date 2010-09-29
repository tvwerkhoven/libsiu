/*
 path++.h -- easy class-based path handling for C++
 Copyright (C) 2010  Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
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

using namespace std;

Path::Path(string p):
path(""), sep("/")
{
	//! @todo get seperator from OS
	path = p;
}

Path::Path():
path(""), sep("/")
{ ; }

bool Path::isabs() { return isabs(path); }
bool Path::isabs(string &p) { return ((p.substr(0,1)) == sep); }

string Path::join(string &p1, string &p2) {
	if (p2.substr(0,1) == sep) {
		p1 = p2;
		return p1;
	}
	
	if (p1.substr(p1.length()-1) == sep)
		p1 += p2;
	else
		p1 = p1 + sep + p2;
	
	return p1;
}

string Path::basename(string &p) {
	return p.substr(p.rfind(sep)+1);
}

string Path::basename() { return basename(path); }

string Path::dirname(string &p) {
	return p.substr(0,p.rfind(sep)+1);
}

string Path::dirname() { return dirname(path); }

bool Path::exists(string &p) {
	return (!access(p.c_str(), F_OK));
}

bool Path::exists() { return exists(path); }

bool Path::isdir(string &p) {
	if (!exists(p))
		return false;
	
	struct stat buf;
	stat(constp.c_str(), &buf);
	
}