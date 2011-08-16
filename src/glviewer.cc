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

#include "autoconfig.h"

#define DEBUGPRINT(fmt, ...) \
	do { if (LIBSIU_DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
	__LINE__, __func__, __VA_ARGS__); } while (0)

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

const char *vertexprogram =
"void main() {"
"	gl_FrontColor = gl_Color;"
"	gl_TexCoord[0] = gl_MultiTexCoord0;"
"	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"}";

const char *fragmentprogram1 = 
"uniform sampler2D tex;"
"uniform vec4 scale;"
"uniform float bias;"
"uniform float underover;"
"void main() {"
"       vec4 color = texture2D(tex, vec2(gl_TexCoord[0]));"
"	color.gb -= underover * step(0.98, color.r);"
"	color.g += underover * step(color.r, 0.02);"
"	gl_FragColor = (color * scale) + bias;"
"}";

const char *fragmentprogram2 = 
"void main() {"
"	gl_FragColor = gl_Color;"
"}";


#include "glviewer.h"
#include "pthread++.h"
#include "utils.h"

//! @todo Figure out realization / configure / on_update / do_update -> http://www.yorba.org/blog/jim/2010/10/those-realize-map-widget-signals.html

using namespace std;
using namespace Gtk;

OpenGLImageViewer::OpenGLImageViewer():
scale(0), scalemin(SCALEMIN), scalemax(SCALEMAX), minval(0), maxval(-1),
rscale(1), gscale(1), bscale(1),
ngrid(8, 8), grid(false), 
flipv(false), fliph(false), zoomfit(false), crosshair(false),
gl_img()
{
	DEBUGPRINT("%s", "+\n");
	
	glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA | Gdk::GL::MODE_DOUBLE);
	if(!glconfig) {
		fprintf(stderr, "OpenGLImageViewer(): Not double-buffered!\n");
		glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA);
	}
	if(!glconfig)
		throw runtime_error("OpenGLImageViewer(): Could not create OpenGL-capable visual");
	
	gtkimage.set_gl_capability(glconfig);
	gtkimage.set_double_buffered(false);
	
	setshift(0.0);
	setscale(0.0);
	
	// Reserve some space for boxes & lines
	boxes.reserve(128);
	lines.reserve(128);
		
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

// Draw-related functions

void OpenGLImageViewer::on_image_configure_event(GdkEventConfigure *event) {
	DEBUGPRINT("%s", "+\n");
	do_update();
}

void OpenGLImageViewer::on_image_expose_event(GdkEventExpose *event) {
	DEBUGPRINT("%s", "+\n");
	do_update();
}

void OpenGLImageViewer::on_image_realize() {
	DEBUGPRINT("%s", "+\n");
	glwindow = gtkimage.get_gl_window();
	if (!glwindow || !glwindow->gl_begin(gtkimage.get_gl_context()))
		return;
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LINE_SMOOTH);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	// Enable shaders?
	glEnable(GL_FRAGMENT_SHADER);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_SHADER_ARB);
	
	glEnable(GL_VERTEX_SHADER);
	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_VERTEX_SHADER_ARB);
	
	static GLuint vertexshader = 0;
	static GLuint fragmentshader1 = 0;
	static GLuint fragmentshader2 = 0;
	
	// Compile & link shader programs?
	DEBUGPRINT("compiling:\n%s\n", vertexprogram);
	vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, (const GLchar **)&vertexprogram, 0);
	glCompileShader(vertexshader);
	GLint compiled = 0;
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &compiled);
	if(!compiled)
		throw runtime_error("Could not compile vertex shader!");
	
	DEBUGPRINT("compiling:\n%s\n", fragmentprogram1);
	fragmentshader1 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader1, 1, (const GLchar **)&fragmentprogram1, 0);
	glCompileShader(fragmentshader1);
	glGetShaderiv(fragmentshader1, GL_COMPILE_STATUS, &compiled);
	if(!compiled)
		throw runtime_error("Could not compile fragment shader!");
	
	DEBUGPRINT("compiling:\n%s\n", fragmentprogram2);
	fragmentshader2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentshader2, 1, (const GLchar **)&fragmentprogram2, 0);
	glCompileShader(fragmentshader2);
	glGetShaderiv(fragmentshader2, GL_COMPILE_STATUS, &compiled);
	if(!compiled)
		throw runtime_error("Could not compile fragment shader!");
	
	DEBUGPRINT("%s", "linking...\n");
	program1 = glCreateProgram();
	glAttachShader(program1, vertexshader);
	glAttachShader(program1, fragmentshader1);
	glLinkProgram(program1);
	GLint linked = 0;
	glGetProgramiv(program1, GL_LINK_STATUS, &linked);
	if(!linked)
		throw runtime_error("Could not link shader!");
	
	DEBUGPRINT("%s", "linking...\n");
	program2 = glCreateProgram();
	glAttachShader(program2, vertexshader);
	glAttachShader(program2, fragmentshader2);
	glLinkProgram(program2);
	linked = 0;
	glGetProgramiv(program2, GL_LINK_STATUS, &linked);
	if(!linked)
		throw runtime_error("Could not link shader!");

	glwindow->gl_end();
	
	do_full_update();
}

