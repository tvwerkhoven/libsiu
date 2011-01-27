/*
 imgdata.cc -- abstraction class for reading/writing data 
 Copyright (C) 2010--2011 Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
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
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <algorithm>
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
// Contains various library we have.
#include "autoconfig.h"

#endif
#ifdef HAVE_ICS
#include <libics.h>
#endif
#ifdef HAVE_FITS
#include <fitsio.h>
#endif
#ifdef HAVE_GSL
#include <gsl/gsl_matrix.h>
#endif

#include "types.h"
#include "io.h"
#include "imgdata.h"

//! @todo handle errors better, set data to NULL on failure

// Constructors from file
ImgData::ImgData(Io &io, const std::string f, imgtype_t t): 
io(io), finfo(Path(f), t), err(ERR_NO_ERROR),
havegsl(HAVE_GSL), havefits(HAVE_FITS), havepgm(true), haveics(HAVE_ICS)
{
	io.msg(IO_DEB2, "ImgData::ImgData() new from file.");
		
	if (loaddata(finfo.path, finfo.itype))
		err = ERR_LOAD_FILE;
}

ImgData::ImgData(Io &io, const Path f, imgtype_t t): 
io(io), finfo(Path(f), t), err(ERR_NO_ERROR),
havegsl(HAVE_GSL), havefits(HAVE_FITS), havepgm(true), haveics(HAVE_ICS)
{
	io.msg(IO_DEB2, "ImgData::ImgData() new from file.");
	
	if (loaddata(finfo.path, finfo.itype))
		err = ERR_LOAD_FILE;
}

// Constructors from GSL data
#ifdef HAVE_GSL
ImgData::ImgData(Io &io, const gsl_matrix *m, const bool copy):
io(io), err(ERR_NO_ERROR),
havegsl(HAVE_GSL), havefits(HAVE_FITS), havepgm(true), haveics(HAVE_ICS)
{
	io.msg(IO_DEB2, "ImgData::ImgData(gsl_matrix, cp=%d)", copy);
	
	if (setGSLdata(m, copy)) {
		err = ERR_SETDATA;
		io.msg(IO_ERR, "ImgData::ImgData(): error at init");
	}
}

ImgData::ImgData(Io &io, const gsl_matrix_float *m, const bool copy):
io(io), err(ERR_NO_ERROR),
havegsl(HAVE_GSL), havefits(HAVE_FITS), havepgm(true), haveics(HAVE_ICS)
{
	io.msg(IO_DEB2, "ImgData::ImgData(gsl_matrix_float, cp=%d)", copy);
	
	if (setGSLdata(m, copy)) {
		err = ERR_SETDATA;
		io.msg(IO_ERR, "ImgData::ImgData(): error at init");
	}
}
#endif


ImgData::~ImgData() {
	// If there is only one process using this data (i.e. this object), delete 
	// it upon destruction of this object
	if (data.refs <= 1 && data.data)
		free(data.data);
}

#ifdef HAVE_GSL
int ImgData::setGSLdata(const gsl_matrix *mat, const bool copy) {
	io.msg(IO_DEB2, "ImgData::setGSLdata(gsl_matrix, cp=%d).", copy);
	data.dt = FLOAT64;
	data.bpp = sizeof(double);

	return _setGSLdata(mat, copy);
}

int ImgData::setGSLdata(const gsl_matrix_float *mat, const bool copy) {
	io.msg(IO_DEB2, "ImgData::setGSLdata(gsl_matrix_float, cp=%d).", copy);

	data.dt = FLOAT32;
	data.bpp = sizeof(float);

	return _setGSLdata(mat, copy);
}

template <class T>
int ImgData::_setGSLdata(const T *mat, const bool copy) {
	data.ndims = 2;
	data.dims[0] = mat->size2;
	data.dims[1] = mat->size1;
	data.nel = mat->size1 * mat->size2;
	data.size = data.nel * data.bpp;

	data.strides[0] = 1;
	
	if (copy) {
		data.strides[1] = mat->size2;
		if (data.dt == FLOAT64) {
			double *datatmp = (double *) malloc(mat->size1 * mat->size1 * data.bpp);
			for (size_t i=0; i<mat->size1; i++) // Height (row)
				for (size_t j=0; j<mat->size2; j++) // Width (column, changes fastest)
					datatmp[i * data.dims[0] + j] = mat->data[i * mat->tda + j];
			
			data.data = (void *) datatmp;
		} else if (data.dt == FLOAT32) {
			float *datatmp = (float *) malloc(mat->size1 * mat->size1 * data.bpp);
			for (size_t i=0; i<mat->size1; i++) // Height (row)
				for (size_t j=0; j<mat->size2; j++) // Width (column, changes fastest)
					datatmp[i * data.dims[0] + j] = mat->data[i * mat->tda + j];

			data.data = (void *) datatmp;
		}
		// 1 ref to this data, only us
		data.refs = 1;
	} else {
		data.strides[1] = mat->tda;
		data.data = (void *) mat->data;
		// 2 refs to this data, this object + something else 
		data.refs = 2;
	}
	
	// New data, so stats are wrong now
	stats.init = false;
	
	return 0;
}
#endif


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
		case DATA_UNDEF:
		default:
			return "DATA_UNDEF";
	}
}

ImgData::imgtype_t ImgData::guesstype(const Path &file) {
	string ext = file.get_ext();
	transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	
	if (ext == "fits") return ImgData::FITS;
	else if (ext == "gsl") return ImgData::GSL;
	else if (ext == "ics" || ext == "ids") return ImgData::ICS;
	else if (ext == "pgm") return ImgData::PGM;
	
	err = ERR_FILETYPE;
	io.msg(IO_ERR, "Could not detect filetype from filename.");
	
	return ImgData::IMG_UNDEF;
}

void ImgData::calcstats() {
	double sum=0, min=getpixel(0), max=getpixel(0), pix=0;
	size_t minidx=0, maxidx=0;
	size_t p;
	
	for (p=0; p<data.nel; p++) {
		pix = getpixel(p);
		sum += pix; 
		if (pix < min) {min = pix; minidx = p;}
		else if (pix > max) {max = pix; maxidx = p;}
	}
	
	stats.min = min;
	stats.max = max;
	stats.sum = sum;
	stats.maxidx = maxidx;
	stats.minidx = minidx;
	stats.init = true;
}

void ImgData::printmeta() {
	io.msg(IO_INFO, "ImgData::printmeta() data: %p, ndims: %d, nel: %lld, bpp: %d, size: %lld", 
		   data.data, data.ndims, data.nel, data.bpp, data.size);
	
	// Dimensions
	io.msg(IO_INFO | IO_NOLF, "ImgData::printmeta() dim[0] = %d", data.dims[0]);
	for (int d=1; d<data.ndims; d++)
		io.msg(IO_INFO | IO_NOID, ", dim[%d] = %d", d, data.dims[d]);
	io.msg(IO_INFO | IO_NOID, "\n");
	
	calcstats();
	
	io.msg(IO_INFO, "ImgData::printmeta() range: %g (@%lld) -- %g (@%lld), avg: %g, sum: %g", 
		   stats.min, stats.minidx, stats.max, stats.maxidx, stats.sum/data.nel, stats.sum);
}

double ImgData::getpixel(const int idx0, const int idx1) {
	return getpixel(idx0 + data.dims[0] * idx1);
}

double ImgData::getpixel(const int idx0, const int idx1, const int idx2) {
	return getpixel(idx0 + data.dims[0] * idx1 + data.dims[0] * data.dims[1] * idx2);
}

double ImgData::getpixel(const int idx0, const int idx1, const int idx2, const int idx3) {
	return getpixel(idx0 + data.dims[0] * idx1 + data.dims[0] * data.dims[1] * idx2 + data.dims[0] * data.dims[1] * data.dims[2] * idx3);
}


double ImgData::getpixel(const int idx) {
	if (data.dt == UINT8) return (double) ((uint8_t*) data.data)[idx];
	else if (data.dt == INT8) return (double) ((int8_t*) data.data)[idx];
	else if (data.dt == UINT16) return (double) ((uint16_t*) data.data)[idx];
	else if (data.dt == INT16) return (double) ((int16_t*) data.data)[idx];
	else if (data.dt == UINT32) return (double) ((uint32_t*) data.data)[idx];
	else if (data.dt == INT32) return (double) ((int32_t*) data.data)[idx];
	else if (data.dt == UINT64) return (double) ((uint64_t*) data.data)[idx];
	else if (data.dt == INT64) return (double) ((int64_t*) data.data)[idx];
	else if (data.dt == FLOAT32) return (double) ((float*) data.data)[idx];
	else if (data.dt == FLOAT64) return (double) ((double*) data.data)[idx];
	else return (double) io.msg(IO_ERR, "ImgData::getpixel(): fail!");
}

//template <typename T>
//T ImgData::getpixel(const int idx) {
//	return ((T *) data.data)[idx];
//}

int ImgData::swapaxes(const int *order) {
	io.msg(IO_INFO | IO_NOLF, "ImgData::swapaxes() New dimension order: %d", order[0]);
	for (int d=1; d<data.ndims; d++)
		io.msg(IO_INFO | IO_NOID, ", %d", order[d]);
	io.msg(IO_INFO | IO_NOID, "\n");
	
	if (data.dt == UINT8) _swapaxes(order, (uint8_t*) data.data);
	else if (data.dt == INT8) _swapaxes(order, (int8_t*) data.data);
	else if (data.dt == UINT16) _swapaxes(order, (uint16_t*) data.data);
	else if (data.dt == INT16) _swapaxes(order, (int16_t*) data.data);
	else if (data.dt == UINT32) _swapaxes(order, (uint32_t*) data.data);
	else if (data.dt == INT32) _swapaxes(order, (int32_t*) data.data);
	else if (data.dt == UINT64) _swapaxes(order, (uint64_t*) data.data);
	else if (data.dt == INT64) _swapaxes(order, (int64_t*) data.data);
	else if (data.dt == FLOAT32) _swapaxes(order, (float*) data.data);
	else if (data.dt == FLOAT64) _swapaxes(order, (double*) data.data);
	else return (double) io.msg(IO_ERR, "ImgData::swapaxes(): fail!");
	
	return 0;
}

template <typename T>
void ImgData::_swapaxes(const int* /* order */, T datacast) {
	io.msg(IO_XNFO, "ImgData::_swapaxes(): Swapping axes now...");
	//! @todo check if data already exists
	
	if (!data.data)
		return (void) io.msg(IO_ERR, "ImgData::_swapaxes(): Cannot swap, no data loaded!");
	
	T tmp = (T) malloc(data.size);
	int i,j,k,l;
	size_t curridx, newidx;
	size_t *dim = data.dims;
	
	//	d, i, j, k, l
	//	0  0  0  0  0
	//	1  1  0  0  0 
	//	10 0  5  0  0
	//	256 0 128 0  0
	//	512 0 0  1
	
	if (data.ndims !=4)
		return (void) io.msg(IO_ERR, "ImgData::_swapaxes(): Only works for ndims=4 for now");
	
	//! @todo re-implement this
	for (size_t d=0; d<data.nel; d++) {
		i = d % dim[0];
		j = (d/dim[0]) % dim[1];
		k = (d/(dim[0]*dim[1])) % dim[2];
		l = (d/(dim[0]*dim[1]*dim[2])) % dim[3];
		curridx = i + j*dim[0] + k*dim[1]*dim[0] + l*dim[2]*dim[1]*dim[0];
		newidx = j + k*dim[1] + l*dim[1]*dim[2] + i*dim[1]*dim[2]*dim[3];
		
		if (curridx != d) return (void) io.msg(IO_ERR, "ImgData::_swapaxes(): d=%lld != curridx=%lld (%d,%d,%d,%d)", d, curridx, i,j,k,l);
		
		tmp[newidx] = datacast[curridx];
	}
	
	// Update pointer to transformed data
	free(data.data);
	data.data = tmp;
}

