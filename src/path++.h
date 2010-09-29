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

#ifndef HAVE_PATHPP_H
#define HAVE_PATHPP_H

#include <string>

using namespace std;

/*!
 @brief Implements easier path-handling, based on Python's os.path
 */
class Path {
public:
	Path(string p);
	Path();
	~Path() { ; }
	
	string path;				//!< Full path
	string sep;					//!< Directory seperator
		
	//string append(string p1);	//!< Append p1 to current path
	string join(string &p1, string &p2); //!< Join p1 and p2
	
	string basename(string &p);	//!< Get the filename from the path
	string basename();	//!< Get the filename from the path
	string dirname(string &p);	//!< Get the directory name from the path
	string dirname();		//!< Get the directory name from the path
	
//	string abspath();
	
	bool exists();
	bool exists(string &p);
	bool isabs();
	bool isabs(string &p);
//	bool isdir();
//	bool isfile();
//	bool isrel();
};

#endif // HAVE_PATHPP_H