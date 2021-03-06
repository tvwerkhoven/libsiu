/*
    Socket.h -- Socket creation
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

#ifndef HAVE_SOCKET_H
#define HAVE_SOCKET_H

#include <stdarg.h>
#include <stdexcept>
#include <string>
#include <format.h>

#include <sys/socket.h>
#include <netdb.h>

#define MAXBUFLEN 4096

class Socket {
	int fd;
	size_t inlen;
	char inbuf[MAXBUFLEN];

	Socket(const int fd);

public:
	Socket();
	Socket(const std::string &port);
	Socket(const std::string &host, const std::string &port);
	~Socket();
	bool setblocking(bool blocking = true);
	void listen(const std::string &port);
	bool connect(const std::string &host, const std::string &port);
	Socket *accept() const;
	void close();
	bool gets(char *buf, const size_t len);
	bool write(const void *buf, const size_t len);
	bool write(const std::string str);
	bool read(void *buf, const size_t len);
	bool vprintf(const char *format, va_list va);
	bool printf(const char *format, ...);
	std::string readline();
	bool readline(std::string &line);
	bool readavailable() const ;
	bool writeavailable() const ;
	Socket &operator<<(const std::string line);
	Socket &operator<<(const char *line);
	Socket operator>>(std::string &line);
	bool is_connected() const;
	static std::string resolve(struct sockaddr *addr, socklen_t addrlen, int flags = NI_NUMERICHOST | NI_NUMERICSERV);
	std::string getpeername() const;
	std::string getsockname() const;

	class exception: public std::runtime_error {
		public:
		exception(const std::string reason): runtime_error(reason) {}
	};
};

#endif // HAVE_SOCKET_H
