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
#include <sys/stat.h>

using namespace std;

/*!
 @brief Implements easier path-handling, loosely based on Python's os.path
 */
class Path {
private:
	string path;				//!< Full path
	string sep;					//!< Directory seperator (will never change runtime) @todo cannot make this const? foamctrl.cc gives ../libsiu/path++.h: In member function ‘Path& Path::operator=(const Path&)’:	../libsiu/path++.h:31: error: non-static const member ‘const std::string Path::sep’, can't use default assignment operator
	string extsep;			//!< Extension seperator (will never change runtime) @todo cannot make this const? 
	
	FILE *fd;						//!< Used for reading/writing to a file
		
public:
	// Path(const char *p); //!< @todo implemeting this causes error in config-test? config-test.cc:48: error: call of overloaded ‘write(const char [19])’ is ambiguous
	Path(const string &p); //!< New Path from string
	Path(const Path &p); //!< New Path from Path
	Path();							//!< New empty Path
	~Path() { ; }				//!< Nothing to destruct here
	
	inline bool operator==(const string &b) const { return (b == path); }
	inline bool operator==(const Path &b) const { return (b.str() == path); }
	inline bool operator==(Path &b) const { return (b.str() == path); }
	Path operator+(const Path& rhs) const;
	Path operator+(const string& rhs) const;
	Path operator+=(const Path& rhs);
	Path operator+=(const string& rhs);
	
	Path append(const string &p1); //!< Append string p1 to current path 
	Path append(const Path &p1) { return append(p1.str()); } //!< Append Path p1 to the current path
	
	Path basename() const { return Path(path.substr(path.rfind(sep)+1)); } //!< Get the filename from the path
	Path dirname() const { return Path(path.substr(0, path.rfind(sep)+1)); } //!< Get the dirname from the path
	
	int length() const { return path.length(); } //!< Get path length (i.e. length of string)
	
	string str() const { return path; } //!< Return the path as string
	const char *c_str() const { return path.c_str(); } //!< Return the path as c_str()

	string get_ext() const { return path.substr(path.rfind(extsep)+1); } //!< Get the file extension from the path

	bool stat(const mode_t test_mode) const; //!< Test whether Path has mode test_mode (see stat(2))
	bool access(const int test_mode) const; //!< Test whether Path has mode test_mode (see access(2))
	
	bool isset() const { return (this->length() != 0); }
	bool exists() const { return !this->access(F_OK); }
	bool r() const { return !this->access(R_OK); }
	bool w() const { return !this->access(W_OK); }
	bool x() const { return !this->access(X_OK); }
	bool rwx() const { return !this->access(R_OK | W_OK | X_OK); }
	bool isabs() const { return ((path.substr(0,1)) == sep); }
	bool isrel() const { return !isabs(); }
	bool isdir() const { return stat(S_IFDIR); }
	bool isfile() const { return stat(S_IFREG); }
	bool islink() const { return stat(S_IFLNK); }
	
	FILE *fopen(string mode="a+");
	int fprintf(const string msg);
	int fprintf(const char *format, ... );
	int fclose() { return ::fclose(fd); }
	FILE *get_fd() { return fd; }
};

#endif // HAVE_PATHPP_H
