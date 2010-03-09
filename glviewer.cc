/*
 *  glviewer.cc
 *  libsiu
 *
 *  Created by Tim on 20100309.
 *  Copyright 2010 Tim van Werkhoven. All rights reserved.
 *
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

using namespace std;
using namespace Gtk;

static double clamp(double val, double min, double max) {
	return val < min ? min : val > max ? max : val;
}

OpenGLImageViewer::OpenGLImageViewer():
scale(0), scalemin(SCALEMIN), scalemax(SCALEMAX) {
	glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA | Gdk::GL::MODE_DOUBLE);
	if(!glconfig) {
		fprintf(stderr, "Not double-buffered!\n");
		glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA);
	}
	if(!glconfig)
		throw runtime_error("Could not create OpenGL-capable visual");
	
	gtkimage.set_gl_capability(glconfig);
	gtkimage.set_double_buffered(false);
	
	// Init with empty image
	gl_img.d = gl_img.w = gl_img.h = 0;
	gl_img.data = NULL;
		
	// Callbacks
	gtkimage.signal_configure_event().connect_notify(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_configure_event));
	gtkimage.signal_expose_event().connect_notify(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_expose_event));
	gtkimage.signal_realize().connect(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_realize));
	
	// Set zoom & image drag controls
	signal_scroll_event().connect(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_scroll_event));
	signal_button_press_event().connect(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_button_event));
	signal_motion_notify_event().connect(sigc::mem_fun(*this, &OpenGLImageViewer::on_image_motion_event));
	add_events(Gdk::POINTER_MOTION_HINT_MASK);
	
	add(gtkimage);
}

OpenGLImageViewer::~OpenGLImageViewer() {
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

void OpenGLImageViewer::scalestep(double step) {
	scale += step;
	scale = clamp(scale, scalemin, scalemax);
	force_update();
}

bool OpenGLImageViewer::on_image_motion_event(GdkEventMotion *event) {
	double s = pow(2.0, scale);
	sx = clamp(sxstart + 2 * (event->x - xstart) / s / gl_img.w, -1, 1);
	sy = clamp(systart - 2 * (event->y - ystart) / s / gl_img.h, -1, 1);
	do_update();
	return true;
}

bool OpenGLImageViewer::on_image_button_event(GdkEventButton *event) {
	if (event->type == GDK_2BUTTON_PRESS) {
		// Double-click: reset translation
		sx = sy = 0;
		do_update();
	}
	else if (event->type == GDK_3BUTTON_PRESS) {
		// Triple-click: reset translation & zoom
		scale = 0;
		do_update();		
	}
	else {
		// Normal click: remember current translation, use in on_image_motion_event()
		sxstart = sx;
		systart = sy;
		xstart = event->x;
		ystart = event->y;
	}
	return true;
}

void OpenGLImageViewer::on_image_realize() {
	glwindow = gtkimage.get_gl_window();
	if (!glwindow || !glwindow->gl_begin(gtkimage.get_gl_context()))
		return;
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	
	const int depth = gl_img.d;
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	glwindow->gl_end();
}


void OpenGLImageViewer::on_image_configure_event(GdkEventConfigure *event) {
	do_update();
}

void OpenGLImageViewer::on_image_expose_event(GdkEventExpose *event) {
	do_update();
}

void OpenGLImageViewer::on_update() {
	const int depth = gl_img.d;
	
	if(!glwindow || !glwindow->gl_begin(gtkimage.get_gl_context()))
		return;
	{
		glTexImage2D(GL_TEXTURE_2D, 0, depth <= 8 ? GL_LUMINANCE8 : GL_LUMINANCE16, gl_img.w, gl_img.h, 0, GL_LUMINANCE, depth <= 8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT, (GLubyte *) gl_img.data);
	}
	
	glwindow->gl_end();
	
	force_update();
}

void OpenGLImageViewer::force_update() {
	do_update();
}

void OpenGLImageViewer::do_update() {
	if(!glwindow || !glwindow->gl_begin(gtkimage.get_gl_context()))
		return;
	
	// Set up viewport
	
	int ww = gtkimage.get_width();
	int wh = gtkimage.get_height();
	
	glViewport(0, 0, ww, wh);
	
	// Image scaling and centering
	
	int cw = gl_img.w;
	int ch = gl_img.h;
	float s;
	// TODO: implement window-fitting
	//	if(fit.get_active()) {
	//		s = min((float)ww / cw, (float)wh / ch);
	//		scale = log(s)/log(2.0);
	//		sx = sy = 0;
	//	} else {
	s = pow(2.0, scale);
	//	}
	cw *= s;
	ch *= s;
	
	
	glLoadIdentity();
	glScalef((float)cw / ww, (float)ch / wh, 1);
	
	// Render image
	
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glPushMatrix();
	glTranslatef(sx, sy, 0);
	
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
	
	// Swap buffers.
	if (glwindow->is_double_buffered())
		glwindow->swap_buffers();
	else
		glFlush();
	
	glwindow->gl_end();
}
