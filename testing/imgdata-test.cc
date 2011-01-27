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
	printf("imgdata-test.cc\n");
	
	// Init bare ImgData class, check for features
	Io io(4);
	ImgData im1(io);
	
	bool t(1);
	bool f(0);
	printf("imgdata-test.cc: bool: %d %d\n", t, f);

	printf("imgdata-test.cc: imgdata features:\n");
	printf("imgdata-test.cc: havegsl: %d\n", im1.havegsl);
	printf("imgdata-test.cc: haveics: %d\n", im1.haveics);
	printf("imgdata-test.cc: havefits: %d\n", im1.havefits);
	printf("imgdata-test.cc: havepgm: %d\n", im1.havepgm);
	
	if (argc > 1) {
		printf("imgdata-test.cc: using file '%s'\n", argv[1]);
		
		// Try to read file
		ImgData im2(io, argv[1]);
		
		// Save as different formats
		im2.writedata("imgdata-test-out.fits", ImgData::FITS, true);
		im2.writedata("imgdata-test-out.gsl", ImgData::GSL, true);
		im2.writedata("imgdata-test-out.pgm", ImgData::PGM, true);
		im2.writedata("imgdata-test-out.ics", ImgData::ICS, true);
		
		// Re-load files
		ImgData im2a(io, "imgdata-test-out.fits");
		ImgData im2b(io, "imgdata-test-out.gsl");
		ImgData im2c(io, "imgdata-test-out.pgm");
		ImgData im2d(io, "imgdata-test-out.ics");
		
	} else {
		printf("imgdata-test.cc: making from data\n");
		// Make random data
	}
	
	return 0;
}