/*
 time++.h -- easy class-based time handling for C++
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

#ifndef HAVE_TIMEPP_H
#define HAVE_TIMEPP_H

#include <sys/time.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <stdint.h>
#include <inttypes.h>
#include <string>

using namespace std;
const double USEC_PER_SEC = 1000000.0; //!< Conversion factor

/*!
 @brief Implements easier time-handling. The goal is convenience, not absolute accuracy.
 */
class Time {
public:
	typedef struct time {
		time(intmax_t _i=0, long double _f=0): i(_i), f(_f) { ; }
		intmax_t i;												//!< Seconds since epoch in UTC
		long double f;										//!< Fractional seconds
	} epoch_t;
	
	
private:
	
	// Raw time containers
	struct timeval t_timeval;
	struct tm *t_str_tm;
	
	epoch_t t_epoch;										//!< Current time (our main clock)
	
	Time add(const Time &extra, int fac=1); //!< Add 'extra' to current time
	void sync();												//!< Sync other timestamps from t_epoch (our main clock)
	
public:
	Time();															//!< New current Time()
	Time(const intmax_t i, const long double f); //!< New Time() based on seconds and fractional seconds
	Time(const Time &stamp);						//!< New Time() based on Time
	Time(const time_t stamp);						//!< New Time() based on time_t
	Time(const epoch_t stamp);					//!< New Time() based on epoch_t
	Time(const struct timeval stamp);		//!< New Time() based on struct timeval
	~Time();
	
	inline bool operator!=(const Time &b) const { return (b.as_epoch_t().i != this->as_epoch_t().i || b.as_epoch_t().f != this->as_epoch_t().f); }
	inline bool operator==(const Time &b) const { return (b.as_epoch_t().i == this->as_epoch_t().i && b.as_epoch_t().f == this->as_epoch_t().f); }
	Time operator+(const Time& rhs) const;
	Time operator+=(const Time& rhs);
	Time operator-(const Time& rhs) const;
	Time operator-=(const Time& rhs);

	Time operator-() { t_epoch.i *= -1; t_epoch.f *= -1; sync(); return *this; }
	Time operator+() { return *this; }

  double operator/(const double& rhs) { return (t_epoch.i/rhs) + (t_epoch.f/rhs); }

	Time operator--() { t_epoch.i -= 1; sync(); return *this; }
	Time operator++() { t_epoch.i += 1; sync(); return *this; }

	void update();											//!< Update current stored time
	
	time_t as_time_t() const;						//!< Return time as time_t
	epoch_t as_epoch_t() const;					//!< Return time as epoch_t
	struct tm* as_str_tm() const;				//!< Return time as struct tm
	struct timeval as_str_tv() const;		//!< Return time as struct timeval
	string str() const;									//!< Return seconds since epoch as string (with 9 decimals)
	const char* c_str() const;					//!< Return seconds since epoch as string (with 9 decimals)
	
	string strftime(string fmt) const;	//!< Time++ wrapper for strftime()
	
};

#endif // HAVE_PATHPP_H
