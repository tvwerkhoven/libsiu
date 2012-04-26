/*
 sighhandle.cc -- handle signals
 
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

#include "autoconfig.h"

#include "pthread++.h"
#include "sighandle.h"
#include "format.h"
#include "utils.h"

#include <stdlib.h>
#include <signal.h>
#include <sigc++/signal.h>

SigHandle::SigHandle(bool blockall):
handled_signal(-1), ign_count(0), quit_count(0), max_quit_count(1) {
	DEBUGPRINT("%s", "init\n");
	sigset_t signal_set;
	
	// Block all signals if requested
	DEBUGPRINT("%s", "blocking signals...\n");
	if (blockall) {
		sigfillset(&signal_set);
		pthread_sigmask(SIG_BLOCK, &signal_set, NULL);
	}
	
	// create the signal handling thread (from pthread++.h)
	DEBUGPRINT("%s", "creating handler...\n");
	handler_thr.create(sigc::mem_fun(*this, &SigHandle::handler));
}

SigHandle::~SigHandle() {
	DEBUGPRINT("%s", "end\n");
	handler_thr.cancel();
	handler_thr.join();
}

void SigHandle::handler() {
	sigset_t signal_set;
	int sig=0;
	
	while (1) {
		// Wait for any signal
		DEBUGPRINT("%s", "waiting for any signal...\n");
		sigfillset(&signal_set);
		sigwait(&signal_set, &sig);
		
		{
			{ 
				pthread::mutexholder h(&sig_mutex);
				handled_signal = sig;
			}
			DEBUGPRINT("SigHandle::handler() got signal: %s\n", strsignal(sig));
			// Decide what to do with the signal
			switch (sig) {
					// These signals are dangerous, stop the program
				case SIGILL:					// malformed, unknown, or privileged instruction
				case SIGABRT:					// from abort()
				case SIGFPE:					// floating point exception
				case SIGSEGV:					// improper memory handling, segfault 
				case SIGBUS:					// bus error, for improper memory handling
					abort();
				case SIGINT:					// ctrl-c
				case SIGTERM:					// normal shutdown
					quit_count++;
					DEBUGPRINT("SigHandle::handler() quitting sig %d (#%zu)\n", 
									sig, quit_count);
					quit_func();
					
					// If quit signal is received twice or more, brutally exit
					if (quit_count > max_quit_count)
						exit(sig);
					break;
					// These signals are probably not fatal, ignore them
				case SIGPIPE:					// broken pipe (write to closed socket etc.)
				case SIGHUP:					// hangup (remote socket closes etc.)
				case SIGALRM:					// Alarm clock (POSIX)
				case SIGPROF:					// Profiling alarm clock (4.2 BSD)
				default:
					ign_count++;
					DEBUGPRINT("SigHandle::handler() ignoring sig %d (#%zu)\n", 
									sig, ign_count);
					ign_func();
					break;
				case SIGQUIT:
					throw format("Received SIGQUIT, throwing!");
			}
		}
	}
}

