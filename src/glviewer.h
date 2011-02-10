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
 
 To convert from one coordinate system to another, see map_coordinates().
 
 Scale is used for image scaling, which is a logarithmic scale.
 
 Optional overlays include:
 - Crosshair through the middle of the image
 - Grid of lines over the image
 
 Other features:
 - Flip horizontal or vertical
 - Zoom in/out/fit to window
 */
class OpenGLImageViewer: public Gtk::EventBox {
private:
	Glib::RefPtr<Gdk::GL::Config> glconfig;	//!< OpenGL configuration
	Glib::RefPtr<Gdk::GL::Window> glwindow;	//!< OpenGL window
	Gtk::GL::DrawingArea gtkimage;			//!< GTK drawingarea
	
	double scale;						//!< Tracks image scaling (zooming)
	double scalemin;				//!< Scale range min
	double scalemax;				//!< Scale range max
	
	float sx, sy;						//!< Current image displacement
	float sxstart, systart;	//!< Tracks mouse dragging 
	gdouble xstart, ystart;	//!< Tracks mouse dragging
	
	coord_t ngrid;					//!< Grid overlay (number of cells)
	bool grid;							//!< Overlay grid toggle
	
	bool flipv;							//!< Vertical flip toggle
	bool fliph;							//!< Horizontal flip toggle
	bool zoomfit;						//!< Fit image to parent window
	bool crosshair;					//!< Crosshair toggle
	
	std::vector<fdvector_t> boxes;				//!< Draw these extra boxes
	std::vector<fdvector_t> lines;				//!< Draw these extra lines
	
	// OpenGL drawing-related events
	void on_image_configure_event(GdkEventConfigure *event);
	void on_image_expose_event(GdkEventExpose *event);
	void on_image_realize();
	
	// Scroll & zoom events
	bool on_image_scroll_event(GdkEventScroll *event);
	bool on_image_button_event(GdkEventButton *event);
	bool on_image_motion_event(GdkEventMotion *event);
	
	// Zoom step functions
	void on_zoomin_activate() { scalestep(-SCALESTEP); }
	void on_zoomout_activate() { scalestep(SCALESTEP); }
	
	void on_update();
	
public:
	typedef enum {
		UNITY=1,		//!< Do not convert
		GLTOGTK,		//!< Convert OpenGL to GTK coordinates
		GLTODATA,		//!< Convert OpenGL to data coordinates
		GTKTOGL,		//!< Convert GTK to OpenGL coordinates
		GTKTODATA,	//!< Convert GTK to data coordinates
	} map_dir_t;												//!< Transform coordinates
	
	//!< Data wrapper
	typedef struct {
		uint16_t d;	//!< Depth (8 or 16)
		uint16_t w;	//!< Width in pixels
		uint16_t h;	//!< Height in pixels
		void *data;	//!< Pointer to data (should be contiguous)
	} gl_img_t;
	
	gl_img_t gl_img;
	
	Glib::Dispatcher view_update;
	
	OpenGLImageViewer();
	~OpenGLImageViewer();
	
	void do_update();

	int map_coord(double inx, double iny, double *outx, double *outy, map_dir_t direction);
	void setscale(double);
	void scalestep(double step) { setzoomfit(false); setscale(scale + step); }
	double getscale() { return scale; }
	
	void addbox(const fdvector_t box, map_dir_t direction=UNITY) { boxes.push_back(box); }
	void delbox(const int idx) { boxes.erase (boxes.begin()+idx); }
	
	void addline(const fdvector_t line, map_dir_t direction=UNITY) { lines.push_back(line); }
	void delline(const int idx) { lines.erase (lines.begin()+idx); }
	
	void setscalerange(double min, double max) { scalemin = min; scalemax = max; }
	void setscalerange(double minmax) { scalemax = scalemin = minmax; }
	
	void setshift(float, float);
	void setshift(float s) { setshift(s, s); }
	void getshift(float *x, float *y) { *x = sx; *y = sy; }
	
	void setgrid(int, int);
	void setgrid(int n) { setgrid(n, n); }
	void setgrid(bool v = true) { grid = v; }
	bool getgrid() { return grid; }
	
	void setflipv(bool v = true) { flipv = v; }
	bool getflipv() { return flipv; }
	
	void setfliph(bool v = true) { fliph = v; }
	bool getfliph() { return fliph; }
	
	void setzoomfit(bool v = true) { zoomfit = v; }
	bool getzoomfit() { return zoomfit; }

	void setcrosshair(bool v = true) { crosshair = v; }
	bool getcrosshair() { return crosshair; }

//	void setpager(bool v = true) { pager = v; }
//	bool getpager() { return pager; }
	
	void linkData(void *data, int depth, int w, int h);
};


#endif // HAVE_GLVIEWER_H
