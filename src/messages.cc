#include <sys/time.h>
#include <time.h>

#include <cstdio>
#include <stdarg.h>
#include <cstring>

#include "messages.h"

using namespace std;

string message::pop_front() {
	size_t b, e = 0;

	b = find_first_not_of(" \t\n", 0);
	if(b == string::npos)
		b = 0;
	if((*this)[b] == ':') {
		++b;
		e = size();
	} else {
		e = find_first_of(" \t\n", b);
		if(e == string::npos)
			e = size();
	}
	if(b == e)
		return "";
	string result(*this, b, e);
	e = find_first_not_of(" \t\n", e);
	if(e != string::npos)
		erase(0, e);

	return result;
}

void message::reply(const string line) {
	if(returnqueue)
		*returnqueue << new message(replyheader + line);
}

messagequeue &messagequeue::operator<<(message *m) {
	pthread::mutexholder h(&mutex);

	queue.push(m);
	cond.signal();

	return *this;
}

messagequeue &messagequeue::operator<<(const string line) {
	pthread::mutexholder h(&mutex);

	queue.push(new message(line));
	cond.signal();

	return *this;
}

message *messagequeue::pop(unsigned int timeout) {
	struct timeval tv;
	struct timespec ts;
	message *m = NULL;
	gettimeofday(&tv, NULL);
	ts.tv_nsec = tv.tv_usec + timeout * 1000;
	ts.tv_sec = tv.tv_sec + ts.tv_nsec / 1000000;
	ts.tv_nsec %= 1000000;
	ts.tv_nsec *= 1000;

	pthread::mutexholder h(&mutex);

	if(!queue.size())
		cond.timedwait(mutex, &ts);

	if(queue.size()) {
		m = queue.front();
		queue.pop();
	}

	return m;
}

messagequeue messagequeue::operator>>(message *&m) {
	pthread::mutexholder h(&mutex);

	while(!queue.size())
		cond.wait(mutex);
	m = queue.front();
	queue.pop();

	return *this;
}

void messagequeue::flush() {
	pthread::mutexholder h(&mutex);

	while(queue.size()) {
		delete queue.front();
		queue.pop();
	}
}
