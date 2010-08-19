#include <string>
#include <stdio.h>
#include <inttypes.h>

#include <libics.h>
#include <fitsio.h>

#include "io.h"
#include "imgdata.h"

ImgData::~ImgData() {
	if (data.data) 
		free(data.data);
}

ImgData::ImgData(Io &io, const std::string f, imgtype_t t = ImgData::AUTO): io(io) {
	io.msg(IO_DEB2, "ImgData::ImgData() new from file.");
	
	err = ERR_NO_ERROR;
	
	// TODO set image params to zero/undef

	if (t == ImgData::AUTO)
		t = guessType(f);
	
	if (loadData(f, t))
		err = ERR_LOAD_FILE;
}

string ImgData::dtype_str(dtype_t dt) {
	switch (dt) {
		case UINT8: return "UINT8";
		case INT8: return "INT8";
		case UINT16: return "UINT16";
		case INT16: return "INT16";
		case UINT32: return "UINT32";
		case INT32: return "INT32";
		case UINT64: return "UINT64";
		case INT64: return "INT64";
		case FLOAT32: return "FLOAT32";
		case FLOAT64: return "FLOAT64";
		case DATA_UNDEF: return "DATA_UNDEF";
		default: return "NULL";
	}
}

ImgData::imgtype_t ImgData::guessType(const std::string file) {
	int idx = file.rfind(".");
	string substr = file.substr(idx+1);
	
	// TODO convert all to lowercase	
	if (substr == "fits") return ImgData::FITS;
	else if (substr == "gsl") return ImgData::GSL;
	else if (substr == "ics" || substr == "ids") return ImgData::ICS;
	else if (substr == "pgm") return ImgData::PGM;
	
	err = ERR_FILETYPE;
	io.msg(IO_ERR | IO_FATAL, "Could not detect filetype from filename, aborting.");
	
	return ImgData::IMG_UNDEF;
}

void ImgData::calcStats() {
	double sum=0, min=getPixel(0), max=getPixel(0), pix=0;
	uint64_t minidx=0, maxidx=0;
	uint64_t p;
	
	for (p=0; p<data.nel; p++) {
		pix = getPixel(p);
		sum += pix; 
		if (pix < min) {min = pix; minidx = p;}
		else if (pix > max) {max = pix; maxidx = p;}
	}
	
	stats.min = min;
	stats.max = max;
	stats.sum = sum;
	stats.maxidx = maxidx;
	stats.minidx = minidx;
}

void ImgData::printMeta() {
	io.msg(IO_INFO, "ImgData:: data: %p, ndims: %d, nel: %lld, bpp: %d, size: %lld", 
		   data.data, data.ndims, data.nel, data.bpp, data.size);
	
	// Dimensions
	io.msg(IO_INFO | IO_NOLF, "ImgData:: dim[0] = %d", data.dims[0]);
	for (int d=1; d<data.ndims; d++)
		io.msg(IO_INFO | IO_NOID, ", dim[%d] = %d", d, data.dims[d]);
	io.msg(IO_INFO | IO_NOID, "\n");
	
	calcStats();
	
	io.msg(IO_INFO, "ImgData:: range: %g (@%lld) -- %g (@%lld), avg: %g, sum: %g", 
		   stats.min, stats.minidx, stats.max, stats.maxidx, stats.sum/data.nel, stats.sum);
}

double ImgData::getPixel(const int idx) {
	if (data.dt == ImgData::UINT8) return (double) ((uint8_t*) data.data)[idx];
	else if (data.dt == ImgData::INT8) return (double) ((int8_t*) data.data)[idx];
	else if (data.dt == ImgData::UINT16) return (double) ((uint16_t*) data.data)[idx];
	else if (data.dt == ImgData::INT16) return (double) ((int16_t*) data.data)[idx];
	else if (data.dt == ImgData::UINT32) return (double) ((uint32_t*) data.data)[idx];
	else if (data.dt == ImgData::INT32) return (double) ((int32_t*) data.data)[idx];
	else if (data.dt == ImgData::UINT64) return (double) ((uint64_t*) data.data)[idx];
	else if (data.dt == ImgData::INT64) return (double) ((int64_t*) data.data)[idx];
	else if (data.dt == ImgData::FLOAT32) return (double) ((float*) data.data)[idx];
	else if (data.dt == ImgData::FLOAT64) return (double) ((double*) data.data)[idx];
	else return (double) io.msg(IO_ERR, "getPixel(): fail!");
}

