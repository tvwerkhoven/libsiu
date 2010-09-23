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
		printf("==== Level = %d = %d\n", i, io->getVerb());

		io->msg(IO_ERR, "Error");
		io->msg(IO_WARN, "W");
		io->msg(IO_INFO, "I");
		io->msg(IO_XNFO, "X");
		io->msg(IO_DEB1, "1");
		io->msg(IO_DEB2, "2");
		delete io;
	}

	io = new Io(0);
	printf("==== Level = %d = %d\n", 0, io->getVerb());
	for (int i=0; i<10; i++) 
		io->incVerb();
	printf("==== Level = %d = %d\n", 10, io->getVerb());
	delete io;

	io = new Io(5);
	for (int i=0; i<10; i++) 
		io->decVerb();
	printf("==== Level = %d = %d\n", -5, io->getVerb());
	delete io;

	return 0;
}

