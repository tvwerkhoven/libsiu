#ifndef HAVE_IMGDATA_H
#define HAVE_IMGDATA_H

#include <string>
#include <stdio.h>
#include <inttypes.h>
#include <gsl/gsl_matrix.h>

#ifdef HAVE_CONFIG_H
#include <autoconfig.h>
#endif

#include "types.h"
#include "path++.h"
#include "io.h"

const uint8_t IMGDATA_MAXNDIM = 32;

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
		ERR_CREATE_IMG,
		ERR_WRITE_FILE,
		ERR_TYPE_UNKNOWN,
		ERR_TYPE_UNSUPP,
		ERR_FILETYPE,
	} error_t;
	
	// Data layout
	typedef struct data_t {
		void *data;											//!< Data blob
		int ndims;											//!< Number of dimenions (<= IMGDATA_MAXNDIM)
		size_t dims[IMGDATA_MAXNDIM];	//!< Actual dimenions
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
		stats_t() : init(false) { }
	} stats_t;
	
	// File info
	typedef struct file_t {
		Path path;
		imgtype_t itype;
		file_t(Path _p=string(""), imgtype_t _i=AUTO) : path(_p), itype(_i) { }
	} file_t;
	
private:
	Io &io;
	std::string strerr;
	
	int loadFITS(const Path&);
	int loadICS(const Path&);
	int loadGSL(const Path&);
	int loadPGM(const Path&);
	
	int writeFITS(const Path&);
	int writeICS(const Path&);
	int writeGSL(const Path&);
	int writePGM(const Path&);
	
	int readNumber(FILE *fd);			//!< Helper function for readPGM()
	
	imgtype_t guesstype(const Path&);
	
	string dtype_str(dtype_t dt);
	
	template <typename T>
	void _swapaxes(const int *order, T data);
	
public:	
	data_t data;
	file_t finfo;
	stats_t stats;
	error_t err;
	
	// New bare ImgData instance
	ImgData(Io &io): io(io) { ; }
	// New from file & filetype
	ImgData(Io &io, const std::string f, imgtype_t t = AUTO);
	ImgData(Io &io, const Path f, imgtype_t t = AUTO);
	~ImgData(void);
	
	// Generic data IO routines
	int loaddata(const Path&, imgtype_t);
	int writedata(const Path &p, const imgtype_t t) { return writedata(p.str(), t); }
	int writedata(const std::string, const imgtype_t);
	
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
	
	void calcstats();
	// Print file metadata
	void printmeta();
	
	// Public handlers
	gsl_matrix *as_GSL(bool);
	data_t as_datat() { data.refs++; return data; }
	error_t geterr() { return err; }
	dtype_t getdtype() { return data.dt; }
	int getbitpix() { return data.bpp; }
	int getbpp() { return data.bpp; }
	void *getdata() { return data.data; }
	imgtype_t getimgtype() { return finfo.itype; }
	int getndims() { return data.ndims; }
	size_t getdim(int d) { if (d < data.ndims) return data.dims[d]; return 0; }
	size_t getwidth() { return getdim(0); }
	size_t getheight() { return getdim(1); }
	size_t getsize() { return data.size; }
	size_t getnel() { return data.nel; }

};

#endif // HAVE_IMGDATA_H
