/*
 protocol-thread-test.cc -- Test protocol library with multiple threads
 Copyright (C) 2011 Tim van Werkhoven <werkhoven@strw.leidenuniv.nl>
 
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

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif
#include <unistd.h>
#include <string>
#include <sigc++/signal.h>

#include "pthread++.h"
#include "protocol.h"
#include "socket.h"
#include "pthread++.h"

using namespace std;
typedef Protocol::Server::Connection Connection;

void serv_on_message(Connection *connection, std::string line);
void serv_on_connect(Connection *connection, bool status);

static void on_client_msg(std::string line);

void cli_thread_worker();
void srv_thread_worker1();
void srv_thread_worker2();

const string SRV_PORT = "1234";
const string SRV_NAME1 = "SYS";
const string SRV_NAME2 = "WFS";

// Server threads
const int SRV_NTHR = 2;
pthread::thread srv_thr[SRV_NTHR];
pthread::mutex srv_thr_mutex;
size_t srv_rcvd[SRV_NTHR];

// Client threads
const int CLI_NTHR = 4;
pthread::thread cli_thr[CLI_NTHR];
pthread::mutex cli_thr_mutex;
size_t cli_sent = 0;
size_t cli_rcvd = 0;

int main() {
	fprintf(stdout, "%s: init\n", __FUNCTION__);
	string msg;
	
	// Start two servers
	fprintf(stdout, "%s: creating servers\n", __FUNCTION__);
	srv_thr[0].create(sigc::ptr_fun(srv_thread_worker1));
	srv_thr[1].create(sigc::ptr_fun(srv_thread_worker2));
	srv_rcvd[0] = 0;
	srv_rcvd[1] = 0;
	
	usleep(10 * 1E6);

	// Start worker threads
	fprintf(stdout, "%s: creating clients\n", __FUNCTION__);
	for (int i=0; i<CLI_NTHR; i++)
		cli_thr[i].create(sigc::ptr_fun(cli_thread_worker));
	
	
	fprintf(stdout, "%s: sleeping now...\n", __FUNCTION__);
	usleep(1 * 1E6);

	fprintf(stdout, "%s: stopping.\n", __FUNCTION__);
	fprintf(stdout, "%s: sent: %zu, srv_rcvd: %zu, cli_rcvd: %zu\n", __FUNCTION__, cli_sent, srv_rcvd[0] + srv_rcvd[1], cli_rcvd);
	srv_thr[0].cancel();
	srv_thr[0].join();

	srv_thr[1].cancel();
	srv_thr[1].join();
	
//	fprintf(stdout, "%s: sent: %zu, srv_rcvd: %zu, cli_rcvd: %zu\n", __FUNCTION__, cli_sent, srv_rcvd[0] + srv_rcvd[1], cli_rcvd);
	

	return 0;
}

void serv_on_connect(Connection *connection, bool status) {
	fprintf(stdout, "%s: %s connected: %d\n", __FUNCTION__, connection->server->name.c_str(), status);
}

void serv_on_message(Connection *connection, string line) {
	//fprintf(stdout, "%s: %s message: %s\n", __FUNCTION__, connection->server->name.c_str(), line.c_str());
	//fprintf(stderr, "<- %s\n", connection->server->name.c_str());
	if (connection->server->name == SRV_NAME1)
		(srv_rcvd[0])++;
	else
		(srv_rcvd[1])++;
	
	connection->write("ok :message received "+line);
}

void on_client_msg(std::string line) {
	cli_rcvd++;
	//fprintf(stdout, "%s: received: %s\n", __FUNCTION__, line.c_str());
}

void cli_thread_worker() {
	fprintf(stdout, "%s: start\n", __FUNCTION__);
	
	Protocol::Client client("127.0.0.1", SRV_PORT);
	client.connect();
	client.slot_message = sigc::ptr_fun(on_client_msg);
	
	// Start writing to servers
	while (1) {
		//		fprintf(stderr, "-> %s\n", SRV_NAME1.c_str());
		client.write(SRV_NAME1 + " this is a bogus message, see?");
		cli_sent++;
		//fprintf(stderr, "-> %s\n", SRV_NAME2.c_str());
		client.write(SRV_NAME2 + " this is a bogus message too!");
		cli_sent++;
		usleep(10);
	}
}

void srv_thread_worker1() {
	fprintf(stdout, "%s: start\n", __FUNCTION__);
	Protocol::Server serv1(SRV_PORT, SRV_NAME1);
	
	serv1.slot_message = sigc::ptr_fun(serv_on_message);
	serv1.slot_connected = sigc::ptr_fun(serv_on_connect);
	fprintf(stdout, "%s: listening\n", __FUNCTION__);
	serv1.listen();
	
	while (1)
		sleep(1);
}

void srv_thread_worker2() {
	fprintf(stdout, "%s: start\n", __FUNCTION__);
	Protocol::Server serv2(SRV_PORT, SRV_NAME2);
	
	serv2.slot_message = sigc::ptr_fun(serv_on_message);
	serv2.slot_connected = sigc::ptr_fun(serv_on_connect);
	serv2.listen();
	fprintf(stdout, "%s: listening\n", __FUNCTION__);
	
	while (1)
		sleep(1);
}