//template <typename T>
//T ImgData::getPixel(const int idx) {
//	return ((T *) data.data)[idx];
//}

int ImgData::swapAxes(const int *order) {
	io.msg(IO_INFO | IO_NOLF, "New dimension order: %d", order[0]);
	for (int d=1; d<data.ndims; d++)
		io.msg(IO_INFO | IO_NOID, ", %d", order[d]);
	io.msg(IO_INFO | IO_NOID, "\n");
	
	if (data.dt == ImgData::UINT8) _swapAxes(order, (uint8_t*) data.data);
	else if (data.dt == ImgData::INT8) _swapAxes(order, (int8_t*) data.data);
	else if (data.dt == ImgData::UINT16) _swapAxes(order, (uint16_t*) data.data);
	else if (data.dt == ImgData::INT16) _swapAxes(order, (int16_t*) data.data);
	else if (data.dt == ImgData::UINT32) _swapAxes(order, (uint32_t*) data.data);
	else if (data.dt == ImgData::INT32) _swapAxes(order, (int32_t*) data.data);
	else if (data.dt == ImgData::UINT64) _swapAxes(order, (uint64_t*) data.data);
	else if (data.dt == ImgData::INT64) _swapAxes(order, (int64_t*) data.data);
	else if (data.dt == ImgData::FLOAT32) _swapAxes(order, (float*) data.data);
	else if (data.dt == ImgData::FLOAT64) _swapAxes(order, (double*) data.data);
	else return (double) io.msg(IO_ERR, "getPixel(): fail!");
	
	return 0;
}

template <typename T>
void ImgData::_swapAxes(const int* /* order */, T datacast) {
	io.msg(IO_XNFO, "Swapping axes now...");
	
	T tmp = (T) malloc(data.size);
	int i,j,k,l;
	uint64_t curridx, newidx;
	uint64_t *dim = data.dims;
	
	//	d, i, j, k, l
	//	0  0  0  0  0
	//	1  1  0  0  0 
	//	10 0  5  0  0
	//	256 0 128 0  0
	//	512 0 0  1
	
	if (data.ndims !=4)
		return (void) io.msg(IO_ERR, "Error, only works for ndims=4 for now");
	
	for (uint64_t d=0; d<data.nel; d++) {
		i = d % dim[0];
		j = (d/dim[0]) % dim[1];
		k = (d/(dim[0]*dim[1])) % dim[2];
		l = (d/(dim[0]*dim[1]*dim[2])) % dim[3];
		curridx = i + j*dim[0] + k*dim[1]*dim[0] + l*dim[2]*dim[1]*dim[0];
		newidx = j + k*dim[1] + l*dim[1]*dim[2] + i*dim[1]*dim[2]*dim[3];
		
		if (curridx != d) return (void) io.msg(IO_ERR, "Error, d=%lld != curridx=%lld (%d,%d,%d,%d)", d, curridx, i,j,k,l);
		
		tmp[newidx] = datacast[curridx];
	}
	
	// Update pointer to transformed data
	free(data.data);
	data.data = tmp;
}

int ImgData::setData(void *newdata, int nd, uint64_t dims[], dtype_t dt, int bpp) {
	uint64_t nel=1;
	
	data.data = newdata;
	data.ndims = nd;
	for (int d=0; d<nd; d++) {
		if (d >= IMGDATA_MAXDIM)
			return io.msg(IO_ERR, "Error in setData(): number of dimensions too big!");
		
		nel *= dims[d];
		data.dims[d] = dims[d];
	}
	data.dt = dt;
	data.bpp = bpp;
	
	data.nel = nel;
	data.size = nel * bpp;
	
	return 0;
}

