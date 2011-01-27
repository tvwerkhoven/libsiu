/*
 imgdata-test.cc -- Test imgdata class
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


#include <stdio.h>
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#include <string>

#include "imgdata.h"

int main(int argc, char *argv[]) {
	fprintf("imgdata-test.cc\n");
	
	// Init bare ImgData class, check for features
	Io io(4);
	im1 = ImgData(io);

	fprintf("imgdata-test.cc: imgdata features:\n");
	fprintf("imgdata-test.cc: havegsl: %d\n", im1.havegsl);
	fprintf("imgdata-test.cc: haveics: %d\n", im1.haveics);
	fprintf("imgdata-test.cc: havefits: %d\n", im1.havefits);
	fprintf("imgdata-test.cc: havepgm: %d\n", im1.havepgm);
	
	if (argc > 1) {
		fprintf("imgdata-test.cc: using file '%s'\n", argv[0]);
		
		// Try to read file
		
	} else {
		fprintf("imgdata-test.cc: making from data\n");
		// Make random data
	}
	
	return 0;
}
