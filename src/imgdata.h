#ifndef HAVE_IMGDATA_H
#define HAVE_IMGDATA_H

#include <string>
#include <stdio.h>
#include <inttypes.h>

#ifdef HAVE_CONFIG_H
#include <autoconfig.h>
#endif

#include "io.h"

const uint8_t IMGDATA_MAXDIM = 32;

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
	
	// Data formats
	typedef enum {
		UINT8=0,
		INT8,
		UINT16,
		INT16,
		UINT32,
		INT32,
		UINT64,
		INT64,
		FLOAT32,
		FLOAT64,
		DATA_UNDEF
	} dtype_t;
	
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
	typedef struct {
		void *data;
		int ndims;
		uint64_t dims[IMGDATA_MAXDIM];
		dtype_t dt;
		int bpp;
		uint64_t size;
		uint64_t nel;
	} data_t;

	// Data statistics
	typedef struct {
		double min;
		double max;
		double sum;
		uint64_t minidx;
		uint64_t maxidx;
	} stats_t;
	
	// File info
	typedef struct {
		std::string path;
		imgtype_t itype;
	} file_t;
	
private:
	Io &io;
	std::string strerr;
	
	int loadFITS(const std::string);
	int loadICS(const std::string);
	int loadGSL(const std::string);
	int loadPGM(const std::string);
	
	int writeFITS(const std::string);
	int writeICS(const std::string);
	int writeGSL(const std::string);
	int writePGM(const std::string);
	
	imgtype_t guessType(const std::string);
	
	string dtype_str(dtype_t dt);
	
	template <typename T>
	void _swapAxes(const int *order, T data);
	
public:	
	data_t data;
	file_t finfo;
	stats_t stats;
	error_t err;
	
	// New bare ImgData instance
	ImgData(Io &io): io(io) { ; }
	// New from file & filetype
	ImgData(Io &io, const std::string f, imgtype_t t);
	~ImgData(void);
	
	// Generic data IO routines
	int loadData(const std::string, imgtype_t);
	int writeData(const std::string, const imgtype_t);
	
	// Create from data
	int setData(void *data, int nd, uint64_t dims[], dtype_t dt, int bpp);
	
	// Return a single pixel at (1-d) index idx
	double getPixel(const int idx);
	//template <typename T> T getPixel(const int idx);
	
	// Swap axis & transpose data
	int swapAxes(const int *order);
	
	void calcStats();
	// Print file metadata
	void printMeta();
	
	// Public handlers
	error_t getErr() { return err; }
	dtype_t getDtype() { return data.dt; }
	int getBitpix() { return data.bpp; }
	int getBPP() { return data.bpp; }
	void *getData() { return data.data; }
	imgtype_t getImgtype() { return finfo.itype; }
	int getNDims() { return data.ndims; }
	int getDim(int d) { return data.dims[d]; }
	int getSize() { return data.size; }
	int getNEl() { return data.nel; }

};

#endif // HAVE_IMGDATA_H