int ImgData::setdata(void *newdata, int nd, size_t dims[], dtype_t dt, int bpp) {
	io.msg(IO_DEB2, "ImgData::setdata(%p, %d, ..., ..., %d)", newdata, nd, bpp);
	size_t nel=1;
	
	data.data = newdata;
	data.ndims = nd;
	for (int d=0; d<nd; d++) {
		if (d >= IMGDATA_MAXNDIM)
			return io.msg(IO_ERR, "ImgData::setdata(): number of dimensions too big!");
		
		nel *= dims[d];
		data.dims[d] = dims[d];
	}
	data.dt = dt;
	data.bpp = bpp;
	
	data.nel = nel;
	data.size = nel * bpp;
	
	// New data, so stats are wrong now
	stats.init = false;
	
	// This is only a reference, so somebody 
	//! @todo Add references
	data.refs = 1;
	
	return 0;
}

gsl_matrix *ImgData::as_GSL(bool copy) {
	if (data.ndims != 2)
		return NULL;

	gsl_matrix *tmpmat;
	
	if (copy == false) {
		//! @todo implement no-copy
		return NULL;
	} else {
		// Allocate (nrows, ncols) = (height, width)
		tmpmat = gsl_matrix_alloc(data.dims[1], data.dims[0]);
		
		if (data.dt == UINT8)
			for (size_t i=0; i<data.dims[1]; i++) // Height (row)
				for (size_t j=0; j<data.dims[0]; j++) // Width (column, changes fastest)
					gsl_matrix_set(tmpmat, i, j, (double) ((uint8_t*) data.data)[i * data.dims[0] + j]);
		else if (data.dt == INT8)
			for (size_t i=0; i<data.dims[1]; i++)
				for (size_t j=0; j<data.dims[0]; j++)
					gsl_matrix_set(tmpmat, i, j, (double) ((int8_t*) data.data)[i * data.dims[0] + j]);
		else if (data.dt == UINT16)
			for (size_t i=0; i<data.dims[1]; i++)
				for (size_t j=0; j<data.dims[0]; j++)
					gsl_matrix_set(tmpmat, i, j, (double) ((uint16_t*) data.data)[i * data.dims[0] + j]);
		else if (data.dt == INT16)
			for (size_t i=0; i<data.dims[1]; i++)
				for (size_t j=0; j<data.dims[0]; j++)
					gsl_matrix_set(tmpmat, i, j, (double) ((int16_t*) data.data)[i * data.dims[0] + j]);
		else if (data.dt == UINT32)
			for (size_t i=0; i<data.dims[1]; i++)
				for (size_t j=0; j<data.dims[0]; j++)
					gsl_matrix_set(tmpmat, i, j, (double) ((uint32_t*) data.data)[i * data.dims[0] + j]);
		else if (data.dt == INT32)
			for (size_t i=0; i<data.dims[1]; i++)
				for (size_t j=0; j<data.dims[0]; j++)
					gsl_matrix_set(tmpmat, i, j, (double) ((int32_t*) data.data)[i * data.dims[0] + j]);
		else if (data.dt == UINT64)
			for (size_t i=0; i<data.dims[1]; i++)
				for (size_t j=0; j<data.dims[0]; j++)
					gsl_matrix_set(tmpmat, i, j, (double) ((uint64_t*) data.data)[i * data.dims[0] + j]);
		else if (data.dt == INT64)
			for (size_t i=0; i<data.dims[1]; i++)
				for (size_t j=0; j<data.dims[0]; j++)
					gsl_matrix_set(tmpmat, i, j, (double) ((int64_t*) data.data)[i * data.dims[0] + j]);
		else if (data.dt == FLOAT32)
			for (size_t i=0; i<data.dims[1]; i++)
				for (size_t j=0; j<data.dims[0]; j++)
					gsl_matrix_set(tmpmat, i, j, (double) ((float*) data.data)[i * data.dims[0] + j]);
		else if (data.dt == FLOAT64)
			for (size_t i=0; i<data.dims[1]; i++)
				for (size_t j=0; j<data.dims[0]; j++)
					gsl_matrix_set(tmpmat, i, j, (double) ((double*) data.data)[i * data.dims[0] + j]);
	}
	
	return tmpmat;
}

