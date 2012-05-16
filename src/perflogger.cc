/*
 monotime.cc -- measure monotonic time duration
 g++ -I/Users/tim/workdocs/dev/libsiu/src/ `pkg-config sigc++-2.0 --cflags --libs` monotime.cc -o monotime
 
 Copyright (C) 2011 Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
 This file is part of FOAM.
 
 FOAM is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 2 of the License, or
 (at your option) any later version.
 
 FOAM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with FOAM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "autoconfig.h"

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#include <vector>

#include <format.h>
#include <pthread++.h>

#include "perflogger.h"
#include "utils.h"

using namespace std;

PerfLog::PerfLog(const double i, const bool live, const bool print):
interval(i), totaliter(0), nstages(0), init(false), do_live(live), do_print(print), do_callback(true), do_alwaysupdate(false)
{
	// Pre-allocate memory in vectors (10 stages should be enough for most purposes, will be dynamically added if necessary)
	allocate(10);

	DEBUGPRINT("sizes %zu, %zu, %zu and %zu\n", last.size(), minlat.size(), maxlat.size(), sumlat.size());
	
	// Clear struct timeval's
	for (size_t i=0; i < last.size(); i++)
		timerclear(&(last.at(i)));
	reset_logs();
	
	// Start logger thread and return
	logthr.create(sigc::mem_fun(*this, &PerfLog::logger));
}

PerfLog::~PerfLog() {
	DEBUGPRINT("%s", "\n");
	// Stop logger thread
	do_live = false;
	//! @bug This does not work properly, thread carries on and locks program on exit
	logthr.cancel();
	logthr.join();
}

void PerfLog::allocate(const size_t size) {
	DEBUGPRINT("allocate(size=%zu)\n", size);
	last.resize(size);
	minlat.resize(size);
	maxlat.resize(size);
	sumlat.resize(size);
	avgcount.resize(size);
}

void PerfLog::reset_logs() {
	for (size_t i=0; i < sumlat.size(); i++) {
		timerclear(&(minlat.at(i)));
		timerclear(&(maxlat.at(i)));
		timerclear(&(sumlat.at(i)));
		avgcount.at(i) = 0;
	}
}

bool PerfLog::addlog(const size_t stage) {
	if (stage == 0)
		totaliter++;
	
	// Try to get mutex, otherwise abort (logger() needs mutex for reporting)
	pthread::mutexholdertry htry(&mutex);
	if (!htry.havelock()) {
		DEBUGPRINT("stage=%zu lockfail\n", stage);
		return false;
	}
	
	// Check if we've monitored this stage before (stage starts at 0)
	if (stage+1 > nstages)
		nstages = stage+1;
	// Check if memory is sufficient
	if (nstages > sumlat.size())
		allocate(nstages+5);
	
	// Initialize here, but only in stage 0 (otherwise do later)
	if (!init) {
		if (stage == 0) {
			gettimeofday(&(last[0]), 0);
			init = true;
			DEBUGPRINT("stage=%zu init\n", stage);
			return true;
		} else {
			return false;
		}
	}
	
	struct timeval now, diff, tmp;
	
	// Stage 0 is always compared with stage 0 at the previous iteration, other 
	// stages are compared with a stage before in the same iteration
	size_t cmpstage = stage-1;
	if (stage == 0) cmpstage = 0;
	
	// Get current time, calculate difference with previous, then store as previous
	gettimeofday(&now, 0);
	timersub(&now, &(last.at(cmpstage)), &diff);
	last.at(stage) = now;

	// Add current latency (diff) to sum of latencies, and count the number we've monitored this stage
	timeradd(&(sumlat.at(stage)), &diff, &tmp);
	sumlat.at(stage) = tmp;
	(avgcount.at(stage))++;
	
	DEBUGPRINT("stage=%zu nonfirst, avgcount[stage]=%zu, diff = %ld.%06ld\n",
						 stage, avgcount.at(stage),
						 (long int) diff.tv_sec, (long int) diff.tv_usec);
	
	// Update minimum & maximum latency if necessary
	if (timercmp(&(minlat.at(stage)), &diff, >) || 
			((minlat.at(stage)).tv_sec == 0 && (minlat.at(stage)).tv_usec == 0))
		minlat.at(stage) = diff;
	else if (timercmp(&(maxlat.at(stage)), &diff, <))
		maxlat.at(stage) = diff;
	
	return true;
}

void PerfLog::print_report(FILE *stream) {
	fprintf(stream, "PerfLog: In the last measurement, we got these latencies:\n");
	
	for (size_t i=0; i < nstages; i++) {
		string rep = "";
		rep += format("PerfLog: Stage[%zu]: #=%zu", i, avgcount.at(i));
		rep += format(", min: %ld.%06ld", 
							 (long int) minlat.at(i).tv_sec, (long int) minlat.at(i).tv_usec);
		rep += format(", max: %ld.%06ld", 
							 (long int) maxlat.at(i).tv_sec, (long int) maxlat.at(i).tv_usec);
		rep += format(", sum: %ld.%06ld", 
							 (long int) sumlat.at(i).tv_sec, (long int) sumlat.at(i).tv_usec);
		double sum = sumlat.at(i).tv_sec*1E6 + sumlat.at(i).tv_usec;
		rep += format(", avg: %.6f\n", sum/avgcount.at(i)/1e6);
		fprintf(stream, "%s", rep.c_str());
	}
}

void PerfLog::logger() {
	struct timeval now, next, diff;
	size_t lastiter=0;

	while (do_live) {
		gettimeofday(&lastlog, 0);
		{
			// Get mutex to work with data
			pthread::mutexholder h(&mutex);
			if (totaliter > lastiter || do_alwaysupdate) {
				if (do_print)
					print_report();
				if (do_callback)
					slot_report(interval, nstages, minlat, maxlat, sumlat, avgcount);
			}
			
			// Reset latencies
			reset_logs();
			// Last iteration that we updated is this one
			lastiter = totaliter;
		}
		
		// Sleep until next iteration
		if (interval > 0) {
			// Make sure each iteration takes at minimum 'interval' seconds:
			diff.tv_sec = (int) interval; // use (int) to floor() in case interval > 1
			diff.tv_usec = interval * 1.0e6;
			timeradd(&lastlog, &diff, &next);
			
			gettimeofday(&now, 0);
			timersub(&next, &now, &diff);
			if(diff.tv_sec >= 0)
				usleep(diff.tv_sec * 1.0e6 + diff.tv_usec);
		}
	}
	DEBUGPRINT("%s\n", "ending");
}
