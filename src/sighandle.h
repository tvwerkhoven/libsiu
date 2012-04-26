/*
 sighhandle.h -- handle signals
 
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

#ifndef HAVE_SIGHANDLE_H
#define HAVE_SIGHANDLE_H

#ifndef _GNU_SOURCE
// For strsignal(3) on Linux
#define _GNU_SOURCE
#endif
#include <sigc++/signal.h>
#include <string>
#include <string.h>

#include "pthread++.h"
#include "utils.h"

/*! Signal handling class 
 
 At the start (constructor), block all signals in main thread, which is 
 inherited in child threads. Then start a new signal handling thread in the 
 background which listens with sigwait() for any signal. Depending on the 
 signal received, call slot function ign_func() or quit_func(). 
 */
class SigHandle {
  int handled_signal;				//!< Holds the last handled signal
	
	size_t ign_count;					//!< Amount of ignore signals received
	size_t quit_count;				//!< Amount of quit signals received (used to check if quit is in progress)
	
	const size_t max_quit_count; //!< After this many quit signals, force a quit with exit(-1)
	
	pthread::mutex sig_mutex; //!< Mutex for handled_signal
	
	void handler();						//!< Signal handler routine, uses sigwait() to parse system signals
	pthread::thread handler_thr; //!< Thread for handler()
	
public:
	sigc::slot<void> ign_func;	//!< Slot to call for signals to be ignored (can be empty)
	sigc::slot<void> quit_func; //!< Slot to call for signals to quit on (can be empty, global stop function is better)
	
	size_t get_ign_count() { return ign_count; }
	size_t get_quit_count() { return quit_count; }
	bool is_quitting() { return (quit_count > 0); }
	
	int get_sig() { pthread::mutexholder h(&sig_mutex); return handled_signal; }
	std::string get_sig_info() { pthread::mutexholder h(&sig_mutex); return strsignal(handled_signal); }
	
  SigHandle(bool blockall=true);
	~SigHandle();
};

#endif // HAVE_SIGHANDLE_H