int ImgData::loaddata(const Path &f, imgtype_t t) {
	if (t == ImgData::AUTO)
		t = guesstype(f);
		
	switch (t) {
#ifdef HAVE_FITS
		case ImgData::FITS:
			return loadFITS(f);
			break;
#endif // HAVE_FITS
#ifdef HAVE_ICS
		case ImgData::ICS:
			return loadICS(f);
			break;
#endif // HAVE_ICS
		case ImgData::PGM:
			return loadPGM(f);
			break;
#ifdef HAVE_GSL
		case ImgData::GSL:
			return loadGSL(f);
			break;
#endif // HAVE_GSL
		default:
			err = ERR_TYPE_UNKNOWN;
			return io.msg(IO_ERR, "ImgData::loaddata(f=%s): Unknown datatype, cannot load file.", f.c_str());
			break;
	}
	
	return -1;
}

int ImgData::writedata(const Path &f, const imgtype_t t, const bool overwrite) {
	if (f.exists()) {
		if (!overwrite) {
			err = ERR_FILE_EXISTS;
			return io.msg(IO_ERR, "ImgData::writedata(): File '%s' exists, cannot write to disk.", f.c_str());
			return -1;
		}	else {
			// File exists, remove
			if (unlink(f.c_str())) {
				return io.msg(IO_ERR, "ImgData::writedata(): Error deleting file '%s': %s", f.c_str(), strerror(errno));
				err = ERR_UNKNOWN;
			}
		}
	}
	
	switch (t) {
#ifdef HAVE_FITS
		case ImgData::FITS:
			return writeFITS(f);
			break;
#endif // HAVE_FITS
#ifdef HAVE_ICS
		case ImgData::ICS:
			return writeICS(f);
			break;
#endif // HAVE_ICS
		case ImgData::PGM:
			return writePGM(f);
			break;
#ifdef HAVE_GSL
		case ImgData::GSL:
			return writeGSL(f);
			break;
#endif // HAVE_GSL
		default:
			err = ERR_TYPE_UNKNOWN;
			return io.msg(IO_ERR, "ImgData::writedata(): Unknown datatype, cannot write data.");
			break;
	}
}

