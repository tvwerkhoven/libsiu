/*
 time++.cc -- easy class-based time handling for C++
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

#include <time.h>
#include <sys/time.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <stdint.h>
#include <inttypes.h>
#include <string>

#include "format.h"
#include "time++.h"
#include "utils.h"

using namespace std;

/* 
 * Constructors / destructor
 */

Time::Time() {
	update();
}

Time::Time(const Time &stamp) {
	epoch_t tmp = stamp.as_epoch_t();
  
	t_epoch.i = tmp.i;
	t_epoch.f = tmp.f;
	
	sync();
}

Time::Time(const intmax_t i, const long double f) {
	t_epoch.i = i;
	t_epoch.f = f;
	
	sync();	
}

Time::Time(const time_t stamp) {
	t_timeval.tv_sec = stamp;
	t_timeval.tv_usec = 0;
	
	t_epoch.i = t_timeval.tv_sec;
	t_epoch.f = 0.0;
	
	t_str_tm = gmtime(&(t_timeval.tv_sec));
}

Time::Time(const epoch_t stamp) {
	t_timeval.tv_sec = stamp.i;
	t_timeval.tv_usec = (suseconds_t) stamp.f * USEC_PER_SEC;

	t_epoch = stamp;
	
	t_str_tm = gmtime(&(t_timeval.tv_sec));
}

Time::Time(const struct timeval stamp) {
	t_timeval = stamp;
	
	t_epoch.i = t_timeval.tv_sec;
	t_epoch.f = (long double) t_timeval.tv_usec / USEC_PER_SEC;
	
	t_str_tm = gmtime(&(t_timeval.tv_sec));
}

Time::~Time() {
}

/* 
 * Time functions
 */

void Time::update() {
	gettimeofday(&t_timeval, NULL);
	
	t_str_tm = gmtime(&(t_timeval.tv_sec));
	
	t_epoch.i = t_timeval.tv_sec;
	t_epoch.f = t_timeval.tv_usec * 1.0 / USEC_PER_SEC;
	
	DEBUGPRINT("%ju, %Lg\n", t_epoch.i, t_epoch.f);
}

Time Time::add(const Time &extra, int fac) {
	DEBUGPRINT("%ju, %Lg\n", t_epoch.i, t_epoch.f);
	epoch_t tmp = extra.as_epoch_t();

	// Calculate new time
	t_epoch.i += tmp.i * fac;
	t_epoch.f += tmp.f * fac;
	
	// Check overflow of fractional seconds & correct
	if (t_epoch.f >= 1) {
		t_epoch.i += 1;
		t_epoch.f -= 1;
	} else if (t_epoch.f < 0) {
		t_epoch.i -= 1;
		t_epoch.f += 1;
	}
	
	sync();
	
	DEBUGPRINT("%ju, %Lg\n", t_epoch.i, t_epoch.f);
	return *this;
}

void Time::sync() {
	DEBUGPRINT("%ju, %Lg\n", t_epoch.i, t_epoch.f);
	
	t_timeval.tv_sec = t_epoch.i;
	t_timeval.tv_usec = (suseconds_t) (t_epoch.f * USEC_PER_SEC);
	
	t_str_tm = gmtime(&(t_timeval.tv_sec));
}

/* 
 * Operator overloading
 */

Time Time::operator+(const Time& rhs) const {
	Time out(*this);
	return out.add(rhs);
}

Time Time::operator+=(const Time& rhs) {
	return this->add(rhs);
}

Time Time::operator-(const Time& rhs) const {
	Time out(*this);
	return out.add(rhs, -1);
}

Time Time::operator-=(const Time& rhs) {
	return this->add(rhs, -1);
}


/* 
 * Report functions
 */

time_t Time::as_time_t() const { return t_timeval.tv_sec; }
Time::epoch_t Time::as_epoch_t() const { return t_epoch; }
struct tm* Time::as_str_tm() const { return t_str_tm; }
struct timeval Time::as_str_tv() const { return t_timeval; }
string Time::str() const { 
	DEBUGPRINT("%ju, %Lg\n", t_epoch.i, t_epoch.f);
	return format("%d.%09d", t_epoch.i, (int) (t_epoch.f * 1E9)); 
}
const char *Time::c_str() const { 
	DEBUGPRINT("%ju, %Lg\n", t_epoch.i, t_epoch.f);
	return format("%d.%09d", t_epoch.i, (int) (t_epoch.f * 1E9)).c_str();
}

string Time::strftime(string fmt) const {
	char buf[128];
	::strftime(buf, sizeof(buf), fmt.c_str(), t_str_tm);
	return string(buf);
}
