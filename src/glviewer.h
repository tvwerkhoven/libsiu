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

#include <stdint.h>

#include <gtkmm.h>
#include <gtkglmm.h>
#include <gdkmm/pixbuf.h>
#include <GL/glext.h>

#include "types.h"

// Default scaling steps and range
const double SCALESTEP = 1.0/3.0;
const double SCALEMIN = -5.0;
const double SCALEMAX = 5.0;

/*! 
 @brief OpenGL scrolled area
 
 An OpenGL viewing area with scrolling and zooming implemented.
 
 Coordinates involved:
 - The outer frame is gtkimage, a Gtk::GL::DrawingArea. The size is get_width() by get_height(). Axes increase towards bottom-right
 - We use a gl viewport matching the size of gtkimage, we map the texture to coordinates (-1, -1) to (1, 1). Axes increase towards top-right
 - The image (data) is drawn with an offset of (sx, sy) (in OpenGL), data origin is at (-1, -1) (in OpenGL)
 
 To convert from one coordinate system to another, see map_coordinates() and map_dir_t.
 
 Scale is used for image scaling, which is a logarithmic scale.
 
 Optional overlays include:
 - Crosshair through the middle of the image
 - Grid of lines over the image
 
 Other features:
 - Flip horizontal or vertical
 - Zoom in/out/fit to window
 - Add boxes and/or lines with addbox(), addline() for overlays.
 
 */
class OpenGLImageViewer: public Gtk::EventBox {
private:
	Glib::RefPtr<Gdk::GL::Config> glconfig;	//!< OpenGL configuration
	Glib::RefPtr<Gdk::GL::Window> glwindow;	//!< OpenGL window
	Gtk::GL::DrawingArea gtkimage;			//!< GTK drawingarea
	
	double sx, sy;						//!< Current image displacement
	double sxstart, systart;	//!< Tracks mouse dragging 
	gdouble xstart, ystart;	//!< Tracks mouse dragging
	
	double scale;						//!< Tracks image scaling (zooming)
	double scalemin;				//!< Scale range min
	double scalemax;				//!< Scale range max
	
	coord_t ngrid;					//!< Grid overlay (number of cells)
	bool grid;							//!< Overlay grid toggle
	
	bool flipv;							//!< Vertical flip toggle
	bool fliph;							//!< Horizontal flip toggle
	bool zoomfit;						//!< Fit image to parent window
	bool crosshair;					//!< Crosshair toggle
	
	std::vector<fvector_t> boxes;				//!< Draw these extra boxes
	std::vector<fvector_t> lines;				//!< Draw these extra lines
	
	// OpenGL drawing-related events
	void on_image_configure_event(GdkEventConfigure *event);
	void on_image_expose_event(GdkEventExpose *event);
	void on_image_realize();
	
	// Scroll & zoom events
	bool on_image_motion_event(GdkEventMotion *event);
	bool on_image_button_event(GdkEventButton *event);
	bool on_image_scroll_event(GdkEventScroll *event);
	
	// Zoom step functions
	void on_zoomin_activate() { scalestep(-SCALESTEP); }
	void on_zoomout_activate() { scalestep(SCALESTEP); }
	
	void on_update();
	
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
	typedef struct {
		uint16_t d;	//!< Depth (8 or 16)
		uint16_t w;	//!< Width in pixels
		uint16_t h;	//!< Height in pixels
		const void *data;	//!< Pointer to data (should be contiguous)
	} gl_img_t;
	
	gl_img_t gl_img;
	
	Glib::Dispatcher view_update;
	
	OpenGLImageViewer();
	~OpenGLImageViewer();
	
	void do_update();
	
	void link_data(const void * const data, const int depth, const int w, const int h);

	int map_coord(const float inx, const float iny, float * const outx, float * const outy, const map_dir_t direction) const;
	int map_coord(const double inx, const double iny, double * const outx, double * const outy, const map_dir_t direction) const;
	//!< Add an overlay box
	void addbox(const fvector_t box, const map_dir_t conv=UNITY);
	//!< Remove an overlay box by index number
	void delbox(const size_t idx) { boxes.erase (boxes.begin()+idx); }
	//!< Check whether (x,y) is inside a box, return index. Must be GTK coordinates!
	int inbox(const double x, const double y) const;
	//!< Return box with index idx
	fvector_t getbox(const size_t idx) const { return boxes[idx]; }
	
	//!< Add an overlay line
	void addline(const fvector_t line, const map_dir_t conv=UNITY);
	//!< Remove an overlay line by index number
	void delline(const size_t idx) { lines.erase (lines.begin()+idx); }
	//!< Return line with index idx 
	fvector_t getline(const size_t idx) const { return lines[idx]; }
	
	void setscale(const double);
	void scalestep(const double step) { setzoomfit(false); setscale(scale + step); }
	double getscale() const { return scale; }
		
	void setscalerange(const double min, const double max) { scalemin = min; scalemax = max; }
	void setscalerange(const double minmax) { scalemax = scalemin = minmax; }
	
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
