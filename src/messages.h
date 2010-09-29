#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include "pthread++.h"
#include <string>
#include <queue>

class messagequeue {
	std::queue<class message *> queue;
	pthread::mutex mutex;
	pthread::cond cond;
	
	public:
	messagequeue &operator<<(message *m);
	messagequeue &operator<<(const std::string line);
	messagequeue operator>>(message *&m);
	void flush();
	message *pop(unsigned int timeout);
};

class message: public std::string {
	public:
	class messagequeue *returnqueue;
	std::string replyheader;
	
	message(const std::string line): std::string(line) {}
	void reply(const std::string line);
	std::string pop_front();
	message operator>>(std::string &word) { word = pop_front(); return *this; }
};

#endif
