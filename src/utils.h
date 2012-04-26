/*
 utils.h -- Tiny miscellaneous functions
 Copyright (C) 2011 Tim van Werkhoven <T.I.M.vanWerkhoven@xs4all.nl>
 
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

#ifndef HAVE_UTILS_H
#define HAVE_UTILS_H

template <class T> T clamp(const T x, const T min, const T max) { return x < min ? min : x > max ? max : x; }

#include <stdlib.h> // for rand_r()

static inline double simple_rand() { 
	static unsigned int seed = 1;
	return rand_r(&seed)*1.0/RAND_MAX;
}

#include "autoconfig.h"

#define DEBUGPRINT(fmt, ...) \
 do { if (LIBSIU_VERBOSE) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
 __LINE__, __func__, __VA_ARGS__); } while (0)


#endif // HAVE_UTILS_H