void OpenGLImageViewer::link_data(const void *const data, const int depth, const int w, const int h) {
	DEBUGPRINT("%s", "+\n");
	// Link to new data
	gl_img.d = depth;
	gl_img.w = w;
	gl_img.h = h;
	gl_img.data = data;
	
	// First time: set maxval to the maximum data value
	if (maxval == -1) maxval = (1 << depth) - 1;
	
	// Update frame
	do_full_update();
}

void OpenGLImageViewer::do_full_update() {
	const int depth = gl_img.d;
	DEBUGPRINT("d=%d, min=%g, max=%g\n", depth, minval, maxval);
	
	if(!glwindow || !glwindow->gl_begin(gtkimage.get_gl_context()))
		return;
	
	GLfloat scale = 1 << ((depth <= 8 ? 8 : 16) - depth);
	glPixelTransferf(GL_RED_SCALE, scale);
	glPixelTransferf(GL_GREEN_SCALE, scale);
	glPixelTransferf(GL_BLUE_SCALE, scale);
	
	glTexImage2D(GL_TEXTURE_2D, 0, depth <= 8 ? GL_LUMINANCE8 : GL_LUMINANCE16, gl_img.w, gl_img.h, 0, GL_LUMINANCE, depth <= 8 ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT, (GLubyte *) gl_img.data);
	
	glwindow->gl_end();
	
	do_update();
}

void OpenGLImageViewer::do_update() {
	DEBUGPRINT("%s", "+\n");
	
	if(!glwindow || !glwindow->gl_begin(gtkimage.get_gl_context()))
		return;
	
	// Set up GL viewport exactly the size of the GTK area
	int ww = gtkimage.get_width();
	int wh = gtkimage.get_height();
	
	glViewport(0, 0, ww, wh);
	
	// Image scaling and centering
	double cw = gl_img.w;
	double ch = gl_img.h;
	//double mwh = min(ww, wh);
	
	double s;
	
	if (zoomfit) {
		s = min((double)ww / cw, (double)wh / ch);
		scale = log(s)/log(2.0);
		sx = sy = 0;
	} else {
		s = pow(2.0, scale);
	}
	
	// Load identity, scale, translate, flip
	glLoadIdentity();
	glScalef((double)s * cw / ww, (double)s * ch / wh, 1);
	
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glTranslatef(sx, sy, 0);
	
	if (fliph)
		glScalef(-1, 1, 1);
	if (flipv)
		glScalef(1, -1, 1);
	
	// Render image
	glUseProgram(program1);
	GLint scale = glGetUniformLocation(program1, "scale");
	GLint bias = glGetUniformLocation(program1, "bias");
	GLint uo = glGetUniformLocation(program1, "underover");
//	float min = minval.get_value();
//	float max = maxval.get_value();
	float sc = (1 << gl_img.d) / (maxval - minval);
	glUniform4f(scale, sc * rscale, sc * gscale, sc * bscale, 1.0);
	glUniform1f(bias, -minval / (maxval - minval));
	glUniform1f(uo, 10 * underover);
	glEnable(GL_TEXTURE_2D);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (s == 1 || s >= 2) ? GL_NEAREST : GL_LINEAR);
	
	static const GLt2n3v3f rect[4] = {
		{0, 1, 0, 0, 1, -1, -1, 0},
		{1, 1, 0, 0, 1, +1, -1, 0},
		{0, 0, 0, 0, 1, -1, +1, 0},
		{1, 0, 0, 0, 1, +1, +1, 0},
	};
	glInterleavedArrays(GL_T2F_N3F_V3F, 0, rect);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glPopMatrix();
	
	glUseProgram(program2);
	glDisable(GL_TEXTURE_2D);

