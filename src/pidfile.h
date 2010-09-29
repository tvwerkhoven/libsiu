#ifndef PIDFILE_H
#define PIDFILE_H

#include <stdexcept>
#include <string>

class pidfile {
	class exception: public std::runtime_error {
		public:
		exception(const std::string reason): runtime_error(reason) {}
	};

	FILE *file;
	int fd;
	bool set;

	public:
	const std::string filename;
	pidfile(const std::string filename);
	~pidfile();

	pid_t getpid();
	void setpid();
};

#endif
