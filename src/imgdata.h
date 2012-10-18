/*
 imgdata.h -- abstraction class for reading/writing data 
 Copyright (C) 2010--2011 Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>
 
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

#ifndef HAVE_IMGDATA_H
#define HAVE_IMGDATA_H

#ifdef HAVE_CONFIG_H
// Contains various library we have.
#include "autoconfig.h"
#endif

#include <string>
#include <stdio.h>
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <stdint.h>
#if HAVE_GSL
#include <gsl/gsl_matrix.h>
#endif


#include "types.h"
#include "path++.h"
#include "io.h"

const uint8_t IMGDATA_MAXNDIM = 32;

using namespace std;

class ImgData {
public:
	// Image formats
	typedef enum {
		FITS=0,
		ICS,
		PGM,
		GSL,
		AUTO,
		IMG_UNDEF
	} imgtype_t;
	
	typedef enum {
		ERR_NO_ERROR=0,
		ERR_OPEN_FILE,
		ERR_LOAD_FILE,
		ERR_CREATE_FILE,
		ERR_FILE_EXISTS,
		ERR_CREATE_IMG,
		ERR_WRITE_FILE,
		ERR_TYPE_UNKNOWN,
		ERR_TYPE_UNSUPP,
		ERR_FILETYPE,
		ERR_SETDATA,
		ERR_UNKNOWN,
	} error_t;
	
	// Data layout
	typedef struct data_t {
		void *data;											//!< Data blob
		int ndims;											//!< Number of dimenions (<= IMGDATA_MAXNDIM)
		size_t dims[IMGDATA_MAXNDIM];		//!< Actual dimenions
		size_t strides[IMGDATA_MAXNDIM]; //!< Internal memory strides
		dtype_t dt;											//!< Datatype
		int bpp;												//!< Bits per pixel
		size_t size;										//!< Size in bytes
		size_t nel;											//!< Number of elements (== size*8/bpp)
		int refs;												//!< Reference counter, i.e. number of processes using this data
		data_t() : data(NULL), ndims(0), dt(DATA_UNDEF), bpp(-1), size(0), nel(0), refs(0) { }
	} data_t;

	// Data statistics
	//! @todo how to set stats to 'undefined' with only bools? -> init good solution?
	typedef struct stats_t {
		double min;
		double max;
		double sum;
		size_t minidx;
		size_t maxidx;
		bool init;
		stats_t() : min(0), max(0), sum(0), minidx(0), maxidx(0), init(false) { }
	} stats_t;
	
	// File info
	typedef struct file_t {
		Path path;
		imgtype_t itype;
		file_t(Path _p=string(""), imgtype_t _i=AUTO) : path(_p), itype(_i) { }
	} file_t;
	
private:
	Io &io;
	error_t err;
	data_t data;
	
	int loadFITS(const Path&);					//!< Load FITS Files (cfitsio)
	int loadICS(const Path&);						//!< Load ICS Files (libics)
	int loadGSL(const Path&);						//!< Load GSL matrices (lgsl)
	int loadPGM(const Path&);						//!< Load PGM files
	
	int writeFITS(const Path&);					//!< Write FITS
	int writeICS(const Path&);					//!< Write ICS
	int writeGSL(const Path&);					//!< Write GSL (warning: does not store metadata!)
	int writePGM(const Path&);					//!< Write PGM

#if HAVE_GSL
	int setGSLdata(const gsl_matrix *mat, const bool copy=false); //!< Set data from GSL matrix (double)
	int setGSLdata(const gsl_matrix_float *mat, const bool copy=false); //!< Set data from GSL matrix (float)
#endif
	
	template <class T>
	int _setGSLdata(const T *mat, const bool copy);
	
	int readNumber(FILE *fd);						//!< Helper function for readPGM()
	
	imgtype_t guesstype(const Path&);		//!< Guess filetype based on extension
	
	string dtype_str(dtype_t dt);				//!< Return datatype as string
	
	template <typename T>
	void _swapaxes(const int *order, T data);	//!< Swap axes of data (transpose etc.)

	file_t finfo;
	stats_t stats;

	bool havegsl;
	bool havefits;
	bool havepgm;
	bool haveics;

public:	
		
	// New bare ImgData instance
	ImgData(Io &io);
	// New from file & filetype
	ImgData(Io &io, const std::string f, imgtype_t t = AUTO);
	ImgData(Io &io, const Path f, imgtype_t t = AUTO);
	// New from data
#if HAVE_GSL
	ImgData(Io &io, const gsl_matrix *m, const bool copy=false);
	ImgData(Io &io, const gsl_matrix_float *m, const bool copy=false);
#endif
	
	~ImgData(void);
	
	// Generic data IO routines
	int loaddata(const Path&, imgtype_t);
	int writedata(const Path &p, const imgtype_t t, const bool overwrite=false);
	int writedata(const std::string pstr, const imgtype_t t, const bool overwrite=false) { Path p(pstr); return writedata(p, t, overwrite); }
	
	// Create from data
	int setdata(void *data, int nd, size_t dims[], dtype_t dt, int bpp);
	
	// Return a single pixel at (1-d) index idx
	double getpixel(const int idx);
	//template <typename T> T getPixel(const int idx);
	// Return a single pixel in 2d, 3d or 4d data.
	double getpixel(const int idx0, const int idx1);
	double getpixel(const int idx0, const int idx1, const int idx2);
	double getpixel(const int idx0, const int idx1, const int idx2, const int idx3);
	
	// Swap axis & transpose data
	int swapaxes(const int *order);
	
	// Calculate & print stats
	void calcstats();
	void printmeta();
	
	// Public handlers
#if HAVE_GSL
	gsl_matrix *as_GSL(bool copy=true);
#endif
	data_t as_datat() { data.refs++; return data; }
	
	// Get properties
	bool have_gsl() const { return havegsl; }
	bool have_fits() const { return havefits; }
	bool have_pgm() const { return havepgm; }
	bool have_ics() const { return haveics; }
	
	error_t geterr() const { return err; }
	
	// Get data stuff
	dtype_t getdtype() const { return data.dt; }
	int getbitpix() const { return data.bpp; }
	int getbpp() const { return data.bpp; }
	void *getdata() const { return data.data; }
	imgtype_t getimgtype() const { return finfo.itype; }
	int getndims() const { return data.ndims; }
	size_t getdim(const int d) const { if (d < data.ndims) return data.dims[d]; return 0; }
	size_t getwidth() const { return getdim(0); }
	size_t getheight() const { return getdim(1); }
	size_t getsize() const { return data.size; }
	size_t getnel() const { return data.nel; }
	
	// Get statistics data
	bool have_stats() const { return stats.init; }
	double get_minval() const { return stats.min; }
	size_t get_minidx() const { return stats.minidx; }
	double get_maxval() const { return stats.max; }
	size_t get_maxidx() const { return stats.maxidx; }
	double get_sum() const { return stats.sum; }
	

};

#endif // HAVE_IMGDATA_H
