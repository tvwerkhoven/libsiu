/*
 csv-test.cc -- Test Csv library
 Copyright (C) 2011 Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
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
#include <vector>

#include "csv.h"

#include "libsiu-testing.h"

int main(int argc, char *argv[]) {
	DEBUGPRINT("testing %s\n", argv[0]);
	
	vector<double> line;
	
	for (int i = 0; i < 10; i++)
		line.push_back(drand48());
	
	Csv empty;
	empty.csvdata.push_back(line);
	empty.csvdata.push_back(line);
	empty.csvdata.push_back(line);
	
	DEBUGPRINT("writing %s\n", "csv-test-empty.csv");
	if (!empty.write("csv-test-empty.csv", "testing Csv."))
		DEBUGPRINT("error writing %s\n", "csv-test-empty.csv");
	
	DEBUGPRINT("reading %s\n", "csv-test-empty.csv");
	Csv reempty("csv-test-empty.csv");
	
	DEBUGPRINT("writing %s\n", "csv-test-reempty.csv");
	if (!reempty.write("csv-test-reempty.csv", "testing Csv 2."))
		DEBUGPRINT("error writing %s\n", "csv-test-reempty.csv");
		
	
	DEBUGPRINT("%s", "Done\n");
}
