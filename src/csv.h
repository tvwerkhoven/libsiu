/*
 csv.h -- simple CSV parser
 Copyright (C) 2011  Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
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

#ifndef HAVE_CSV_H
#define HAVE_CSV_H

#include <string>
#include <vector>
#include <gsl/gsl_matrix.h>

#include "path++.h"

/*! @brief Simple CSV parser for reading and writing
 
 This class can read a CSV file with specific options (see Csv()), and stores
 the data locally in csv.csvdata (which is a double vector). This data can 
 then (optionally) be altered, and saved to a (different) file.
 */
class Csv {
private:
	const char commpref;								//!< Comment prefix (like "#")
	const char wordsep;									//!< Word seperator (like ",")
	const char linesep;									//!< Line seperator (like '\n')
	
	string file;												//!< CSV file

public:
	/*! @brief Init new object based on 'file'
	 
	 @param [in] file Path to CSV file
	 */
	Csv(const string file, const char cpref='#', const char wsep=',', const char lsep='\n');
	/*! @brief Init new object based on data
	 
	 @param [in] *newdata Data for CSV object
	 @param [in] copy Copy *newdata or not
	 */
	Csv(vector< vector<double> > &newdata, const char cpref='#', const char wsep=',', const char lsep='\n', bool copy=false);
	/*! @brief Init new empy object

	 @param [in] cpref Comment prefix (default "#")
	 @param [in] wsep Word seperator (default ",")
	 @param [in] lsep Line seperator (default '\n')
	 */
	Csv(const char cpref='#', const char wsep=',', const char lsep='\n');
	~Csv();
	
	vector< vector<double> > csvdata;			//!< CSV data is stored here
	
	bool read(string f);									//!< Read 'file', store data in *data
	bool write(string f, const string &comment="", const bool app=true); //!< Write data to 'file', optionally with a comment line (prefix/header/other)
};


#endif // HAVE_CSV_H
