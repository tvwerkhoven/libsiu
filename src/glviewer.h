/*
 glviewer.h -- OpenGL viewing area implementation
 Copyright (C) 2010  Guus Sliepen <guus@sliepen.eu.org> &
 Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
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

#ifndef HAVE_GLVIEWER_H
#define HAVE_GLVIEWER_H

#include "autoconfig.h"

//#define GL_GLEXT_PROTOTYPES
////#define GL_ARB_IMAGING
//#define GL_ARB_imaging
//
//#ifdef HAVE_GL_GL_H
//#include "GL/gl.h"
//#elif HAVE_OPENGL_GL_H
//#include "OpenGL/gl.h"
//#endif
//
//#include <GL/glext.h>
//
//#ifdef HAVE_GL_GLU_H
//#include "GL/glu.h"
//#elif HAVE_OPENGL_GLU_H 
//#include "OpenGL/glu.h"
//#endif
//
//#ifdef HAVE_GL_GLUT_H
//#include "GL/glut.h"
//#elif HAVE_GLUT_GLUT_H 
//#include "GLUT/glut.h"
//#endif

#include <stdint.h>
#include <gtkmm.h>
#include <gtkglmm.h>
#include <gdkmm/pixbuf.h>

#include "pthread++.h"
#include "types.h"

static GLuint program1 = 0;
static GLuint program2 = 0;

// Default scaling steps and range
const double SCALESTEP = 1.0/3.0;
const double SCALEMIN = -8.0;
const double SCALEMAX = 8.0;

/*! 
 @brief OpenGL scrolled area
 
 An OpenGL viewing area with scrolling and zooming implemented.
 
 Coordinates involved:
 - The outer frame is gtkimage, a Gtk::GL::DrawingArea. The size is get_width() by get_height(). Axes increase towards bottom-right
 - We use a gl viewport matching the size of gtkimage, we map the texture to coordinates (-1, -1) to (1, 1). gl viewport coordinate axes increase towards top-right
 - The image (data) is drawn with an offset of (sx, sy) (in OpenGL), data origin is at (-1, -1) (in OpenGL). Data coordinates increase towards top-right
 
 To convert from one coordinate system to another, see map_coordinates() and map_dir_t.
 
 Scale is used for image scaling, which is a logarithmic scale.
 
 Optional overlays include:
 - Crosshair through the middle of the image
 - Grid of lines over the image
 - Boxes and/or lines with addbox(), addline() for overlays (in DATA coordinates)

 Other features:
 - Flip horizontal or vertical (with flipv and fliph)
 - Zoom in/out/fit to window (with scrolling) (using scale, scalemin, scalemax)
 - Under- and over-exposure indicator (through setunderover())
 - Data display clipping (with setminmax())
 - Connect to view_update which is signalled when view settings change (zoom, pan)
 
 */
class OpenGLImageViewer: public Gtk::EventBox {
private:
	typedef struct {
		GLfloat u;
		GLfloat v;
		GLfloat nx;
		GLfloat ny;
		GLfloat nz;
		GLfloat x;
		GLfloat y;
		GLfloat z;
	} GLt2n3v3f;

	Glib::RefPtr<Gdk::GL::Config> glconfig;	//!< OpenGL configuration
	Glib::RefPtr<Gdk::GL::Window> glwindow;	//!< OpenGL window
	Gtk::GL::DrawingArea gtkimage;			//!< GTK drawingarea
	
	double sx, sy;						//!< Current image displacement
	double sxstart, systart;	//!< Tracks mouse dragging 
	gdouble xstart, ystart;	//!< Tracks mouse dragging
	
	double scale;						//!< Tracks image scaling (zooming)
	double scalemin;				//!< Scale range min
	double scalemax;				//!< Scale range max
	
	double minval;					//!< Minimum intensity to display (for clipping)
	double maxval;					//!< Maximum intensity to display (for clipping)
	
	double rscale;					//!< Red value scaling
	double gscale;					//!< Green value scaling
	double bscale;					//!< Blue value scaling
	
	bool underover;					//!< Apply under- and over-exposure masking
	
	coord_t ngrid;					//!< Grid overlay (number of cells)
	bool grid;							//!< Overlay grid toggle
	
	bool flipv;							//!< Vertical flip toggle
	bool fliph;							//!< Horizontal flip toggle
	bool zoomfit;						//!< Fit image to parent window
	bool crosshair;					//!< Crosshair toggle
	
	bool boxcross;					//!< Crosshair in boxes
		
	std::vector<fvector_t> boxes;				//!< Draw these extra boxes
	std::vector<fvector_t> lines;				//!< Draw these extra lines
	
	pthread::mutex gui_mutex;	//!< Mutex for overlay read/writes
	
	// OpenGL drawing-related events
	void on_image_configure_event(GdkEventConfigure *event);	//!< When widget changes sie
	void on_image_expose_event(GdkEventExpose *event); //!< When widgets need to be redrawn
	void on_image_realize();						//!< When widget is first shown on a screen
	
