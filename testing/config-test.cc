/*
 config-test.cc -- Test Configuration file parsing
 Copyright (C) 2010 Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
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
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <string>

#include "config.h"

int main() {
	fprintf(stderr, "config-test.cc\n");
	
	config *cfg = new config;
	
	// SET CONFIG
	fprintf(stderr, "config-test.cc: setting config.\n");
	cfg->set("double", (double) 1.0);
	cfg->set("bool", (bool) true);
	
	cfg->set("uint16_t", (uint16_t) UINT16_MAX);
	cfg->set("uint32_t", (uint32_t) UINT32_MAX);
	cfg->set("uint64_t", (uint64_t) UINT64_MAX);
	
	cfg->set("int16_t", (int16_t) INT16_MAX);
	cfg->set("int32_t", (int32_t) INT32_MAX);
	cfg->set("int64_t", (int64_t) INT64_MAX);
	
	cfg->set("string", (std::string)"Hello world");
	
	fprintf(stderr, "config-test.cc: writing to ./config-test1.tmp\n");
	cfg->write("./config-test1.tmp");
	
	// READ CONFIG
	fprintf(stderr, "config-test.cc: reading from ./config-test1.tmp\n");
	config *cfgr = new config("./config-test1.tmp");
	
	cfgr->getdouble("double");
	cfgr->getbool("bool");
	cfgr->getuint16("uint16_t");
	cfgr->getuint32("uint32_t");
	cfgr->getuint64("uint64_t");
	cfgr->getint16("int16_t");
	cfgr->getint32("int32_t");
	cfgr->getint64("int64_t");
	cfgr->getstring("string");
	try {
		cfgr->getstring("doesntexist");
	}	catch(...) {
		fprintf(stderr, "config-test.cc: success!\n");
	}
	cfgr->write("./config-test2.tmp");
	
	delete cfg;
	delete cfgr;
	
	return 0;
}
