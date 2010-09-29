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
dir(""), file(""), sep("/")
{
	//! @todo get seperator from OS
	dir = p;
	file = p;
}

Path::Path():
dir(""), file(""), sep("/")
{ ; }

bool Path::isabs() { return isabs(dir); }
bool Path::isabs(string p) { return ((p.substr(0,1)) == sep); }

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

string Path::basename(string p) {
	size_t pos = p.rfind(sep);
	if (pos != string::npos)
		return p.substr(pos);
	else
		return "";
}

//Path::dirname(string p) {
//	
//}
//
//Path::dirname() {
//	return dirname(join(dir, file));
//}


