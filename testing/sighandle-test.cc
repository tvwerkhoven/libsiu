/*
 sighandle-test.cc -- Test Sighandle class
 Copyright (C) 2011 Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>
 
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

#include <sigc++/signal.h>
#include "sighandle.h"

void quit_func() {
	printf("quit_func()\n");
}

void ign_func() {
	printf("ign_func()\n");
}

int main() {
	SigHandle sig;
	sig.quit_func = sigc::ptr_fun(&quit_func);
	sig.ign_func = sigc::ptr_fun(&ign_func);
	
	while (1)
		sleep(1);
}