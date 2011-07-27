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
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#include <string>

#include "libsiu-testing.h"

#include "config.h"

#define TMPFILE "/tmp/config-test"

void error(string what) {
	fprintf(stderr, "config-test.cc: ERROR for %s\n", what.c_str());
	exit(-1);
}

int main(int /* argc */, char *argv[]) {
	DEBUGPRINT("testing %s\n", argv[0]);
	
	config *cfg = new config;
	
	// SET CONFIG
	fprintf(stderr, "config-test.cc: setting config.\n");
	cfg->set("double", (double) 1.0);
	fprintf(stderr, "config-test.cc: setting (double) %g.\n", 1.0);
	cfg->set("bool", (bool) true);
	fprintf(stderr, "config-test.cc: setting (bool) %d.\n", true);
	
	cfg->set("uint16_t", (uint16_t) UINT16_MAX);
	fprintf(stderr, "config-test.cc: setting (uint16_t) %"PRIu16".\n", UINT16_MAX);
	cfg->set("uint32_t", (uint32_t) UINT32_MAX);
	fprintf(stderr, "config-test.cc: setting (uint32_t) %"PRIu32".\n", UINT32_MAX);
	cfg->set("uint64_t", (uint64_t) UINT64_MAX);
	fprintf(stderr, "config-test.cc: setting (uint64_t) %"PRIu64".\n", UINT64_MAX);
	
	cfg->set("int16_t", (int16_t) INT16_MAX);
	fprintf(stderr, "config-test.cc: setting (int16_t) %"PRIi16".\n", INT16_MAX);
	cfg->set("int32_t", (int32_t) INT32_MAX);
	fprintf(stderr, "config-test.cc: setting (int32_t) %"PRIi32".\n", INT32_MAX);
	cfg->set("int64_t", (int64_t) INT64_MAX);
	fprintf(stderr, "config-test.cc: setting (int64_t) %"PRIi64".\n", INT64_MAX);
	
	cfg->set("string", (std::string)"Hello world");
	fprintf(stderr, "config-test.cc: setting (std::string) Hello world\n");
	
	fprintf(stderr, "config-test.cc: writing to " TMPFILE "\n");
	cfg->write(TMPFILE);
	
	// READ CONFIG
	fprintf(stderr, "config-test.cc: reading from " TMPFILE "\n");
	config *cfgr = new config(TMPFILE);
	
	if (cfgr->getdouble("double") != 1.0) error("double");
	if (cfgr->getbool("bool") != true) error("bool");
	if (cfgr->getuint16("uint16_t") !=  (uint16_t) UINT16_MAX) error("uint16_t");
	if (cfgr->getuint32("uint32_t") != (uint32_t) UINT32_MAX) error("uint32_t");
	if (cfgr->getuint64("uint64_t") != (uint64_t) UINT64_MAX) error("uint64_t");
	if (cfgr->getint16("int16_t") != (int16_t) INT16_MAX) error("int16_t");
	if (cfgr->getint32("int32_t") != (int32_t) INT32_MAX) error("int32_t");
	if (cfgr->getint64("int64_t") != (int64_t) INT64_MAX) error("int64_t");
	if (cfgr->getstring("string") != (std::string)"Hello world") error("string");
	try {
		cfgr->getstring("doesntexist");
	}	catch(...) {
		// Ok, do nothing
		fprintf(stderr, "config-test.cc: ok\n");
	}
	//cfgr->write("./config-test2.tmp");
	
	delete cfg;
	delete cfgr;
	
	// Test multiple writes to one file:
	DEBUGPRINT("%s", "Test multiple writes to one file...\n");
	
	config *cfga = new config(TMPFILE);
	config *cfg1 = new config(TMPFILE, "cfg1");
	config *cfg2 = new config(TMPFILE, "cfg2");
	
	cfga->set("name", (std::string)"cfga");
	cfga->set("size", 1);

	cfg1->set("name", (std::string)"cfg1");
	cfg1->set("size", 2);

	cfg2->set("name", (std::string)"cfg2");
	cfg2->set("size", 3);
	
	cfga->update(cfg1);
	cfga->update(cfg2);
	cfga->write();
	
	config *cfgre = new config(TMPFILE);
	map<string, string> all = cfgre->getall();

	for(map<string, string>::const_iterator i = all.begin(); i != all.end(); ++i)
		DEBUGPRINT("%s = %s\n", i->first.c_str(), i->second.c_str());

	DEBUGPRINT("everything %s\n", "ok");
	return 0;
}
