#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

#include <string>

#include "pidfile.h"

using namespace std;

pidfile::pidfile(const string filename): set(false), filename(filename) {
	fd = open(filename.c_str(), O_RDWR | O_CREAT, 0644);
	if(fd == -1)
		throw exception("Cannot open pidfile '" + filename + "': " + strerror(errno));
}

pidfile::~pidfile() {
	if(set) {
		lockf(fd, F_ULOCK, 0);
		unlink(filename.c_str());
		close(fd);
	}
}

pid_t pidfile::getpid() {
	if(!lockf(fd, F_TEST, 0)) {
		set = true;
		return 0;
	}

	char buf[16];
	ssize_t len;
	
	lseek(fd, 0, SEEK_SET);
	len = read(fd, buf, sizeof buf);

	if(len == -1)
		throw exception("Cannot read from pidfile '" + filename + "': " + strerror(errno));

	if(len == 0 || len >= (ssize_t) sizeof buf)
		throw exception("Error parsing pid from '" + filename + "'");

	return atoll(buf);
}

void pidfile::setpid() {
	if(lockf(fd, F_TLOCK, 0) == -1)
		throw exception("Cannot lock pidfile '" + filename + "': " + strerror(errno));

	lseek(fd, 0, SEEK_SET);

	char buf[16];
	int len;
	
	len = snprintf(buf, sizeof buf, "%lli\n", (long long int)::getpid());
	if(len <= 0 || len >= (ssize_t) sizeof buf)
		throw exception("Error formatting pid");
	
	if(write(fd, buf, len) == -1)
		throw exception("Cannot write to pidfile '" + filename + "': " + strerror(errno));

	set = true;
}
