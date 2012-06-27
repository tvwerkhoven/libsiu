/*
 perflogger.h -- measure monotonic time duration in different steps
 
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

#ifndef HAVE_PERFLOGGER_H
#define HAVE_PERFLOGGER_H

#include "autoconfig.h"

#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#include <vector>
#include <map>

#include <sigc++/signal.h>
#include <pthread++.h>

using namespace std;

/*! @brief Log performance of anything in multiple stages.
 
 Provides means to log average, minimum and maximum latency of certain 
 looping computational steps. In each iteration of the loop to monitor, 
 addlog(<stage>) can be placed at various stages of the loop to log the time 
 it took since the previous addlog(<stage>) entry. If stage=0, it will be 
 compared with the last time addlog(0) was called, i.e. the previous loop.
 
 Reports can be printed live at regular intervals (through the logger() 
 thread), or can be manually printed with print_report().
 
 As an example, consider this loop:
 
 while (true) {
	out1 = func1(...)
  out2 = func2(out1)
  func3(out3)
 }
 
 To monitor the performance of this loop, simply place addlog() entries at 
 desired locations.

 while (true) {
   addlog("init stage")
   out1 = func1(...)
   addlog("func1()")
   out2 = func2(out1)
   addlog("func2()")
   func3(out3)
	 addlog("func3()")
 }
 
 When calling print_report(), the minimum, maximum and average latency will be 
 printed for the code between addlog(i) and addlog(i+1), and between 
 consecutive calls of addlog(0).
 
 Note that this code might have some overhead.
 
 N.B. This class struct timeval to store each latency, meaning that the 
 resolution is 1µs. If your iterations are faster than 10µs this code will 
 give poor results.
 */
class PerfLog {
private:
//	const size_t nhist;					//!< Length of history to remember (default 100)
	
	struct timeval lastlog;			//!< Last log entry (to measure interval in logthr)
	double interval;						//!< Performance averaging interval

	size_t totaliter;						//!< Total number of iterations done
	
	pthread::thread logthr;			//!< Logger thread
	pthread::mutex mutex;				//!< Data access mutex
	
	bool init;									//!< Is this the first call? Then only note the time and return.
	bool do_live;								//!< Print performance live in logger()
	
	vector< size_t > avgcount;		//!< Counter for the number of measurements we have
	vector< struct timeval > last;	//!< Last measured timestamp (previous iteration)
	vector< struct timeval > minlat; //!< Min latency for each stage in the last interval
	vector< struct timeval > maxlat; //!< Max latency for each stage in the last interval
	vector< struct timeval > sumlat; //!< Summed latency for each stage in the last interval
	vector< double > sumsqlat; //!< Summed squared latency (in seconds) for each stage in the last interval (to calculate standard deviation)
	
	vector<string> stagenames;	//!< List of names for each stage
	
	void logger();							//!< Performance logger thread
	void reset_logs();					//!< Reset logs
	void allocate(size_t size);	//!< (re-)allocate memory for logging
	
public:
	PerfLog(const double i=1.0, const bool live=false, const bool print=false);
	~PerfLog();
	
	bool do_print;							//!< Whether or not to print performance every interval seconds [false]
	bool do_callback;						//!< Whether or not to callback slot_report() every interval seconds [true]
	bool do_alwaysupdate;				//!< Always update, even if no iterations were logged since last update [false]
	
	size_t get_nstages() const { return stagenames.size(); }
	
	bool addlog(const size_t stage);		//!< Add log entry for specific stage, with name for this stage
	bool addlog(const string stagename);
	bool setinterval(double i=1.0); //!< Set new update interval (in seconds)
	
	void print_report(FILE *stream=stdout); //!< Print last report to some stream
	
	sigc::slot<void, double, size_t, vector< struct timeval >, vector< struct timeval >, vector< struct timeval >, vector< size_t > > slot_report; //!< Slot for performance reporting, will be called as slot_report(interval, last, minlat, maxlat, sumlat, avgcount);
};

#endif // HAVE_PERFLOGGER_H
