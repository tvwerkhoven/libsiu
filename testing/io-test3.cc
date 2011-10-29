/*
 io-test3.cc -- Test console I/O speed
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
#include "io.h"

int main() {
	printf("io-test3.cc::Test IO speed and blocking behaviour...\n");
	Io *io;
	io = new Io(10);

	while (true) {
		io->msg(IO_DEB2, "Debug2 message goes here");
	}
		
	printf("io-test2.cc::Succes!\n");
	return 0;
}

