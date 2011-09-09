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
	void realloc_data();
	template <class T> void fill_data(T *tmpdata);
	double getdata(size_t idx);

public:
	void on_render();
	void add_random();
	void update();
	bool on_timeout();
	
	int iter;
	
	size_t width, height, depth;
	void *data;
	Simple();
	~Simple();
};


Simple::Simple():
render("Re-render"), iter(0), width(100), height(480), depth(8), data(NULL)
{
	fprintf(stderr, "Simple::Simple()\n");
	realloc_data();
	
	on_render();
  glarea.set_size_request(512, 512);
	
	// Add box in lower right corner
	glarea.addbox(fvector_t(0, 0, 3, 3));
	// Add line in this box
	glarea.addline(fvector_t(0, 0, 2, 2));	
	
	// Add 10 random boxes
	for (int i=0; i<10; i++) {
		double x = drand48()*68, y=drand48()*448;
		double width = 16.+drand48()*32;
		double height = 16.+drand48()*32;
		glarea.addbox(fvector_t(x, y, x+width, y+height));
	}
	
	// Add line for demonstration
	glarea.addline(fvector_t(0, 128, 64, 256));	

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
	
	//Glib::signal_timeout().connect(sigc::mem_fun(*this, &Simple::on_timeout), 1000.0/2.0);
}

Simple::~Simple() {
	free(data);
}

void Simple::realloc_data() {
	fprintf(stderr, "Simple::realloc_data() w=%zu, h=%zu, d=%zu\n", width, height, depth);
	free(data);
	data = malloc(width * height * depth/8);
}

double Simple::getdata(size_t idx) {

	if (depth == 8)
		return ((uint8_t *) data)[idx];
	else if (depth == 16)
		return ((uint16_t *) data)[idx];
	else
		return 0;
}

template <class T> void Simple::fill_data(T *tmpdata) {
	int fac = 8;
	if (iter % 2)
		fac = 4;

	int i=10, j=10;
	fprintf(stderr, "Simple::fill_data() pix[10,10] = %zu * %d * (%g / %g)\n", 
					(((size_t) 1) << (sizeof *tmpdata * 8)), fac, sqrt(pow(i,2.0) + pow(j,2.0)), sqrt(pow(width,2.0) + pow(height,2.0)));

	for (size_t i=0; i<height; i++)
		for (size_t j=0; j<width; j++)
			tmpdata[i*width + j] = (((size_t) 1) << (sizeof *tmpdata * 8)) * fac * sqrt(pow(i,2.0) + pow(j,2.0)) / sqrt(pow(width,2.0) + pow(height,2.0));
	size_t maxval = (((size_t) 1) << (sizeof *tmpdata * 8)) -1;
	tmpdata[0] = maxval;
	tmpdata[1*width +1] = 0;
	tmpdata[2*width +2] = maxval;
	tmpdata[3*width +3] = 0;
	tmpdata[4*width +4] = maxval;
	tmpdata[3*width +4] = 0;
	tmpdata[2*width +4] = maxval;
	tmpdata[1*width +4] = 0;
	tmpdata[0*width +4] = maxval;

	fprintf(stderr, "Simple::fill_data() pix[10,10] = %g or %d\n", 
					(((size_t) 1) << (sizeof *tmpdata * 8)) * fac * sqrt(pow(i,2.0) + pow(j,2.0)) / sqrt(pow(width,2.0) + pow(height,2.0)),
					tmpdata[i*width + j]);

}

void Simple::on_render() {
	// Change display settings every iteration to test various settings
	iter++;
	fprintf(stderr, "Simple::on_render() iter=%d\n", iter);
	
	
	if (iter % 3 == 0) {
		glarea.setunderover(true);
//		glarea.setminmax(0, 1 << (depth-1));
	} else {
		glarea.setunderover(false);
//		glarea.setminmax(0, 1 << (depth));		
	}
	
	if (iter % 4 == 0) {
		if (depth == 8) {
			depth = 16;
			width = 110;
			height = 320;
			glarea.setminmax(0, 1 << 16);
		} else if (depth == 16) {
			depth = 8;
			width = 320;
			height = 160;
			glarea.setminmax(0, 1 << 8);
		}
		realloc_data();
	}
	
	// fill data
	if (depth == 8)
		fill_data((uint8_t *) data);
	else if (depth == 16)
		fill_data((uint16_t *) data);
	else
		fprintf(stderr, "Simple::on_render() ERROR data depth\n");
	
	glarea.link_data((void *) data, depth, width, height);
}

void Simple::add_random() {
	int w = drand48() * 100 - 50;
	int h = drand48() * 400 - 200;
	glarea.addline(fvector_t(50, 200, 50+w, 200+h));	
}

void Simple::update() {
	glarea.do_update();
}

bool Simple::on_timeout() {
	fprintf(stderr, "Simple::on_timeout(0)\n");
	add_random();
	sleep(1);

	fprintf(stderr, "Simple::on_timeout(1)\n");
	update();
	sleep(1);
	
	return true;
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
		pix = (double) getdata(width*(int) datay + (int) datax);
	else
		pix = datax = datay = -1.0;
	
	// Get image shift
	double xx, yy;
	glarea.getshift(&xx, &yy);
	
	// Set statusbar text
	stbar.push(	(Glib::ustring) format("Position: (%d, %d), opengl: (%.2f, %.2f), data: (%d, %d), value: %g, offset (%.2f, %.2f)", (int) event->x, (int) event->y, glx, gly, (int) datax, (int) datay, pix, xx, yy));
	return false;
}


int main(int argc, char** argv) {
	fprintf(stderr, "::main()\n");
	fprintf(stderr, "Gtk::Main kit(...)\n");
  Gtk::Main kit(argc, argv);
	fprintf(stderr, "Gtk::GL::init(...)\n");
  Gtk::GL::init(argc, argv);

	fprintf(stderr, "glutInit(...)\n");
	glutInit(&argc, argv);
	
  int major, minor;
  Gdk::GL::query_version(major, minor);
  std::cout << "OpenGL extension version - "
	<< major << "." << minor << std::endl;
	
  Simple simple;
	
	kit.run(simple);
	
	
  return 0;
}

