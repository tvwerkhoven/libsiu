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

#define DEBUGPRINT(fmt, ...) \
do { if (LIBSIU_DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
__LINE__, __func__, __VA_ARGS__); } while (0)

#include "pthread++.h"
#include "sighandle.h"

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
			fprintf(stderr, "SigHandle::handler() got signal: %s\n", strsignal(sig));
			// Decide what to do with the signal
			switch (sig) {
					// These signals are not fatal, ignore them
				case SIGPIPE:
				case SIGFPE:
				case SIGHUP:
					ign_count++;
					fprintf(stderr, "SigHandle::handler() ignoring sig %d (#%zu)\n", 
									sig, ign_count);
					ign_func();
					break;
					// These signals are dangerous, stop the program
				case SIGQUIT:
				case SIGINT:
				default:
					quit_count++;
					fprintf(stderr, "SigHandle::handler() quitting sig %d (#%zu)\n", 
									sig, quit_count);
					quit_func();
					
					// If quit signal is received twice or more, brutally exit
					if (quit_count > max_quit_count)
						exit(-1);
					break;
			}
		}
	}
}

