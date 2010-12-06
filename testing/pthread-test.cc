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

int nworker = 1;
double work;

pthread::mutex rw;
pthread::mutex mut;
pthread::cond cond;

void sub_worker_prog() {
	fprintf(stderr, "worker(%X) subworker waiting...\n", pthread_self());
	cond.wait(mut);
}

void *worker_prog(void *args) {
	fprintf(stderr, "worker(%X) 1 init, sleeping\n", pthread_self());
	{
		pthread::mutexholder h(&mut);
		sub_worker_prog();
	}

	fprintf(stderr, "worker(%X) 2 awake! waiting for rw.lock()\n", pthread_self());
	{
	pthread::mutexholder h(&rw);
	fprintf(stderr, "worker(%X) 3 got lock! got %g\n", pthread_self(), work);

	usleep(work * 1000000);
	}
	
	fprintf(stderr, "worker(%X) 4 done\n", pthread_self());
	return NULL;
}

void *server_prog(void *args) {
	fprintf(stderr, "server() 1 init\n");
	
	for (int i=0; i<nworker; i++) {
		fprintf(stderr, "server() 2a waiting for rw.lock()\n");
		rw.lock();
		work = drand48();
		fprintf(stderr, "server() 2b got lock, work=%g\n", work);
		rw.unlock();
		fprintf(stderr, "server() 2c broadcast\n");
		cond.signal();
		usleep(0.5 * 1000000);
	}

	fprintf(stderr, "server() 3 done\n");
	return NULL;
}

int main() {
	fprintf(stderr, "main() init nworker=%d\n", nworker);
	
	pthread::thread workers[nworker];
	pthread::thread server;
	
	for (int i=0; i<nworker; i++) {
		fprintf(stderr, "main() start worker=%d\n", i);
		workers[i].create(worker_prog);
	}
	
	fprintf(stderr, "main() start server\n");
	server.create(server_prog);
	
	for (int i=0; i<nworker; i++) {
		fprintf(stderr, "main() join worker=%d\n", i);
		workers[i].join();
	}
	
	usleep(0.5 * 1000000);
	
	fprintf(stderr, "main() end\n");
	return 0;
}