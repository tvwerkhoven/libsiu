/*
 opengl-test.cc -- test OpenGL with GTK. Based on:
 simple.cc: Simple gtkglextmm example, written by Naofumi Yasufuku  <naofumi@users.sourceforge.net>
 
 Copyright (C) 2010 Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
 This file is part of FOAM.
 
 FOAM is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 2 of the License, or
 (at your option) any later version.
 
 FOAM is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with FOAM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <cstdlib>

#include "autoconfig.h"

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

#ifdef HAVE_GL_GLUT_H
#include "GL/glut.h"
#elif HAVE_GLUT_GLUT_H 
#include "GLUT/glut.h"
#endif


#include <gtkmm.h>
#include <gtkglmm.h>
#include <gdkmm/pixbuf.h>

#include "format.h"
#include "glviewer.h"

using namespace std;
using namespace Gtk;

// Simple viewer class

class Simple: public Gtk::Window {
	Gtk::VBox vbox;
	Gtk::Button render;
	Gtk::Statusbar stbar;
	OpenGLImageViewer glarea;
	
	bool on_image_motion_event(GdkEventMotion *event);
	bool on_image_button_event(GdkEventButton *event);
public:
	void on_render();
	int w, h, d;
	uint8_t *data;
	Simple();
	~Simple();
};


Simple::Simple():
render("Re-render")
{
	fprintf(stderr, "Simple::Simple()\n");
	w = 100; h = 480; d = 8;
	data = (uint8_t *) malloc(w*h*d/8);
	
	on_render();
  glarea.set_size_request(512, 512);
	
	// Add line for demonstration
	glarea.addbox(fvector_t(0, 0, 0.1, 0.1));
	
	for (int i=0; i<10; i++) {
		double x = drand48()*68, y=drand48()*448;
		glarea.addbox(fvector_t(x, y, x+32, y+32), OpenGLImageViewer::DATATOGL);
	}
	
	glarea.addline(fvector_t(0, 0.3, 0.1, 0.4));	

	set_title("OpenGL Window");
	set_gravity(Gdk::GRAVITY_STATIC);

	render.signal_clicked().connect(sigc::mem_fun(*this, &Simple::on_render));
	glarea.signal_motion_notify_event().connect(sigc::mem_fun(*this, &Simple::on_image_motion_event));
	glarea.signal_button_press_event().connect(sigc::mem_fun(*this, &Simple::on_image_button_event));

	vbox.pack_start(glarea);
	vbox.pack_start(render, PACK_SHRINK);
	vbox.pack_start(stbar, PACK_SHRINK);
	add(vbox);
	
	show_all_children();
	
}

Simple::~Simple() {
	free(data);
}

void Simple::on_render() {
	fprintf(stderr, "Simple::on_render()\n");
	// fill data
	for (int i=0; i<h; i++)
		for (int j=0; j<w; j++)
			data[i*w + j] = 255 * 8 * sqrt(pow(i,2) + pow(j,2)) / sqrt(pow(w,2) + pow(h,2));
	
	glarea.linkData((void *) data, d, w, h);
}

bool Simple::on_image_button_event(GdkEventButton *event) {
	fprintf(stderr, "Simple::on_image_button_event()\n");
	if (event->button == 3) {
		// Right-mouse click
		int boxidx = glarea.inbox(event->x, event->y);
		fprintf(stderr, "Simple::on_image_button_event() right mouse in box %d\n", boxidx);
	}
	return false;
}

bool Simple::on_image_motion_event(GdkEventMotion *event) {
	// Get GL coordinates
	double glx, gly;
	if (glarea.map_coord(event->x, event->y, &glx, &gly, OpenGLImageViewer::GTKTOGL))
		glx = gly = -1.0;
	
	// Get data coordinates + values
	double datax, datay;
	double pix;

	if (!glarea.map_coord(event->x, event->y, &datax, &datay, OpenGLImageViewer::GTKTODATA))
		pix = (double) data[w*(int) datay + (int) datax];
	else
		pix = datax = datay = -1.0;
	
	// Get image shift
	float xx, yy;
	glarea.getshift(&xx, &yy);
	
	// Set statusbar text
	stbar.push(	(Glib::ustring) format("Position: (%d, %d), opengl: (%.2f, %.2f), data: (%d, %d), value: %g, offset (%.2f, %.2f)", (int) event->x, (int) event->y, glx, gly, (int) datax, (int) datay, pix, xx, yy));
	return false;
}


int main(int argc, char** argv) {
	fprintf(stderr, "::main()\n");
  Gtk::Main kit(argc, argv);
  Gtk::GL::init(argc, argv);

	glutInit(&argc, argv);
	
  int major, minor;
  Gdk::GL::query_version(major, minor);
  std::cout << "OpenGL extension version - "
	<< major << "." << minor << std::endl;
	
  Simple simple;
	
  kit.run(simple);
	
  return 0;
}

