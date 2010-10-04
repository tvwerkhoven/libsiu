/*
 io-test2.cc -- Test console I/O
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

	printf("Test printing at different error levels...\n");
	for (int i=0; i<IO_MAXLEVEL+1; i++) {
		io = new Io(i);
		printf("==== Error level = %d\n", io->getVerb());

		io->msg(IO_ERR | IO_NOLF, "Error ");
		io->msg(IO_WARN | IO_NOLF, "Warn ");
		io->msg(IO_INFO | IO_NOLF, "Info ");
		io->msg(IO_XNFO | IO_NOLF, "Xnfo ");
		io->msg(IO_DEB1 | IO_NOLF, "Debug1 ");
		io->msg(IO_DEB2 | IO_NOLF, "Debug2 ");
		printf("\n");
		delete io;
	}
	
	printf("Test level incrementing and decrementing...\n");
	io = new Io(1);
	
	if (io->getVerb() != 1)
		printf("ERROR: initial level wrong!\n");
	
	io->setVerb(2);
	if (io->getVerb() != 2)
		printf("ERROR: Cannot set level with int!\n");
	io->setVerb("2");
	if (io->getVerb() != 2)
		printf("ERROR: Cannot set level with string!\n");
	
	// Increment a few times
	io->incVerb();
	io->incVerb();
	io->incVerb();
	io->incVerb();
	io->incVerb();
	io->incVerb();
	io->incVerb();
	
	if (io->getVerb() != 7 && io->getVerb() != IO_MAXLEVEL)
		printf("ERROR: incrementing failed!\n");

	// Decrement more
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
	io->decVerb();
	io->decVerb();
	
	if (io->getVerb() != 1)
		printf("ERROR: decrementing failed!\n");
	
	delete io;
	
	printf("Done!\n");
	return 0;
}