	// Scroll & zoom events
	bool on_image_motion_event(GdkEventMotion *event);
	bool on_image_button_event(GdkEventButton *event);
	bool on_image_scroll_event(GdkEventScroll *event);
	
	// Zoom step functions
	void on_zoomin_activate() { scalestep(-SCALESTEP); }
	void on_zoomout_activate() { scalestep(SCALESTEP); }
	
	//!< Do full update of screen, including (camera) image
	void do_full_update();
	
public:
	typedef enum {
		UNITY=1,		//!< Do not convert
		GLTOGTK,		//!< OpenGL to GTK coordinates
		GLTODATA,		//!< OpenGL to data coordinates
		GTKTOGL,		//!< GTK to OpenGL coordinates
		GTKTODATA,	//!< GTK to data coordinates
		DATATOGL,		//!< Data to OpenGL coordinates
		DATATOGTK		//!< Data to GTK coordinates
	} map_dir_t;												//!< Transform coordinates
	
	//!< Data wrapper
	typedef struct _gl_img_t {
		int d;	//!< Depth (8 or 16)
		int w;	//!< Width in pixels
		int h;	//!< Height in pixels
		const void *data;	//!< Pointer to data (should be contiguous)
		_gl_img_t(): d(8), w(-1), h(-1), data(NULL) { }
	} gl_img_t;
	
	gl_img_t gl_img;
	
	//!< Dispatcher called when view settings are changed
	Glib::Dispatcher view_update;
	
	OpenGLImageViewer();
	~OpenGLImageViewer();
	
	//!< Redraw OpenGl area, use previously initialized texture.
	void do_update();
	
	void link_data(const void * const data, const int depth, const int w, const int h);
	void set_data(const int d, const int w, const int h) { gl_img.d = d; gl_img.w = w; gl_img.h = h; }

	int map_coord(const float inx, const float iny, float * const outx, float * const outy, const map_dir_t direction) const;
	int map_coord(const double inx, const double iny, double * const outx, double * const outy, const map_dir_t direction) const;
	//!< Add an overlay box, should be in DATA coordinates
	void addbox(const fvector_t box) { pthread::mutexholder h(&gui_mutex); boxes.push_back(box); }
	//!< Remove an overlay box by index number
	void delbox(const size_t idx) { pthread::mutexholder h(&gui_mutex); boxes.erase (boxes.begin()+idx); }
	//!< Clear all overlay boxes
	void clearboxes() { pthread::mutexholder h(&gui_mutex); boxes.clear(); }
	//!< Check whether (x,y) is inside a box, return index. Must be GTK coordinates!
	int inbox(const double x, const double y) const;
	//!< Return box with index idx
	fvector_t getbox(const size_t idx) const { return boxes[idx]; }
	
	void set_boxcross(const bool v) { boxcross = v; }
	bool get_boxcros() const { return boxcross; }
	
	//!< Add an overlay line, should be in DATA coordinates
	void addline(const fvector_t line) { pthread::mutexholder h(&gui_mutex); lines.push_back(line); }
	//!< Remove an overlay line by index number
	void delline(const size_t idx) { pthread::mutexholder h(&gui_mutex); lines.erase (lines.begin()+idx); }
	//!< Remove all overlay lines
	void clearlines() { pthread::mutexholder h(&gui_mutex); lines.clear(); }
	//!< Return line with index idx 
	fvector_t getline(const size_t idx) const { return lines[idx]; }
	
	void setscale(const double);
	void scalestep(const double step) { setzoomfit(false); setscale(scale + step); }
	double getscale() const { return scale; }
		
	void setscalerange(const double min, const double max) { scalemin = min; scalemax = max; }
	void setscalerange(const double minmax) { scalemax = scalemin = minmax; }
	
	void setminmax(const double min, const double max) { minval = min; maxval = max; }
	void setminmax() { minval = 0; maxval = ((size_t) 1) << gl_img.d; }

	void setunderover(const bool f=true) { underover = f; }
	bool getunderover() const { return underover; }

	void setshift(const double, const double);
	void setshift(const double s) { setshift(s, s); }
	void getshift(double * const x, double * const y) { *x = sx; *y = sy; }
	
	void setgrid(const int, const int);
	void setgrid(const int n) { setgrid(n, n); }
	void setgrid(const bool v = true) { grid = v; }
	bool getgrid() const { return grid; }
	
	void setflipv(const bool v = true) { flipv = v; }
	bool getflipv() const { return flipv; }
	
	void setfliph(const bool v = true) { fliph = v; }
	bool getfliph() const { return fliph; }
	
	void setzoomfit(const bool v = true) { zoomfit = v; }
	bool getzoomfit() const { return zoomfit; }

	void setcrosshair(const bool v = true) { crosshair = v; }
	bool getcrosshair() const { return crosshair; }
};


#endif // HAVE_GLVIEWER_H
