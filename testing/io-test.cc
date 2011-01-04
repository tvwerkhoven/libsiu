/*
 io-test.cc -- Test console I/O
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

#include "io.h"

int main() {
	Io *io;

	for (int i=0; i<10; i++) {
		io = new Io(i);
		io->setdefmask(IO_THR);
		printf("io-test.cc: Level: set to %d, actual = %d\n", i, io->getVerb());

		io->msg(IO_ERR, "Error");
		io->msg(IO_WARN, "Warn");
		io->msg(IO_INFO, "Info");
		io->msg(IO_XNFO, "Xtra");
		io->msg(IO_DEB1, "deb1");
		io->msg(IO_DEB2, "deb2");
		delete io;
	}

	io = new Io(0);
	printf("io-test.cc: Level = %d = %d\n", 0, io->getVerb());
	if (io->getVerb() != 1) {
		printf("io-test.cc: Error: minlevel should be 1\n");
		return -1;
	}
	
	io->incVerb();
	io->incVerb();
	
	if (io->getVerb() != 3) {
		printf("io-test.cc: Error: incVerb() malfunctions.\n");
		return -1;
	}
	
	for (int i=0; i<IO_MAXLEVEL; i++) 
		io->incVerb();

	if (io->getVerb() != IO_MAXLEVEL) {
		printf("io-test.cc: Error: maxlevel should be %d.\n", IO_MAXLEVEL);
		return -1;
	}
	
	delete io;

	io = new Io(5);
	
	io->decVerb();
	io->decVerb();

	if (io->getVerb() != 3) {
		printf("io-test.cc: Error: decVerb() malfunctions.\n");
		return -1;
	}
	
	
	for (int i=0; i<IO_MAXLEVEL; i++) 
		io->decVerb();
	
	if (io->getVerb() != 1) {
		printf("io-test.cc: Error: minlevel should be 1\n");
		return -1;
	}
	
	delete io;
	printf("io-test.cc: test succesful!\n");

	return 0;
}

