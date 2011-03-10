/*
 csv.cc -- simple CSV parser
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

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include "autoconfig.h"
#define DEBUGPRINT(fmt, ...) \
	do { if (LIBSIU_DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
	__LINE__, __func__, __VA_ARGS__); } while (0)

#include "path++.h"
#include "csv.h"

using namespace std;

// Constructors / destructors
Csv::Csv(const string newfile, const char cpref, const char wsep, const char lsep):
commpref(cpref), wordsep(wsep), linesep(lsep), 
file(newfile),
csvdata(NULL) {
	DEBUGPRINT("FILE:%s, '%c', '%c', '%c')\n", newfile.c_str(), commpref, wordsep, linesep);
	read(file);
	
}

Csv::Csv(vector< vector<double> > &newdata, const char cpref, const char wsep, const char lsep, bool copy):
commpref(cpref), wordsep(wsep), linesep(lsep),
csvdata(newdata) {
	DEBUGPRINT("DATA:..., '%c', '%c', '%c')\n", commpref, wordsep, linesep);
}

Csv::Csv(const char cpref, const char wsep, const char lsep):
commpref(cpref), wordsep(wsep), linesep(lsep),
csvdata(NULL) {
	DEBUGPRINT("VANILLA: '%c', '%c', '%c')\n", commpref, wordsep, linesep);
}

Csv::~Csv() {
}

// Read/write routines

bool Csv::read(string file) {
	DEBUGPRINT("%s\n", file.c_str());
	
	ifstream dataio(file.c_str());
	string buf, cell;
	vector<double> dataline;
	size_t linewidth = 0;
	
	csvdata.clear();
	
	while (getline (dataio, buf, linesep)) {
		const char c = '#';
		DEBUGPRINT("line = %s, comp = %d\n", buf.c_str(), buf[0] == commpref);
		// Line starts with 'commpref', skip it
		if (buf[0] == commpref)
			continue;
		
		// Read rest of the line, store in 
		stringstream bufstrm(buf);
		while(std::getline(bufstrm, cell, wordsep)) {
			dataline.push_back(strtod(cell.c_str(), NULL));
			DEBUGPRINT("element = %s\n", cell.c_str());
		}
		
		// If linewidth != the width of this line, not all lines have the same nr of elements, abort
		if (linewidth == 0)
			linewidth = dataline.size();
		else if (linewidth != dataline.size())
			return false;
		
		csvdata.push_back(dataline);
		dataline.clear();
	}
	
	DEBUGPRINT("read %d lines, each %d elems", csvdata.size(), csvdata[0].size());
	return true;
}

bool Csv::write(string file, const string &comment, const bool app) {
	DEBUGPRINT("%s, %s, %d\n", file.c_str(), comment.c_str(), app);
	if (csvdata.size() == 0)
		return false;

	ofstream filestr;
	if (app)
		filestr.open(file.c_str(), ios::out | ios::app); 
	else
		filestr.open(file.c_str(), ios::out); 
	
	if (filestr.fail())
		return false;

	if (comment != "")
		filestr << commpref << comment << linesep;
	
	for (size_t i=0; i<csvdata.size(); i++) {
		vector<double> dataline = csvdata[i];
		
		filestr << dataline[0];
		for (size_t j=1; j<dataline.size(); j++)
			filestr << wordsep << dataline[j];
		
		filestr << linesep;
	}
	
  filestr.close();
	
	DEBUGPRINT("wrote %d lines, each %d elems", csvdata.size(), csvdata[0].size());
	return true;
}
