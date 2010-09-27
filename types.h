/*
 types.h -- Implements some missing miscellaneous types and functions
 Copyright (C) 2010 Tim van Werkhoven <T.I.M.vanWerkhoven@xs4all.nl>
 
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

#ifndef HAVE_TYPES_H
#define HAVE_TYPES_H

// STRUCTS AND TYPES //
/*********************/


/*!
 @brief We use this to define integer 2-vectors (resolutions etc)
 */
typedef struct coord_t {
	coord_t(int _x=0, int _y=0) : x(_x), y(_y) { }
	int x;                  //!< x coordinate
	int y;                  //!< y coordinate
} coord_t;

/*!
 @brief We use this to define floating point 2-vectors (displacements etc)
 */
typedef struct fcoord_t {
	fcoord_t(float _x=0, float _y=0) : x(_x), y(_y) { }
	float x;                //!< x coordinate
	float y;                //!< y coordinate
} fcoord_t;

/*!
 @brief We use this to store gain information for WFC's
 */
typedef struct gain_t {
	gain_t(float _p=1, float _i=0, float _d=0) : p(_p), i(_i), d(_d) { }
	float p;                //!< proportional gain
	float i;                //!< integral gain
	float d;                //!< differential gain
} gain_t;

/*! 
 @brief This enum is used to distinguish between various datatypes for processing.
 
 */
typedef enum {
	DATA_INT8,			//!< ID for int8_t
	DATA_UINT8,			//!< ID for uint8_t
	DATA_INT16,			//!< ID for int16_t
	DATA_UINT16			//!< ID for uint16_t
} dtype_t;


template <class T> T clamp(T x, T min, T max) { if (x < min) x = min; if (x > max) x = max; return x; }

#endif // HAVE_TYPES_H
