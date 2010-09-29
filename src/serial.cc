/*
    serial.cc -- serial interface
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>

#include <string>
#include <map>
#include <set>

#include "format.h"
#include "serial.h"
#include "pthread++.h"

using namespace std;

namespace serial {
	static pthread::recursivemutex portmutex;

	static map<string, port *> ports;

	bool port::write(const char *buf, int len) {
		return ::write(fd, buf, len) >= 0;
	}

	bool port::write(string str) {
		return write(str.c_str(), str.size());
	}

	bool port::vprintf(const char *format, va_list va) {
		return write(vformat(format, va));
	}

	bool port::printf(const char *format, ...) {
		bool result;
		va_list va;

		va_start(va, format);
		result = vprintf(format, va);
		va_end(va);

		return result;
	}

	bool port::gets(char *buf, int len, int timeout) {
		int left = len;
		int result;
		char *p = buf;
		char *newline;

		while(left) {
			struct pollfd pfd = {fd, POLLIN | POLLERR | POLLHUP};
			result = poll(&pfd, 1, timeout);

			if(result <= 0)
				return false;

			result = read(fd, p, left);
			if(result <= 0)
				return false;

			newline = (char *)memchr(p, delimiter, result);
			
			if(newline) {
				*newline = 0;
				if(*buf == '\n')
					memmove(buf, buf + 1, strlen(buf));
				return true;
			}
			
			p += result;
			left -= result;
		}

		return false;
	}

	string port::readline(int timeout) {
		char buf[1024];
		if(gets(buf, sizeof buf, timeout))
			return buf;
		else
			return "";
	}

	port &port::operator<<(const string line) {
		write(line);
		return *this;
	}

	port port::operator>>(string &line) {
		line = readline();
		return *this;
	}

	port::port(const string device, speed_t speed, int parity, char delimiter): delimiter(delimiter), device(device), speed(speed), parity(parity) {
		if(device.empty())
			throw exception("No device name!");

		pthread::mutexholder h(&portmutex);

		if(ports[device])
			throw exception(device + ": Port already in use");

		fd = open(device.c_str(), O_RDWR);
		if(fd == -1)
			throw exception(device + ": Error opening serial port: " + strerror(errno));

		struct termios termios;

		tcgetattr(fd, &termios);
		cfmakeraw(&termios);
		termios.c_cflag |= CLOCAL;
		if(parity) {
			termios.c_cflag |= PARENB;
			if(parity & 1)
				termios.c_cflag |= PARODD;
			else
				termios.c_cflag &= ~PARODD;
		} else {
			termios.c_cflag &= ~PARENB;
		}
		cfsetispeed(&termios, speed);
		cfsetospeed(&termios, speed);
		if(tcsetattr(fd, TCSANOW, &termios))
			throw exception(device + ": Error setting serial port parameters: " + strerror(errno));

		ports[device] = this;
	}

	port::~port() {
		pthread::mutexholder h(&portmutex);

		if(!nodes.empty())
			throw exception(device + ": Port still in use");
		
		close(fd);

		ports.erase(device);
	}

	node::node(const string device, int nr, speed_t speed, int parity): nr(nr) {
		pthread::mutexholder h(&portmutex);

		port = ports[device];

		if(!port)
			port = new port::port(device, speed, parity);

		if(port->nodes.find(nr) != port->nodes.end())
			throw exception(device + format(":%d: Node already allocated", nr));

		if(port->speed != speed)
			throw exception(device + format(":%d: Port already uses a different speed than requested", nr));
		if(port->parity != parity)
			throw exception(device + format(":%d: Port already uses a parity than requested", nr));

		port->nodes.insert(nr);
	}

	node::~node() {
		pthread::mutexholder h(&portmutex);

		port->nodes.erase(nr);

		if(port->nodes.empty())
			delete port;
	}

	bool node::printf(const char *format, ...) {
		bool result;
		va_list va;

		va_start(va, format);
		result = port->vprintf(format, va);
		va_end(va);

		return result;
	}
}