int ImgData::loadData(const std::string f, const imgtype_t t) {
	switch (t) {
#ifdef HAVE_FITS
		case ImgData::FITS:
			return loadFITS(f);
			break;
#endif
#ifdef HAVE_ICS
		case ImgData::ICS:
			return loadICS(f);
			break;
#endif
		case ImgData::PGM:
			return loadPGM(f);
			break;
#ifdef HAVE_GSL
		case ImgData::GSL:
			return loadGSL(f);
			break;
#endif
		default:
			err = ERR_TYPE_UNKNOWN;
			return io.msg(IO_ERR, "Unknown datatype, cannot load file.");
			break;
	}	
}

int ImgData::writeData(const std::string f, const imgtype_t t) {
	switch (t) {
#ifdef HAVE_FITS
		case ImgData::FITS:
			return writeFITS(f);
			break;
#endif
#ifdef HAVE_ICS
		case ImgData::ICS:
			return writeICS(f);
			break;
#endif
		case ImgData::PGM:
			return writePGM(f);
			break;
#ifdef HAVE_GSL
		case ImgData::GSL:
			return writeGSL(f);
			break;
#endif
		default:
			err = ERR_TYPE_UNKNOWN;
			return io.msg(IO_ERR, "Unknown datatype, cannot write data.");
			break;
	}
}

#ifdef HAVE_FITS
int ImgData::loadFITS(const std::string file) {
	fitsfile *fptr;
	char fits_err[30];
	int stat = 0;
	// TODO how big should naxes be? What's the maximum according to the FITS standard?
	long naxes[8];
	int anynul = 0;
	
	fits_open_file(&fptr, file.c_str(), READONLY, &stat);
	if (stat) {
		fits_get_errstatus(stat, fits_err);
		err = ERR_OPEN_FILE;
		return io.msg(IO_ERR, "FITS error: %s", fits_err);
	}
	
	fits_get_img_param(fptr, 8, &(data.bpp), &(data.ndims), naxes, &stat);
	if (stat) {
		fits_get_errstatus(stat, fits_err);
		err = ERR_OPEN_FILE;
		return io.msg(IO_ERR, "FITS error: %s", fits_err);
	}
	
	data.nel = 1;
	for (int d=0; d<data.ndims; d++) {
		data.dims[d] = naxes[d];
		data.nel *= naxes[d];
	}
	data.size = data.nel * data.bpp/8;
	data.data = (void *) malloc(data.size);
	
	// BYTE_IMG (8), SHORT_IMG (16), LONG_IMG (32), LONGLONG_IMG (64), FLOAT_IMG (-32), and DOUBLE_IMG (-64)
	// TBYTE, TSBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TFLOAT, TDOUBLE
	
	switch (data.bpp) {
		case BYTE_IMG: {
			uint8_t nulval = 0;
			fits_read_img(fptr, TBYTE, 1, data.nel, &nulval, \
						  (uint8_t *) (data.data), &anynul, &stat);
			data.dt = UINT8;
			break;
		}
		case SHORT_IMG: {
			uint16_t nulval = 0;
			fits_read_img(fptr, TUSHORT, 1, data.nel, &nulval, \
						  (uint16_t *) (data.data), &anynul, &stat);					
			data.dt = UINT16;
			break;
		}
		case LONG_IMG: {
			uint32_t nulval = 0;
			fits_read_img(fptr, TUINT, 1, data.nel, &nulval, \
						  (uint32_t *) (data.data), &anynul, &stat);					
			data.dt = UINT32;
			break;
		}
		case FLOAT_IMG: {
			float nulval = 0;
			fits_read_img(fptr, TFLOAT, 1, data.nel, &nulval, \
						  (float *) (data.data), &anynul, &stat);					
			data.dt = FLOAT32;
			break;
		}
		case DOUBLE_IMG: {
			double nulval = 0;
			fits_read_img(fptr, TDOUBLE, 1, data.nel, &nulval, \
						  (double *) (data.data), &anynul, &stat);					
			data.dt = FLOAT64;
			break;
		}
		default: {
			err = ERR_TYPE_UNKNOWN;
			return io.msg(IO_ERR, "Unknown FITS datatype");
		}
	}
	
	fits_close_file(fptr, &stat);
	
	if (stat) {
		fits_get_errstatus(stat, fits_err);
		err = ERR_LOAD_FILE;
		return io.msg(IO_ERR, "FITS error: %s", fits_err);
	}	
	
	return 0;
}
#else
int ImgData::loadFITS(const std::string file) {
	return io.msg(IO_ERR, "loadFITS not supported, librabry was not available during compilation.");
}
#endif

