/*
 pthread-test.cc -- Test ptherad++ library
 Copyright (C) 2010 Tim van Werkhoven <t.i.m.vanwerkhoven@xs4all.nl>
 
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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pthread++.h"

int nworker = 4;
double work;
int sum = 0;

pthread::mutex rw;
pthread::mutex mut;
pthread::cond cond;

void sub_worker_prog() {
	pthread_t pt = pthread_self();
	unsigned char *ptc = (unsigned char*)(void*)(&pt);
	char thrid[2+2*sizeof(pt)+1];
	sprintf(thrid, "0x");
	for (size_t i=0; i<sizeof(pt); i++)
		sprintf(thrid, "%s%02x", thrid, (unsigned)(ptc[i]));

	fprintf(stderr, "pthread-test.cc::worker(%s) subworker waiting...\n", thrid);
	
	cond.wait(mut);
}

void *worker_prog(void */*args*/) {
	pthread_t pt = pthread_self();
	unsigned char *ptc = (unsigned char*)(void*)(&pt);
	char thrid[2+2*sizeof(pt)+1];
	sprintf(thrid, "0x");
	for (size_t i=0; i<sizeof(pt); i++)
		sprintf(thrid, "%s%02x", thrid, (unsigned)(ptc[i]));

	fprintf(stderr, "pthread-test.cc::worker(%s) 1 init, sleeping\n", thrid);

	{
		pthread::mutexholder h(&mut);
		sub_worker_prog();
	}

	fprintf(stderr, "pthread-test.cc::worker(%s) 2 awake! waiting for rw.lock()\n", thrid);

	{
		pthread::mutexholder h(&rw);
		fprintf(stderr, "pthread-test.cc::worker(%s) 3 got lock! got %g\n", thrid, work);
		sum++;
		usleep(work * 1000000);
	}
	
	fprintf(stderr, "pthread-test.cc::worker(%s) 4 done\n", thrid);
	return NULL;
}

void *server_prog(void */*args*/) {
	fprintf(stderr, "pthread-test.cc::server() 1 init\n");
	
	for (int i=0; i<nworker; i++) {
		fprintf(stderr, "pthread-test.cc::server() 2a waiting for rw.lock()\n");
		rw.lock();
		work = drand48();
		fprintf(stderr, "pthread-test.cc::server() 2b got lock, work=%g\n", work);
		rw.unlock();
		fprintf(stderr, "pthread-test.cc::server() 2c broadcast\n");
		cond.signal();
		usleep(0.5 * 1000000);
	}

	fprintf(stderr, "pthread-test.cc::server() 3 done\n");
	return NULL;
}

int main() {
	fprintf(stderr, "pthread-test.cc::main() init nworker=%d\n", nworker);
	
	pthread::thread workers[nworker];
	pthread::thread server;
	
	for (int i=0; i<nworker; i++) {
		fprintf(stderr, "pthread-test.cc::main() start worker=%d\n", i);
		workers[i].create(worker_prog);
	}
	
	fprintf(stderr, "pthread-test.cc::main() start server\n");
	server.create(server_prog);
	
	for (int i=0; i<nworker; i++) {
		fprintf(stderr, "pthread-test.cc::main() join worker=%d\n", i);
		workers[i].join();
	}
	
	usleep(0.5 * 1000000);
	
	if (sum != nworker) {
		fprintf(stderr, "pthread-test.cc::main() error!\n");
		return -1;
	}
	
	fprintf(stderr, "pthread-test.cc::main() success!\n");
	return 0;
}