/*
 parse-test.cc -- test parsing functions in format.h 
 Copyright (C) 2010 Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>
 
 This file is part of FOAM.
 
 FOAM is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 2 of the License, or
 (at your option) any later version.
 
 FOAM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with FOAM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "format.h"
#include <stdio.h>
#include <string>

using namespace std;

int main() {
	string cmd = "ok commands 2 set noise <float>; set mode <mode>;";
	string cp = cmd;
	string tmp;
	
	printf("Popping words:\n");
	while ((tmp = popword(cp)) != "") {
		printf("popped: '%s'\n", tmp.c_str());
	}
	cp = cmd;

	printf("Popping groups:\n");
	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp);
	printf("popped: '%s'\n", tmp.c_str());
	
	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp);
	printf("popped: '%s'\n", tmp.c_str());

	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp);
	printf("popped: '%s'\n", tmp.c_str());
	
	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp, ";");
	printf("popped: '%s'\n", tmp.c_str());

	printf("src: '%s'\n", cp.c_str());
	tmp = popword(cp, ";");
	printf("popped: '%s'\n", tmp.c_str());	


	return 0;
}