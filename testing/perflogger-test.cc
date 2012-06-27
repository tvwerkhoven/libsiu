/*
 perflogger-test.cc -- Test path++ library
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

#include <string>
#include <vector>

#include <sigc++/signal.h>

#include <format.h>

#include "perflogger.h"

using namespace std;

void log_callback(double interval, 
									size_t nstages,
									vector< struct timeval > minlat,
									vector< struct timeval > maxlat,
									vector< struct timeval > sumlat, 
									vector< size_t > avgcount) {
	printf("log_callback!\n");
	
	FILE *stream = stdout;
	
	fprintf(stream, "log_callback: In the last %g seconds, we got these %zu latencies:\n", interval, nstages);
	
	for (size_t i=0; i < nstages; i++) {
		string rep = "";
		rep += format("log_callback: Stage[%zu]: #=%zu", i, avgcount.at(i));
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

int main(int, char **) {
	// Start new logger updating every 1.5 seconds, run live mode, print stats during live mode
	PerfLog logger(1.5, true, true);
	logger.slot_report = sigc::ptr_fun(log_callback);
	
	usleep(0.5 * 1E6);
	
	printf("Test 1 start...\n");
	printf("==============================================================================\n");

	// Start 'work'
	for (int i=0; i<5; i++) {
		printf("work1\n");
		logger.addlog(0);
		usleep(0.3 * 1E6);
		
		printf("work2\n");
		logger.addlog(1); // This piece of 'work' took 0.3 seconds
		usleep(0.1 * 1E6);
		
		printf("work3\n");
		logger.addlog(2);
		usleep(0.2 * 1E6);
		
		printf("work4\n");
		logger.addlog(3);
		usleep(0.01 * 1E6);
		
		printf("work5\n");
		logger.addlog(4);
		usleep(0.5 * 1E6);
		
		printf("work6\n");
		logger.addlog(5);
	}
	
	printf("Test 2 start...\n");
	printf("==============================================================================\n");
	// Start new logger updating every 1.5 seconds, run live mode, print stats during live mode
	PerfLog logger2(1.5, true, true);
	
	// Start 'work'
	for (int i=0; i<2000; i++) {
		logger2.addlog("work0 0.011");
		usleep(0.003 * 1E6 * (drand48()*0.1 + 1));
		
		logger2.addlog("work1 0.003"); // This piece of 'work' took 0.3 seconds
		usleep(0.001 * 1E6 * (drand48()*0.1 + 1));
		
		logger2.addlog("work2 0.001");
		usleep(0.002 * 1E6 * (drand48()*0.1 + 1));
		
		logger2.addlog("work3 0.002");
		usleep(0.0001 * 1E6 * (drand48()*0.1 + 1));		
		
		logger2.addlog("work4 0.0001");
		usleep(0.005 * 1E6 * (drand48()*0.1 + 1));		
		
		logger2.addlog("work5 0.005");
	}
  
	return 0;
}