#ifdef HAVE_FITS
int ImgData::loadFITS(const Path &file) {
	io.msg(IO_DEB2, "ImgData::loadFITS(): %s", file.c_str());
	fitsfile *fptr;
	char fits_err[30];
	int stat = 0;
	//! @todo how big should naxes be? What's the maximum according to the FITS standard?
	long naxes[8];
	int ndims = 0;
	int anynul = 0;
	data.data = NULL;
	int bitpix = -1;
	
	fits_open_file(&fptr, file.c_str(), READONLY, &stat);
	if (stat) {
		fits_get_errstatus(stat, fits_err);
		err = ERR_OPEN_FILE;
		return io.msg(IO_ERR, "ImgData::loadFITS() fits_open_file error: %s", fits_err);
	}
	
	fits_get_img_param(fptr, 8, &bitpix, &ndims, naxes, &stat);
	
	if (stat) {
		fits_get_errstatus(stat, fits_err);
		err = ERR_OPEN_FILE;
		return io.msg(IO_ERR, "ImgData::loadFITS() fits_get_img_param error: %s", fits_err);
	}

	
	data.bpp = bitpix;
	if (bitpix < 0) data.bpp *= -1;
	data.ndims = ndims;
	
	data.nel = 1;
	for (int d=0; d<data.ndims; d++) {
		data.dims[d] = naxes[d];
		data.nel *= naxes[d];
	}
	data.size = data.nel * data.bpp/8;
	data.data = (void *) malloc(data.size);
	data.refs++;
	
	io.msg(IO_DEB2, "ImgData::loadFITS(): %d: %zu x %zu x %d, %zu", data.ndims, data.dims[0], data.dims[1], data.bpp, data.nel);
	
	// BYTE_IMG (8), SHORT_IMG (16), LONG_IMG (32), LONGLONG_IMG (64), FLOAT_IMG (-32), and DOUBLE_IMG (-64)
	// TBYTE, TSBYTE, TSHORT, TUSHORT, TINT, TUINT, TLONG, TLONGLONG, TULONG, TFLOAT, TDOUBLE
	
	switch (bitpix) {
		case BYTE_IMG: {
			uint8_t nulval = 0;
			fits_read_img(fptr, TBYTE, 1, data.nel, &nulval, \
						  (uint8_t *) (data.data), &anynul, &stat);
			data.dt = UINT8;
			break;
		}
		case SHORT_IMG: {
			uint16_t nulval = 0;
			//! @bug gives datatype conversion overflow
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
			return io.msg(IO_ERR, "ImgData::loadFITS(): Unknown FITS datatype");
		}
	}
	
	fits_close_file(fptr, &stat);
	
	if (stat) {
		fits_get_errstatus(stat, fits_err);
		err = ERR_LOAD_FILE;
		if (data.data)
			free(data.data);
		
		data.data = NULL;
		data.refs--;
		return io.msg(IO_ERR, "ImgData::loadFITS() fits_read_img error: %s", fits_err);
	}
	
	stats.init = false;
	
	return 0;
}
#else
int ImgData::loadFITS(const Path &file) {
	return io.msg(IO_ERR, "ImgData::loadFITS(): not supported, library was not available during compilation.");
}
#endif // HAVE_FITS

