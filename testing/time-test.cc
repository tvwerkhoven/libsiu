/*
 time-test.cc -- Test time++ library
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

#include "time++.h"

#include "libsiu-testing.h"

int main(int argc, char *argv[]) {
	DEBUGPRINT("testing %s\n", argv[0]);
	
	Time t0(0);
	Time t1(123);

	Time t2(123456789,0.12345678);
	Time t3(123456789+3600,0.12345678);
	
	// Regular time
	DEBUGPRINT("t0: %s\n", t0.c_str());
	DEBUGPRINT("t1: %s\n", t1.c_str());
	DEBUGPRINT("t2: %s\n", t2.c_str());
	DEBUGPRINT("t3: %s\n", t3.c_str());
	
	// Difference
	DEBUGPRINT("t1 - t0 = %s\n", (t1 - t0).c_str());
	DEBUGPRINT("t4 - t3 = %s\n", (t3 - t2).c_str());
}
