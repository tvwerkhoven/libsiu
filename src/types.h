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
 @brief float vector with absolute begin and end
 */
typedef struct fvec {
	fvec(float _lx=0.0, float _ly=0.0, float _tx=1.0, float _ty=1.0) :
		lx(_lx), ly(_ly), tx(_tx), ty(_ty) { }
	float lx;
	float ly;
	float tx;
	float ty;
} fvector_t;

/*!
 @brief int vector with absolute begin and end
 */
typedef struct vec {
	vec(int _lx=0, int _ly=0, int _tx=1, int _ty=1) :
	lx(_lx), ly(_ly), tx(_tx), ty(_ty) { }
	int lx;
	int ly;
	int tx;
	int ty;
} vector_t;

/*!
 @brief difference float vector with absolute begin and length
 */
typedef struct fdvec {
	fdvec(float _lx=0.0, float _ly=0.0, float _sx=1.0, float _sy=1.0) :
	lx(_lx), ly(_ly), sx(_sx), sy(_sy) { }
	float lx;
	float ly;
	float sx;
	float sy;
} fdvector_t;

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
	UINT8=0,			//!< ID for uint8_t
	INT8,					//!< ID for int8_t
	UINT16,				//!< ID for uint16_t
	INT16,				//!< ID for int16_t
	UINT32,				//!< ID for uint32_t
	INT32,				//!< ID for int32_t
	UINT64,				//!< ID for uint64_t
	INT64,				//!< ID for int64_t
	FLOAT32,			//!< ID for float
	FLOAT64,			//!< ID for double
	DATA_UNDEF		//!< ID for undefined
} dtype_t;


template <class T> T clamp(const T x, const T min, const T max) { return x < min ? min : x > max ? max : x; }

#endif // HAVE_TYPES_H
