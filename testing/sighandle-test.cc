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
#include <unistd.h>
#include "sighandle.h"

void quit_func() {
	fprintf(stdout, "%s:%s\n", __FILE__, __FUNCTION__);
}

void ign_func() {
	fprintf(stdout, "%s:%s\n", __FILE__, __FUNCTION__);
}

int main() {
	fprintf(stdout, "%s: start\n", __FILE__);
	SigHandle sig;
	sig.quit_func = sigc::ptr_fun(&quit_func);
	sig.ign_func = sigc::ptr_fun(&ign_func);
	sig.max_quit_count = 10;
	
	pid_t thispid = getpid();

	usleep(0.5 * 1E6);
	// These signals should be caught by ign_func():
	fprintf(stdout, "%s: sending SIGPIPE=%d, SIGHUP=%d\n", __FILE__, SIGPIPE, SIGHUP);
	kill(thispid, SIGPIPE);
	usleep(0.1 * 1E6);
	kill(thispid, SIGHUP);
	
	usleep(0.5 * 1E6);
	// These signals should be caught by quit_func():
	fprintf(stdout, "%s: sending SIGINT=%d, SIGTERM=%d\n", __FILE__, SIGINT, SIGTERM);
	kill(thispid, SIGINT);
	usleep(0.1 * 1E6);
	kill(thispid, SIGTERM);
	
	usleep(0.5 * 1E6);
	int nquit = sig.get_quit_count();
	int nign = sig.get_ign_count();
	int ndef = sig.get_def_count();
	fprintf(stdout, "%s: got nquit: %d, nign: %d, ndef: %d\n", __FILE__, nquit, nign, ndef);

	
	// Check if we caught exceptions
	if (nquit == 2 && nign == 2) {
		fprintf(stdout, "%s: SUCCESS!\n", __FILE__);
		exit(0);
	} else {
		fprintf(stdout, "%s: FAIL!\n", __FILE__);		
		exit(-1);
	}
}