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

#include <iostream>
#include <cstdlib>

#ifdef HAVE_GL_GL_H
#include "GL/gl.h"
#elif HAVE_OPENGL_GL_H
#include "OpenGL/gl.h"
#endif

#ifdef HAVE_GL_GLU_H
#include "GL/glu.h"
#elif HAVE_OPENGL_GLU_H 
#include "OpenGL/glu.h"
#endif

#include "glviewer.h"

// TODO: review coordinate systems in use and document
// TODO: update image shift clamping

using namespace std;
using namespace Gtk;

static double clamp(double val, double min, double max) {
	return val < min ? min : val > max ? max : val;
}

OpenGLImageViewer::OpenGLImageViewer():
scale(0), scalemin(SCALEMIN), scalemax(SCALEMAX),
flipv(false), fliph(false), crosshair(false), pager(false) 
{
	glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA | Gdk::GL::MODE_DOUBLE);
	if(!glconfig) {
		fprintf(stderr, "OpenGLImageViewer(): Not double-buffered!\n");
		glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA);
	}
	if(!glconfig)
		throw runtime_error("OpenGLImageViewer(): Could not create OpenGL-capable visual");
	
	gtkimage.set_gl_capability(glconfig);
	gtkimage.set_double_buffered(false);
	
	// Init with empty image
	gl_img.d = gl_img.w = gl_img.h = 0;
	gl_img.data = NULL;
	
	setshift(0.0);
	setscale(0.0);
		
	// Callbacks
	gtkimage.signal_configure_event().connect_notify(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_configure_event));
	gtkimage.signal_expose_event().connect_notify(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_expose_event));
	gtkimage.signal_realize().connect(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_realize));
	
	// Set zoom & image drag controls
	signal_scroll_event().connect(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_scroll_event));
	signal_button_press_event().connect(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_button_event));
	signal_button_release_event().connect(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_button_event));
	signal_motion_notify_event().connect(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_motion_event));
	add_events(Gdk::POINTER_MOTION_MASK | 
						 Gdk::POINTER_MOTION_HINT_MASK | 
						 Gdk::BUTTON_MOTION_MASK |
						 Gdk::BUTTON1_MOTION_MASK |
						 Gdk::BUTTON2_MOTION_MASK |
						 Gdk::BUTTON3_MOTION_MASK |
						 Gdk::BUTTON_PRESS_MASK |
						 Gdk::BUTTON_RELEASE_MASK);
	
	add(gtkimage);
}

OpenGLImageViewer::~OpenGLImageViewer() {
}

bool OpenGLImageViewer::on_image_motion_event(GdkEventMotion *event) {
	if (event->type == GDK_MOTION_NOTIFY && (event->state & GDK_BUTTON1_MASK)) {
		// When the mouse is moved and the mouse key is pressed, drag the image
		double s = pow(2.0, scale);	
		sx = sxstart + 2 * (event->x - xstart) / s / gl_img.w;
		sy = systart - 2 * (event->y - ystart) / s / gl_img.h;
		do_update();
		return true;
	}
	return false;
}

bool OpenGLImageViewer::on_image_button_event(GdkEventButton *event) {
	if (event->type == GDK_2BUTTON_PRESS) {
		// Double-click: reset translation
		view_update();
		sx = sy = 0;
		do_update();
	}
	else if (event->type == GDK_3BUTTON_PRESS) {
		// Triple-click: reset translation & zoom
		view_update();
		scale = 0;
		do_update();		
	}
	else {
		// Normal click: remember current translation, use in on_image_motion_event()
		view_update();
		sxstart = sx;
		systart = sy;
		xstart = event->x;
		ystart = event->y;
	}
	return true;
}

int OpenGLImageViewer::map_coord(double inx, double iny, double *outx, double *outy, map_dir_t direction) {
	// Translate coordinates from one frame to another
	if (direction == GTKTOGL) {
		double s = pow(2.0, scale);	
		int xfac = (fliph) ? -1 : 1;
		int yfac = (flipv) ? -1 : 1;
		
		*outx = xfac * 2 * (inx - gtkimage.get_width()/2) / s / gl_img.w;
		*outy = yfac * -1 * 2 * (iny - gtkimage.get_height()/2) / s / gl_img.h;
		*outx -= xfac * sx;
		*outy -= yfac * sy;
		return 0;
	}
	else if (direction == GTKTODATA) {
		double tmpx, tmpy;
		// First convert to OpenGL (-1 to 1)
		map_coord(inx, iny, &tmpx, &tmpy, GTKTOGL);
		if (fabs(tmpx) > 1.0 || fabs(tmpy) > 1.0)
			return -1;
		// Now multiply with data size
		*outx = (tmpx+1)/2. * gl_img.w;
		*outy = (tmpy+1)/2. * gl_img.h;
		return 0;
	}
	else if (direction == GLTOGTK) {
		// center is at (sx, sy) * s * (gl_img.w, gl_img.h) / 2
		// 1,1 is at (sx, sy) * s * (gl_img.w, gl_img.h) / 2
		return -1;
	}
	return -1;
}

void OpenGLImageViewer::linkData(void *data, int depth, int w, int h) {
	// Link to new data
	gl_img.d = depth;
	gl_img.w = w;
	gl_img.h = h;
	gl_img.data = data;
	// Update frame
	on_update();
}

