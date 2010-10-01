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
 @brief Implements easier path-handling, loosely based on Python's os.path
 */
class Path {
private:
	bool test_stat(string &p, mode_t test_mode); //!< Test whether path p has mode test_mode (see stat(2))
	
	bool isabs(string &p);
	bool exists(string &p);
	bool isdir(string &p);
	bool isfile(string &p);
	bool islink(string &p);

	string basename(string &p);	//!< Get the filename from the path
	string dirname(string &p);	//!< Get the directory name from the path

	string path;				//!< Full path
	string sep;					//!< Directory seperator

public:
	Path(const string &p); //!< New Path from string
	Path(const Path &p); //!< New Path from Path
	Path();							//!< New empty Path
	~Path() { ; }				//!< Nothing to destruct here
	
	inline bool operator == (const Path &b) const;
	Path operator+(const Path& rhs);
	
	Path append(const string &p1); //!< Append string p1 to current path 
	Path append(const Path &p1); //!< Append Path p1 to the current path
	
	string basename();	//!< Get the filename from the path
	const char *basename_c() { return basename().c_str(); }	//!< Get the filename from the path as c_str()
	string dirname();		//!< Get the directory name from the path
	const char *dirname_c() { return dirname().c_str(); }	//!< Get the directory name from the path as c_str()
	
	string getpath() const { return path; } //!< Return the path
	const char *getpath_c() const { return path.c_str(); } //!< Return the path as c_str()
	
	bool exists();
	bool isabs();
	bool isdir();
	bool isfile();
	bool islink();
};

#endif // HAVE_PATHPP_H