#ifdef HAVE_ICS
int ImgData::loadICS(const std::string file) {
	io.msg(IO_XNFO, "Loading data from '%s' as ICS.", file.c_str());
	// Init ICS variables
	::ICS *ip;
	const char *errtxt;
	Ics_DataType dt;
	Ics_Error retval;
	
	// Open ICS file
	retval = IcsOpen(&ip, file.c_str(), "r");
	if (retval != IcsErr_Ok) {
		errtxt = IcsGetErrorText(retval); 
		err = ERR_OPEN_FILE;
		io.msg(IO_ERR | IO_FATAL, "Could not open file '%s': %s.", file.c_str(), errtxt);
	}
	
	// Get data layout
	int ndims;
	size_t dims[ICS_MAXDIM];
	IcsGetLayout (ip, &dt, &ndims, dims);
	data.ndims = ndims;
	data.nel = 1;
	for (int d=0; d<ndims; d++) {
		data.dims[d] = dims[d];
		data.nel *= dims[d];
	}
	data.size = IcsGetDataSize (ip);
	data.bpp = 8*data.size/data.nel;
	
	// Read data
	data.data = (void *) malloc(data.size);
	retval = IcsGetData (ip, data.data, data.size);
	if (retval != IcsErr_Ok) {
		errtxt = IcsGetErrorText(retval); 
		err = ERR_LOAD_FILE;
		io.msg(IO_ERR | IO_FATAL, "Could not read data from '%s': %s.", file.c_str(), errtxt);
	}
	
	retval = IcsClose (ip);
	
	switch (dt) {
		case Ics_uint8: {
			data.dt = ImgData::UINT8;
			break;
		}
		case Ics_sint8: {
			data.dt = ImgData::INT8;
			break;
		}
		case Ics_uint16: {
			data.dt = ImgData::UINT16;
			break;
		}
		case Ics_sint16: {
			data.dt = ImgData::INT16;
			break;
		}
		case Ics_uint32: {
			data.dt = ImgData::UINT32;
			break;
		}
		case Ics_sint32: {
			data.dt = ImgData::INT32;
			break;
		}
		case Ics_real32: {
			data.dt = ImgData::FLOAT32;
			break;
		}
		case Ics_real64: {
			data.dt = ImgData::FLOAT64;
			break;
		}
		case Ics_complex32:
		case Ics_complex64: {
			err = ERR_TYPE_UNSUPP;
			return io.msg(IO_ERR, "Unsupported datatype (complex), cannot process file");
			break;
		}
		default: {
			err = ERR_TYPE_UNKNOWN;
			return io.msg(IO_ERR, "Unknown datatype, cannot process file");
			break;
		}
	}
	
	return (int) !(retval == IcsErr_Ok);
}
#else
int ImgData::loadICS(const std::string file) {
	return io.msg(IO_ERR, "loadICS not supported, librabry was not available during compilation.");
}
#endif

int ImgData::loadPGM(const std::string /* file */) {
	err = ERR_TYPE_UNSUPP;
	return io.msg(IO_ERR, "Not implemented yet");
}

#ifdef HAVE_GSL
int ImgData::loadGSL(const std::string /* file */) {
	err = ERR_TYPE_UNSUPP;
	return io.msg(IO_ERR, "Not implemented yet");
}
#else
int ImgData::loadGSL(const std::string file) {
	return io.msg(IO_ERR, "loadGSL not supported, librabry was not available during compilation.");
}
#endif