#ifdef HAVE_ICS
int ImgData::loadICS(const Path &file) {
	io.msg(IO_DEB2, "ImgData::loadICS(): %s", file.c_str());

	// Init ICS variables
	::ICS *ip;
	const char *errtxt;
	Ics_DataType dt;
	Ics_Error retval;
	data.data = NULL;
	
	// Open ICS file
	retval = IcsOpen(&ip, file.c_str(), "r");
	if (retval != IcsErr_Ok) {
		errtxt = IcsGetErrorText(retval); 
		err = ERR_OPEN_FILE;
		return io.msg(IO_ERR, "ImgData::loadICS(): Could not open file '%s': %s.", file.c_str(), errtxt);
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
	data.refs++;
	retval = IcsGetData (ip, data.data, data.size);
	if (retval != IcsErr_Ok) {
		errtxt = IcsGetErrorText(retval); 
		err = ERR_LOAD_FILE;
		if (data.data)
			free(data.data);
		data.data = NULL;
		data.refs--;
		return io.msg(IO_ERR, "ImgData::loadICS(): Could not read data from '%s': %s.", file.c_str(), errtxt);
	}
	
	retval = IcsClose (ip);
	
	if (dt == Ics_uint8) data.dt = UINT8;
	else if (dt ==  Ics_sint8) data.dt = INT8;
	else if (dt ==  Ics_uint16) data.dt = UINT16;
	else if (dt ==  Ics_sint16)	data.dt = INT16;
	else if (dt ==  Ics_uint32)	data.dt = UINT32;
	else if (dt ==  Ics_sint32)	data.dt = INT32;
	else if (dt ==  Ics_real32)	data.dt = FLOAT32;
	else if (dt ==  Ics_real64)	data.dt = FLOAT64;
	else if (dt ==  Ics_complex32 || dt ==  Ics_complex64) {
		err = ERR_TYPE_UNSUPP;
		return io.msg(IO_ERR, "ImgData::loadICS(): Unsupported datatype (complex), cannot process file");
	} else {
		err = ERR_TYPE_UNKNOWN;
		return io.msg(IO_ERR, "ImgData::loadICS(): Unknown datatype, cannot process file");
	}
	
	return (int) !(retval == IcsErr_Ok);
}
#else
int ImgData::loadICS(const Path &file) {
	return io.msg(IO_ERR, "ImgData::loadICS(): not supported, library was not available during compilation.");
}
#endif // HAVE_ICS

int ImgData::loadPGM(const Path &file) {
	io.msg(IO_DEB2, "ImgData::loadPGM(): %s", file.c_str());

	// see http://netpbm.sourceforge.net/doc/pgm.html
	FILE *fd;
	int n, maxval;
	char magic[2];
	
	fd = fopen(file.c_str(), "rb");
	if (!fd) {
		err = ERR_OPEN_FILE;
		return io.msg(IO_ERR, "ImgData::loadPGM(): Error opening file '%s'.", file.c_str());
	}
	
	// Read magic
	fread(magic, 2, 1, fd);
	if (ferror(fd)) {
		err = ERR_LOAD_FILE;
		return io.msg(IO_ERR, "ImgData::loadPGM(): Error reading PGM file.");
	}
	
	// Resolution
	data.ndims = 2;
	data.dims[0] = readNumber(fd);
	data.dims[1] = readNumber(fd);
	data.nel = data.dims[0] * data.dims[1];
	
	if (data.dims[0] <= 0 || data.dims[1] <= 0) {
		err = ERR_LOAD_FILE;
		return io.msg(IO_ERR, "ImgData::loadPGM(): Unable to read image width and height");
	}
	
	// Maxval
	maxval = readNumber(fd);
	if (maxval <= 0 || maxval > 65536) {
		err = ERR_TYPE_UNSUPP;
		return io.msg(IO_ERR, "ImgData::loadPGM(): Unsupported PGM format");
	}
	
	if (maxval <= 255) {
		data.dt = UINT8;
		data.bpp = 8;
	}
	else {		
		data.dt = UINT16;
		data.bpp = 16;
	}
	data.data = malloc(data.nel * data.bpp/8);
	data.refs++;
	
	// Read the rest
	if (!strncmp(magic, "P5", 2)) { // Binary
		n = fread(data.data, data.bpp/8, data.nel, fd);
		if (ferror(fd)) {
			err = ERR_LOAD_FILE;
			if (data.data)
				free(data.data);
			data.data = NULL;
			data.refs--;
			return io.msg(IO_ERR, "ImgData::loadPGM(): Could not read file.");
		}
	}
	else if (!strncmp(magic, "P2", 2)) { // ASCII
		if (data.dt == UINT8)
			for (size_t p=0; p < data.nel; p++)
				((uint8_t *) data.data)[p] = readNumber(fd);
		else
			for (size_t p=0; p < data.nel; p++)
				((uint16_t *) data.data)[p] = readNumber(fd);
	}
	else {
		err = ERR_TYPE_UNSUPP;
		return io.msg(IO_ERR, "ImgData::loadPGM(): Unsupported PGM format");
	}
	
	return 0;
}

int ImgData::readNumber(FILE *fd) {
	int number;
	unsigned char ch;
	
	number = 0;
	
	do {
		if (!fread(&ch, 1, 1, fd)) return -1;
		if ( ch == '#' ) {  // Comment is '#' to end of line
			do {
				if (!fread(&ch, 1, 1, fd)) return -1;
			} while ( (ch != '\r') && (ch != '\n') );
		}
	} while ( isspace(ch) );
	
	// Read number
	do {
		number *= 10;
		number += ch-'0';
		
		if (!fread(&ch, 1, 1, fd)) return -1;
	} while ( isdigit(ch) );
	
	return number;
}


#ifdef HAVE_GSL
int ImgData::loadGSL(const Path & /* file */) {
	err = ERR_TYPE_UNSUPP;
	return io.msg(IO_ERR, "ImgData::loadGSL(): Not implemented yet");
}
#else
int ImgData::loadGSL(const Path & /* file */) {
	return io.msg(IO_ERR, "lImgData::loadGSL(): Not supported, librabry was not available during compilation.");
}
#endif // HAVE_GSL

#ifdef HAVE_FITS
int ImgData::writeFITS(const Path &file) {
	io.msg(IO_XNFO, "ImgData::writeFITS('%s')", file.c_str());
	
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
		return io.msg(IO_ERR, "ImgData::writeFITS(): Could not create file '%s' for writing: %s.", file.c_str(), fitserr);
	}
	
	// Get FITS datatype
	int bitpix, dtype;
	if (data.dt == UINT8) { bitpix = BYTE_IMG; dtype = TBYTE; }
	else if (data.dt == INT8) { bitpix = BYTE_IMG; dtype = TSBYTE; }
	else if (data.dt == UINT16) { bitpix = SHORT_IMG; dtype = TUSHORT; }
	else if (data.dt == INT16) { bitpix = SHORT_IMG; dtype = TSHORT; }
	else if (data.dt == UINT32) { bitpix = LONG_IMG; dtype = TUINT; }
	else if (data.dt == INT32) { bitpix = LONG_IMG; dtype = TINT; }
	else if (data.dt == UINT64) { bitpix = LONGLONG_IMG; dtype = TULONG; }
	else if (data.dt == INT64) { bitpix = LONGLONG_IMG; dtype = TLONG; }
	else if (data.dt == FLOAT32) { bitpix = FLOAT_IMG; dtype = TFLOAT; }
	else if (data.dt == FLOAT64) { bitpix = DOUBLE_IMG; dtype = TDOUBLE; }
	else { 
		err = ERR_TYPE_UNKNOWN;
		return io.msg(IO_ERR, "ImgData::writeFITS(): Unknown datatype for FITS");
	}
	
	// create & write image
	fits_create_img(fptr, bitpix, naxis, naxes, &status);
	if (status) {
		fits_read_errmsg(fitserr);
		err = ERR_CREATE_IMG;
		return io.msg(IO_ERR, "ImgData::writeFITS(): Could not create FITS image: %s", fitserr);
	}
	
	fits_write_img(fptr, dtype, fpixel, nelements, data.data, &status);
	if (status) {
		fits_read_errmsg(fitserr);
		err =  ERR_WRITE_FILE;
		return io.msg(IO_ERR, "ImgData::writeFITS(): Could not write FITS image: %s", fitserr);
	}
	
	fits_close_file(fptr, &status);
	return 0;	
}
#else
int ImgData::writeFITS(const Path&) {
	return io.msg(IO_ERR, "ImgData::writeFITS() not supported, library was not available during compilation.");
	
}
#endif // HAVE_FITS

