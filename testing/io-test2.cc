/*
 io-test2.cc -- Test console I/O
 Copyright (C) 2010 Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>
 
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

	printf("io-test2.cc::Test printing at different error levels...\n");
	for (int i=0; i<IO_MAXLEVEL+1; i++) {
		io = new Io(i);
		printf("io-test2.cc:: Verbosity level = %d\n", io->getVerb());

		io->msg(IO_ERR | IO_NOLF, "Error ");
		io->msg(IO_WARN | IO_NOLF, "Warn ");
		io->msg(IO_INFO | IO_NOLF, "Info ");
		io->msg(IO_XNFO | IO_NOLF, "Xnfo ");
		io->msg(IO_DEB1 | IO_NOLF, "Debug1 ");
		io->msg(IO_DEB2 | IO_NOLF, "Debug2 ");
		printf("\n");
		delete io;
	}
	
	printf("io-test2.cc::Test level incrementing and decrementing...\n");
	io = new Io(1);
	
	if (io->getVerb() != 1) {
		printf("io-test2.cc::ERROR: initial level wrong!\n");
		return -1;
	}
	
	io->setVerb(2);
	if (io->getVerb() != 2) {
		printf("io-test2.cc::ERROR: Cannot set level with int!\n");
		return -1;
	}
	io->setVerb("2");
	if (io->getVerb() != 2) {
		printf("io-test2.cc::ERROR: Cannot set level with string!\n");
		return -1;
	}
	
	// Increment a few times
	io->incVerb();
	io->incVerb();
	
	if (io->getVerb() != 4 && io->getVerb() != IO_MAXLEVEL) {
		printf("io-test2.cc::ERROR: incrementing failed!\n");
		return -1;
	}

	for (int i=0; i<IO_MAXLEVEL; i++) 
		io->incVerb();

	if (io->getVerb() != IO_MAXLEVEL) {
		printf("io-test2.cc::ERROR: maxlevelcap failed failed!\n");
		return -1;
	}
	
	// Decrement more
	io->decVerb();
	io->decVerb();
	
	if (io->getVerb() != IO_MAXLEVEL-2) {
		printf("io-test2.cc::ERROR: decVerb failed failed!\n");
		return -1;
	}
	
	io->decVerb();
	io->decVerb();
	io->decVerb();
	io->decVerb();
	io->decVerb();
	io->decVerb();
	io->decVerb();
	io->decVerb();
	io->decVerb();
	io->decVerb();
	
	if (io->getVerb() != 1) {
		printf("io-test2.cc::ERROR: lowlevelcap failed failed!\n");
		return -1;
	}
	
	delete io;
	
	printf("io-test2.cc::Succes!\n");
	return 0;
}