bool OpenGLImageViewer::on_image_scroll_event(GdkEventScroll *event) {
	if(event->direction == GDK_SCROLL_UP)
		on_zoomin_activate();
	else if(event->direction == GDK_SCROLL_DOWN)
		on_zoomout_activate();
	
	return true;
}

void OpenGLImageViewer::setscale(double s) {
	view_update();
	scale = clamp(s, scalemin, scalemax);
	force_update();
}

void OpenGLImageViewer::setshift(float x, float y) {
	view_update();
	sx = clamp(x, -1, 1);
	sy = clamp(y, -1, 1);
	do_update();
}

void OpenGLImageViewer::on_image_realize() {
	glwindow = gtkimage.get_gl_window();
	if (!glwindow || !glwindow->gl_begin(gtkimage.get_gl_context()))
		return;
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	glwindow->gl_end();
}


void OpenGLImageViewer::on_image_configure_event(GdkEventConfigure *event) {
	on_update();
	do_update();
}

void OpenGLImageViewer::on_image_expose_event(GdkEventExpose *event) {
	on_update();
	do_update();
}

void OpenGLImageViewer::on_update() {
	const int depth = gl_img.d;
	
	if(!glwindow || !glwindow->gl_begin(gtkimage.get_gl_context()))
		return;
	
	glTexImage2D(GL_TEXTURE_2D, 0, depth <= 8 ? GL_LUMINANCE8 : GL_LUMINANCE16, gl_img.w, gl_img.h, 0, GL_LUMINANCE, depth <= 8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT, (GLubyte *) gl_img.data);
	
	glwindow->gl_end();
	
	force_update();
}

void OpenGLImageViewer::force_update() {
	do_update();
}

void OpenGLImageViewer::do_update() {
	if(!glwindow || !glwindow->gl_begin(gtkimage.get_gl_context()))
		return;
	
	// Set up GL viewport exactly the size of the GTK area
	int ww = gtkimage.get_width();
	int wh = gtkimage.get_height();
	
	glViewport(0, 0, ww, wh);
	
	// Image scaling and centering
	float cw = gl_img.w;
	float ch = gl_img.h;
	float mwh = min(ww, wh);

	float s;
	
	if (zoomfit) {
		s = min((float)ww / cw, (float)wh / ch);
		scale = log(s)/log(2.0);
		sx = sy = 0;
	} else {
		s = pow(2.0, scale);
	}
	
	glLoadIdentity();
	glScalef((float)s * cw / ww, (float)s * ch / wh, 1);
	//glScalef((float)1, (float)1, 1);
	
	// Render image
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glPushMatrix();
	glTranslatef(sx, sy, 0);
	
	if (fliph)
		glScalef(-1, 1, 1);
	if (flipv)
		glScalef(1, -1, 1);
	
	glEnable(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (s == 1 || s >= 2) ? GL_NEAREST : GL_LINEAR);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 1); glVertex2f(-1, -1);
	glTexCoord2f(1, 1); glVertex2f(+1, -1);
	glTexCoord2f(1, 0); glVertex2f(+1, +1);
	glTexCoord2f(0, 0); glVertex2f(-1, +1);
	glEnd();
	glPopMatrix();
	
	glDisable(GL_TEXTURE_2D);
	
	// Render crosshair
	if (crosshair) {
		glPushMatrix();
		glTranslatef(sx, sy, 0);
		glDisable(GL_LINE_SMOOTH);
		glBegin(GL_LINES);
		glColor3f(0, 1, 0);
		glVertex2f(-1, 0);
		glVertex2f(+1, 0);
		glVertex2f(0, -1);
		glVertex2f(0, +1);
		glEnd();
		glPopMatrix();
	}
	
	// Render pager, 10% size of original window, 5% from the lower-right corner
	if (pager && (s * cw > ww || s * ch > wh)) {
		glPushMatrix();
		// Reset coordinate system
		glLoadIdentity();
		// Scale to approx. 10%
		double mcwh = std::min(cw, ch);
		// Translate to lower-right corner, scale down
		glTranslatef(0.8, -0.8, 0);
		glScalef(0.1 * cw / mcwh / ww * mwh, 0.1 * ch / mcwh / wh * mwh, 1);
		
		// Draw pager bounding box
		glDisable(GL_LINE_SMOOTH);
		glBegin(GL_LINE_LOOP);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f(-1, -1);
		glVertex2f(-1, +1);
		glVertex2f(+1, +1);
		glVertex2f(+1, -1);
		glEnd();
		
		// Draw image contour
		glTranslatef(sx, sy, 0);
		//glScalef((float)s * cw / ww, (float)s * ch / wh, 1);
		glScalef(ww / cw / s, wh / ch / s, 1);
		glBegin(GL_POLYGON);
		glColor3f(0, 1, 1);
		glVertex2f(-1, -1);
		glVertex2f(+1, -1);
		glVertex2f(+1, +1);
		glVertex2f(-1, +1);
		glEnd();
		
		glPopMatrix();
	}
	
	// Swap buffers.
	if (glwindow->is_double_buffered())
		glwindow->swap_buffers();
	else
		glFlush();
	
	glwindow->gl_end();
}
