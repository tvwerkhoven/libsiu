/*
 perflogger.h -- measure monotonic time duration in different steps
 
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

#ifndef HAVE_PERFLOGGER_H
#define HAVE_PERFLOGGER_H

#include "autoconfig.h"

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#include <vector>

#include <sigc++/signal.h>
#include <pthread++.h>

using namespace std;

class PerfLog {
private:
	const size_t nhist;					//!< Length of history to remember (default 100)
	
	struct timeval lastlog;			//!< Last log entry (to measure interval)
	double interval;						//!< Performance averaging interval

	size_t totaliter;						//!< Total number of iterations done
	
	pthread::thread logthr;			//!< Logger thread
	pthread::mutex mutex;				//!< Data access mutex
	
	const size_t nstages;				//!< Number of stages to log for (size for lastlat etc.)
	bool init;									//!< Have we initialized in this interval?
	
	vector< size_t > avgcount;		//!< Counter for the number of measurements we have
	vector< struct timeval > last;	//!< Last measured timestamp (previous iteration)
	vector< struct timeval > minlat; //!< Min latency for each stage in the last interval
	vector< struct timeval > maxlat; //!< Max latency for each stage in the last interval
	vector< struct timeval > sumlat; //!< Summed latency for each stage in the last interval
	
	void logger();							//!< Performance logger thread
	void reset_logs();					//!< Reset logs
	
public:
  PerfLog(size_t nstages, double i=1.0, size_t nh=100);
	~PerfLog();
	
	bool do_print;							//!< Whether or not to print performance every interval seconds.
	bool do_callback;						//!< Whether or not to callback slot_report() every interval seconds.
	
	bool addlog(size_t stage);		//!< Add log entry for specific stage
	bool setinterval(double i=1.0); //!< Set new update interval (in seconds)
	
	void print_report(FILE *stream=stdout, int whichreport=0); //!< Print last report to terminal
	
	sigc::slot<void, double, vector< struct timeval >, vector< struct timeval >, vector< struct timeval >, vector< size_t > > slot_report; //!< Slot for performance reporting, will be called as slot_report(interval, last, minlat, maxlat, sumlat, avgcount);
};

#endif // HAVE_PERFLOGGER_H
