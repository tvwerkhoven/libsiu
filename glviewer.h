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

// Default scaling steps and range
const double SCALESTEP = 1.0/3.0;
const double SCALEMIN = -5.0;
const double SCALEMAX = 5.0;

/*! 
 @brief OpenGL scrolled area
 
 An OpenGL viewing area with scrolling and zooming implemented.
 
 Coordinates involved:
 gtkimage is the drawing area in the GTK application, with size get_width() 
 and get_height(). We set up a gl viewport with that size to use the whole
 area. The image stretches from (-1,-1) to (1,1) in OpenGL coordinates, 
 and is offset by (sx, sy). To convert from one coordinate system to 
 another, see map_coordinates().
 */
class OpenGLImageViewer: public Gtk::EventBox {
	Glib::RefPtr<Gdk::GL::Config> glconfig;	//!< OpenGL configuration
	Glib::RefPtr<Gdk::GL::Window> glwindow;	//!< OpenGL window
	Gtk::GL::DrawingArea gtkimage;			//!< GTK drawingarea
	
	double scale;						//!< Tracks image scaling (zooming)
	double scalemin;				//!< Scale range min
	double scalemax;				//!< Scale range max
	
	float sx, sy;						//!< Current image displacement
	float sxstart, systart;	//!< Tracks mouse dragging 
	gdouble xstart, ystart;	//!< Tracks mouse dragging

	bool flipv;							//!< Vertical flip toggle
	bool fliph;							//!< Horizontal flip toggle
	bool zoomfit;						//!< Fit image to parent window
	bool crosshair;					//!< Crosshair toggle
	bool pager;							//!< Pager toggle	
		
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
	void force_update();
	
public:
	// Transform coordinates direction
	typedef enum {
		GLTOGTK=1,
		GTKTOGL,
		GTKTODATA
	} map_dir_t;
	
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
	
	void setscalerange(double min, double max) { scalemin = min; scalemax = max; }
	void setscalerange(double minmax) { scalemax = scalemin = minmax; }
	
	void setshift(float, float);
	void setshift(float s) { setshift(s, s); }
	
	void setflipv(bool v = true) { flipv = v; }
	bool getflipv() { return flipv; }
	
	void setfliph(bool v = true) { fliph = v; }
	bool getfliph() { return fliph; }
	
	void setzoomfit(bool v = true) { zoomfit = v; }
	bool getzoomfit() { return zoomfit; }

	void setcrosshair(bool v = true) { crosshair = v; }
	bool getcrosshair() { return crosshair; }

	void setpager(bool v = true) { pager = v; }
	bool getpager() { return pager; }
	
	void linkData(void *data, int depth, int w, int h);
};


#endif // HAVE_GLVIEWER_H