#ifdef HAVE_ICS
int ImgData::writeICS(const Path& /* file */) {
	err = ERR_TYPE_UNSUPP;
	return io.msg(IO_ERR, "ImgData::writeICS(): Not implemented yet");
}
#else
int ImgData::writeICS(const Path&) {
	return io.msg(IO_ERR, "ImgData::writeICS() not supported, library was not available during compilation.");
	
}
#endif // HAVE_ICS

int ImgData::writePGM(const Path &file) {
	io.msg(IO_DEB2, "ImgData::writePGM()");	
	
	FILE *fd = fopen(file.c_str(), "wb+");
	
	int maxval=0;
	switch (data.dt) {
		case UINT8: {
			uint8_t *datap = (uint8_t *) data.data;
			for (size_t p=0; p<data.dims[0] * data.dims[1]; p++)
				if (datap[p] > maxval) maxval = datap[p];
			fprintf(fd, "P5\n%zu %zu\n%d\n", data.dims[0], data.dims[1], maxval);
			if (fwrite(datap, data.size, 1, fd) < 1)
				io.msg(IO_ERR, "ImgData::writePGM() Error writing file, something went wrong.");
			break;
		}
		case UINT16: {
			uint16_t *datap = (uint16_t *) data.data;
			for (size_t p=0; p<data.dims[0] * data.dims[1]; p++)
				if (datap[p] > maxval) maxval = datap[p];
			fprintf(fd, "P5\n%zu %zu\n%d\n", data.dims[0], data.dims[1], maxval);
			if (fwrite(datap, data.size, 1, fd) < 1)
				io.msg(IO_ERR, "ImgData::writePGM() Error writing file, something went wrong.");
			break;
		}			
		default:
			io.msg(IO_ERR, "ImgData::writePGM() PGM only supports unsigned 8- or 16-bit integer images, cannot save.");
			break;
	}
	
	fclose(fd);
	
	return 0;	
}

#ifdef HAVE_GSL
int ImgData::writeGSL(const Path& /* file */) {
	err = ERR_TYPE_UNSUPP;
	return io.msg(IO_ERR, "ImgData::writeGSL(): Not implemented yet");		
}
#else
int ImgData::writeGSL(const Path&) {
	return io.msg(IO_ERR, "ImgData::writeGSL(): not supported, library was not available during compilation.");

}
#endif // HAVE_GSL

