/*
    serial.h -- Serial port management
    Copyright (C) 2004-2006  Guus Sliepen <guus@sliepen.eu.org>

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

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdarg.h>
#include <termios.h>
#include <unistd.h>

#include <stdexcept>
#include <string>
#include <set>

#include "pthread++.h"

namespace serial {
	class exception: public std::runtime_error {
		public:
		exception(const std::string reason): runtime_error(reason) {}
	};

	class port {
		friend class node;
		int fd;
		std::set<int> nodes;
		char delimiter;

		public:
		pthread::mutex mutex;
		const std::string device;
		const speed_t speed;
		const int parity;
		port(const std::string device, speed_t speed = B9600, int parity = 0, char delimiter = '\r');
		~port();
		
		class mutexholder {
			pthread::mutexholder h;
			public:
			mutexholder(port *p): h(&p->mutex) {}
		};
		
		bool write(const std::string str);
		bool write(const char *buf, int len);
		bool vprintf(const char *format, va_list va);
		bool printf(const char *format, ...);
		bool gets(char *buf, int len, int timeout = -1);
		std::string readline(int timeout = -1);
		port &operator<<(const std::string line);
		port &operator<<(const char *line);
		port operator>>(std::string &line);
	};

	class node {
		serial::port *port;

		public:
		const int nr;
		node(const std::string device, int nr, speed_t speed = B9600, int parity = 0);
		~node();
		
		class mutexholder {
			serial::port::mutexholder h;
			public:
			mutexholder(node *n): h(n->port) {}
		};
		
		bool write(const std::string str) { return port->write(str); }
		bool write(const char *buf, int len = -1) { return port->write(buf, len); }
		bool vprintf(const char *format, va_list va) { return port->vprintf(format, va); }
		bool printf(const char *format, ...);
		bool gets(char *buf, int len, int timeout = -1) { return port->gets(buf, len, timeout); }
		std::string readline(int timeout = -1) { return port->readline(timeout); }
		node &operator<<(const std::string line) { *port << line; return *this; }
		node &operator<<(const char *line) { *port << line; return *this; }
		node operator>>(std::string &line) { *port >> line; return *this; }
	};
}

#endif