#if 0
	glEnable(GL_TEXTURE_2D);
	// Black in case Texture is not loaded yet.
	glColor3f(0, 0, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (s == 1 || s >= 2) ? GL_NEAREST : GL_LINEAR);
	glBegin(GL_POLYGON);
	// Old: flip texture coordinates with respect to OpenGL coordinats.
	//	glTexCoord2f(0, 1); glVertex2f(-1, -1);
	//	glTexCoord2f(1, 1); glVertex2f(+1, -1);
	//	glTexCoord2f(1, 0); glVertex2f(+1, +1);
	//	glTexCoord2f(0, 0); glVertex2f(-1, +1);
	
	// New: Data in same direction as OpenGL (axes increase towards top-right)
	glTexCoord2f(0, 0); glVertex2f(-1, -1);
	glTexCoord2f(1, 0); glVertex2f(+1, -1);
	glTexCoord2f(1, 1); glVertex2f(+1, +1);
	glTexCoord2f(0, 1); glVertex2f(-1, +1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
#endif
	
	// Render crosshair
	if (crosshair) {
		glDisable(GL_LINE_SMOOTH);
		glBegin(GL_LINES);
		glColor3f(0, 1, 0);
		glVertex2f(-1, 0);
		glVertex2f(+1, 0);
		glVertex2f(0, -1);
		glVertex2f(0, +1);
		glEnd();
	}
	
	{
		pthread::mutexholder h(&gui_mutex); 

		// Render boxes (in DATA coordinates, convert to GL!)
		glColor3f(0, 1, 0);
		for (size_t i=0; i<boxes.size(); i++) { 
			glBegin(GL_LINE_LOOP);
			glVertex3f(boxes[i].lx*2.0/cw-1.0, boxes[i].ly*2.0/ch-1.0, 0.0f);
			glVertex3f(boxes[i].tx*2.0/cw-1.0, boxes[i].ly*2.0/ch-1.0, 0.0f);
			glVertex3f(boxes[i].tx*2.0/cw-1.0, boxes[i].ty*2.0/ch-1.0, 0.0f);
			glVertex3f(boxes[i].lx*2.0/cw-1.0, boxes[i].ty*2.0/ch-1.0, 0.0f);
			glEnd();
		}
		
		// Render box labels
		char label[16];
		for (size_t i=0; i<boxes.size(); i++) { 
			glPushMatrix();
			glTranslatef(boxes[i].lx*2.0/cw-1.0, boxes[i].ty*2.0/ch-1.0, 0);
			glScalef(0.0001*ww / ((double)cw), 0.0001*wh/((double)ch), 1.0f);
			
			// Render The Text
			snprintf(label, 16, "%zu", i);
			char *p = label;
			while (*p != '\0') 
				glutStrokeCharacter(GLUT_STROKE_ROMAN, *p++);
			
			glPopMatrix();
			// glRasterPos2f(boxes[i].lx, boxes[i].ty);
			// glutBitmapString(GLUT_STROKE_ROMAN, format("%d", i));
		}
		
		glBegin(GL_LINES);
		// Render lines (in DATA coordinates, convert to GL!)
		for (size_t i=0; i<lines.size(); i++) { 
			glVertex3f(lines[i].lx*2.0/cw-1.0, lines[i].ly*2.0/ch-1.0, 0.0f);
			glVertex3f(lines[i].tx*2.0/cw-1.0, lines[i].ty*2.0/ch-1.0, 0.0f);
		}
		glEnd();
		
	} // end of gui_mutex;
	
	
	// Render pager, 10% size of original window, 5% from the lower-right corner
#if 0
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
		//glScalef((double)s * cw / ww, (double)s * ch / wh, 1);
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
#endif
	
	// Render grid overlay
	if (grid) {
		glDisable(GL_LINE_SMOOTH);
		glBegin(GL_LINES);
		glColor3f(0, 1, 0);
		// Horizontal lines
		for (int n=1; n<ngrid.y; n++) {
			glVertex2f(-1, 2.0*n/ngrid.y - 1);
			glVertex2f(+1, 2.0*n/ngrid.y - 1);
		}
		// Vertical lines
		for (int n=1; n<ngrid.x; n++) {
			glVertex2f(2.0*n/ngrid.x - 1, -1);
			glVertex2f(2.0*n/ngrid.x - 1, +1);
		}
		glEnd();
	}
	
	// Swap buffers.
	if (glwindow->is_double_buffered())
		glwindow->swap_buffers();
	else
		glFlush();
	
	glwindow->gl_end();
}

// Mouse-related events

