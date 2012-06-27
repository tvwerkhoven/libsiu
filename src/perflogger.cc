/*
 monotime.cc -- measure monotonic time duration
 g++ -I/Users/tim/workdocs/dev/libsiu/src/ `pkg-config sigc++-2.0 --cflags --libs` monotime.cc -o monotime
 
 Copyright (C) 2011 Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>
 
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
#include <math.h>

#include <vector>

#include <format.h>
#include <pthread++.h>

#include "perflogger.h"
#include "utils.h"

using namespace std;

PerfLog::PerfLog(const double i, const bool live, const bool print):
interval(i), totaliter(0), init(false), do_live(live), do_print(print), do_callback(true), do_alwaysupdate(false)
{
	// Pre-allocate memory in vectors (10 stages should be enough for most purposes, will be dynamically added if necessary)
	allocate(10);

	DEBUGPRINT("sizes %zu, %zu, %zu %zu and %zu\n", last.size(), minlat.size(), maxlat.size(), sumlat.size(), sumsqlat.size());
	
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
	sumsqlat.resize(size);
	avgcount.resize(size);
}

void PerfLog::reset_logs() {
	for (size_t i=0; i < sumlat.size(); i++) {
		timerclear(&(minlat.at(i)));
		timerclear(&(maxlat.at(i)));
		timerclear(&(sumlat.at(i)));
		sumsqlat.at(i) = 0;
		avgcount.at(i) = 0;
	}
}

bool PerfLog::addlog(const string stagename) {
	DEBUGPRINT("PerfLog::addlog(%s)\n", stagename.c_str());
	// Check if we've monitored this stage before (stage starts at 0)
	size_t stageidx=0;
	for (stageidx=0; stageidx < stagenames.size() && stagenames[stageidx] != stagename; stageidx++) { ; }

	DEBUGPRINT("PerfLog::addlog(%s) idx = %zu\n", stagename.c_str(), stageidx);

	// If idx is the length of stagenames, 'stagename' was not stored before, add it
	if (stageidx == stagenames.size()) {
		DEBUGPRINT("%s", "PerfLog::addlog() adding!\n");
		stagenames.push_back(stagename);
	}

	// Check if this is the first stage (in that case increase loopcount)
	if (stageidx == 0)
		totaliter++;

	// Check if memory is sufficient
	if (stagenames.size() > sumlat.size())
		allocate(stagenames.size()+5);

	// Try to get mutex, otherwise abort (logger() needs mutex for reporting)
	pthread::mutexholdertry htry(&mutex);
	if (!htry.havelock()) {
		DEBUGPRINT("stage[%zu]=%s lockfail\n", stageidx, stagename.c_str());
		return false;
	}
	
	// Initialize here, but only in stage 0 (otherwise do later)
	if (!init) {
		if (stageidx == 0) {
			gettimeofday(&(last[0]), 0);
			init = true;
			DEBUGPRINT("stage[%zu]=%s init\n", stageidx, stagename.c_str());
			return true;
		} else {
			return false;
		}
	}
	
	struct timeval now, diff, tmp;
	
	// Stage 0 is always compared with stage 0 at the previous iteration, other 
	// stages are compared with a stage before in the same iteration
	size_t cmpstage = stageidx-1;
	if (stageidx == 0) cmpstage = 0;
	
	// Get current time, calculate difference with previous, then store this timestamp in last
	gettimeofday(&now, 0);
	timersub(&now, &(last.at(cmpstage)), &diff);
	last.at(stageidx) = now;
	
	// Add current latency (diff) to sum of latencies
	timeradd(&(sumlat.at(stageidx)), &diff, &tmp);
	sumlat.at(stageidx) = tmp;

	// Calculate diff**2, then store as double (necessary for precision)
	double sumsq = diff.tv_sec + ((double) diff.tv_usec)/1E6;
	sumsqlat.at(stageidx) += (sumsq*sumsq);
//	sumsq *= sumsq;
//	diff.tv_sec = (time_t) floor(sumsq);
//	diff.tv_usec = (sumsq*1E6 - floor(sumsq)*1E6);
//	
//	timeradd(&(sumsqlat.at(stageidx)), &diff, &tmp);
//	sumsqlat.at(stageidx) = tmp;
	
//	printf("stage: %zu, sum: %ld.%06ld, sumsq: %ld.%06ld\n", stageidx, 
//				 (long int) sumlat.at(stageidx).tv_sec, (long int) sumlat.at(stageidx).tv_usec,
//				 (long int) sumsqlat.at(stageidx).tv_sec, (long int) sumsqlat.at(stageidx).tv_usec);

	// count the number we've monitored this stage
	(avgcount.at(stageidx))++;
	
	DEBUGPRINT("stage=%zu nonfirst, avgcount[stage]=%zu, diff = %ld.%06ld\n",
						 stageidx, avgcount.at(stageidx),
						 (long int) diff.tv_sec, (long int) diff.tv_usec);
	
	// Update minimum & maximum latency if necessary
	timersub(&now, &(last.at(cmpstage)), &diff);
	if (timercmp(&(minlat.at(stageidx)), &diff, >) || 
			((minlat.at(stageidx)).tv_sec == 0 && (minlat.at(stageidx)).tv_usec == 0))
		minlat.at(stageidx) = diff;
	else if (timercmp(&(maxlat.at(stageidx)), &diff, <))
		maxlat.at(stageidx) = diff;
	
	return true;
}

bool PerfLog::addlog(const size_t stage) {
	addlog(format("%04zu", stage));
	return true;
}

void PerfLog::print_report(FILE *stream) {
	fprintf(stream, "PerfLog: In the last measurement, we got these latencies:\n");
	
	double sum0 = (double) sumlat.at(0).tv_sec + ((double) sumlat.at(0).tv_usec)/1E6;

	for (size_t i=0; i < stagenames.size(); i++) {
		string rep = "";
		rep += format("PerfLog: %zu/%zu %s: #=%zu", i, stagenames.size()-1, stagenames[i].c_str(), avgcount.at(i));

		double sum = (double) sumlat.at(i).tv_sec + ((double) sumlat.at(i).tv_usec)/1E6;

		// If this is not the first stage, also check the percentage we spend in this stage
		if (i != 0)
			rep += format(" (%.0f%%):", 100.0*sum/sum0);

		// Get sum^2/n
		double sumsq = sumsqlat.at(i);
		// Calculate (sum/n)^2
		double sqsum = (sum/avgcount.at(i)) * (sum/avgcount.at(i));
		// Calculate stddev = sqrt( sum^2/n - (sum/n)^2 )
		double stddev = sqrt((sumsq/avgcount.at(i)) - sqsum);

		// Print average with stddev
		rep += format(" avg: %.3g (±%.1g)", sum/avgcount.at(i), stddev);
		rep += format(", rate: %.3g", 1.0/(sum/avgcount.at(i)));

		fprintf(stream, "%s\n", rep.c_str());
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
					slot_report(interval, get_nstages(), minlat, maxlat, sumlat, avgcount);
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