#ifdef HAVE_FITS
int ImgData::writeFITS(const std::string file) {
	io.msg(IO_XNFO, "Saving data to '%s' as FITS.", file.c_str());
	
	// Init local FITS variables
	fitsfile *fptr;
	char fitserr[80];
	int status = 0;
	long fpixel = 1, naxis = data.ndims, nelements = data.nel;
	long naxes[naxis];
	for (int d=0; d<naxis; d++)
		naxes[d] = data.dims[d];
	
	if (fits_create_file(&fptr, file.c_str(), &status)) {
		fits_read_errmsg(fitserr);
		err = ERR_CREATE_FILE;
		return io.msg(IO_ERR, "Could not create file '%s' for writing: %s.", file.c_str(), fitserr);
	}
	
	// Get FITS datatype
	int bitpix, dtype;
	if (data.dt == ImgData::UINT8) { bitpix = BYTE_IMG; dtype = TBYTE; }
	else if (data.dt == ImgData::INT8) { bitpix = BYTE_IMG; dtype = TSBYTE; }
	else if (data.dt == ImgData::UINT16) { bitpix = SHORT_IMG; dtype = TUSHORT; }
	else if (data.dt == ImgData::INT16) { bitpix = SHORT_IMG; dtype = TSHORT; }
	else if (data.dt == ImgData::UINT32) { bitpix = LONG_IMG; dtype = TUINT; }
	else if (data.dt == ImgData::INT32) { bitpix = LONG_IMG; dtype = TINT; }
	else if (data.dt == ImgData::UINT64) { bitpix = LONGLONG_IMG; dtype = TULONG; }
	else if (data.dt == ImgData::INT64) { bitpix = LONGLONG_IMG; dtype = TLONG; }
	else if (data.dt == ImgData::FLOAT32) { bitpix = FLOAT_IMG; dtype = TFLOAT; }
	else if (data.dt == ImgData::FLOAT64) { bitpix = DOUBLE_IMG; dtype = TDOUBLE; }
	else { 
		err = ERR_TYPE_UNKNOWN;
		return io.msg(IO_ERR, "Unknown datatype for FITS");
	}
	
	// create & write image
	fits_create_img(fptr, SHORT_IMG, naxis, naxes, &status);
	if (status) {
		fits_read_errmsg(fitserr);
		err = ERR_CREATE_IMG;
		return io.msg(IO_ERR, "Could not create FITS image: %s", fitserr);
	}
	
	fits_write_img(fptr, dtype, fpixel, nelements, data.data, &status);
	if (status) {
		fits_read_errmsg(fitserr);
		err =  ERR_WRITE_FILE;
		return io.msg(IO_ERR, "Could not write FITS image: %s", fitserr);
	}
	
	fits_close_file(fptr, &status);
	return 0;	
}
#else
int ImgData::writeFITS(const std::string) {
	return io.msg(IO_ERR, "writeFITS not supported, librabry was not available during compilation.");
	
}
#endif

#ifdef HAVE_ICS
int ImgData::writeICS(const std::string /* file */) {
	err = ERR_TYPE_UNSUPP;
	return io.msg(IO_ERR, "Not implemented yet");
}
#else
int ImgData::writeICS(const std::string) {
	return io.msg(IO_ERR, "writeICS not supported, librabry was not available during compilation.");
	
}
#endif HAVE_ICS

int ImgData::writePGM(const std::string /* file */) {
	err = ERR_TYPE_UNSUPP;
	return io.msg(IO_ERR, "Not implemented yet");	
}

#ifdef HAVE_GSL
int ImgData::writeGSL(const std::string /* file */) {
	err = ERR_TYPE_UNSUPP;
	return io.msg(IO_ERR, "Not implemented yet");		
}
#else
int ImgData::writeGSL(const std::string) {
	return io.msg(IO_ERR, "writeGSL not supported, librabry was not available during compilation.");

}
#endif