bool OpenGLImageViewer::on_image_motion_event(GdkEventMotion *event) {
	// Mouse was moved
	if (event->type == GDK_MOTION_NOTIFY && (event->state & GDK_BUTTON1_MASK)) {
		// When the mouse was moved and a mouse key was pressed, drag the image (see on_image_button_event());
		double s = pow(2.0, scale);	
		sx = clamp(sxstart + 2 * (event->x - xstart) / s / gl_img.w, -2.0, 2.0);
		// Use minus for y because OpenGL and GTK coordinate system difference
		sy = clamp(systart - 2 * (event->y - ystart) / s / gl_img.h, -2.0, 2.0);
		do_update();
	}
	
	return false;
}

bool OpenGLImageViewer::on_image_button_event(GdkEventButton *event) {
	// Only works with left mouse button
	if (event->button == 1) {
		if (event->type == GDK_2BUTTON_PRESS) {
			// Double-click: reset translation
			view_update();
			sx = sy = 0;
			do_update();
		}
		else if (event->type == GDK_3BUTTON_PRESS) {
			// Triple-click: reset zoom (3 button implies 2 button)
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
	}
	return false;
}

bool OpenGLImageViewer::on_image_scroll_event(GdkEventScroll *event) {
	if(event->direction == GDK_SCROLL_UP)
		on_zoomin_activate();
	else if(event->direction == GDK_SCROLL_DOWN)
		on_zoomout_activate();
	
	return true;
}

// Other functions
// ==============

int OpenGLImageViewer::map_coord(const float inx, const float iny, float * const outx, float * const outy, const map_dir_t direction) const {
	double tmpx, tmpy;
	int ret = map_coord((double) inx, (double) iny, &tmpx, &tmpy, direction);
	*outx = (float) tmpx;
	*outy = (float) tmpy;
	return ret;
}

int OpenGLImageViewer::map_coord(const double inx, const double iny, double * const outx, double * const outy, const map_dir_t direction) const {
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
	} else if (direction == GTKTODATA) {
		double tmpx, tmpy;
		// First convert to OpenGL (-1 to 1)
		map_coord(inx, iny, &tmpx, &tmpy, GTKTOGL);
		if (fabs(tmpx) > 1.0 || fabs(tmpy) > 1.0)
			return -1;
		// Now multiply with data size
		*outx = (tmpx+1)/2. * gl_img.w;
		*outy = (tmpy+1)/2. * gl_img.h;
		return 0;
	} else if (direction == GLTODATA) {
		// OpenGL runs from (-1, -1) to (1, 1) while the data runs from 
		// (0, 0) to (gl_img.w, gl_img.h)
		*outx = clamp((inx+1.)/2. * gl_img.w, 0.0, (double)gl_img.w);
		*outy = clamp((iny+1.)/2. * gl_img.h, 0.0, (double)gl_img.h);
	} else if (direction == GLTOGTK) {
		// center is at (sx, sy) * s * (gl_img.w, gl_img.h) / 2
		// 1,1 is at (sx, sy) * s * (gl_img.w, gl_img.h) / 2
		return -1;
	} else if (direction == DATATOGL) {
		// OpenGL runs from (-1, -1) to (1, 1) while the data runs from 
		// (0, 0) to (gl_img.w, gl_img.h)
		*outx = inx*2.0/gl_img.w - 1.0;
		*outy = iny*2.0/gl_img.h - 1.0;
	} else if (direction == DATATOGTK) {
		return -1;
	}
	return -1;
}

int OpenGLImageViewer::inbox(const double x, const double y) const {
	// Convert input coordinates (GTK) to box (DATA) coordinates
	double glx, gly;
	map_coord(x, y, &glx, &gly, GTKTODATA);

	// Loop over all boxes, return index when found
	for (size_t i=0; i<boxes.size(); i++) { 
		if (glx >= boxes[i].lx &&
				glx <= boxes[i].tx &&
				gly >= boxes[i].ly &&
				gly <= boxes[i].ty)
			return i;
	}
	// Not found
	return -1;
}

void OpenGLImageViewer::setscale(const double s) {
	view_update();
	scale = clamp(s, scalemin, scalemax);
	do_update();
}

void OpenGLImageViewer::setgrid(const int nx, const int ny) {
	// Overlay a grid of nx by ny cells over the image
	setgrid(true);
	ngrid.x = nx;
	ngrid.y = ny;
	do_update();
}

void OpenGLImageViewer::setshift(const double x, const double y) {
	view_update();
	sx = clamp(x, -1.0, 1.0);
	sy = clamp(y, -1.0, 1.0);
	do_update();
}

