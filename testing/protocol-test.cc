#include <unistd.h>
#include <string>
#include <sigc++/signal.h>
#include "protocol.h"
#include "socket.h"
#include "pthread++.h"

using namespace std;
typedef Protocol::Server::Connection Connection;

void on_message(Connection *connection, std::string line);
void on_connect(Connection *connection, bool status);

static void on_client_msg(std::string line);

int retval=0;
string n1 = "SYS";
string n2 = "WFS1";

string msg1 = "SYS hello world";
string msg2 = "WFS1 hello sky!";

string resp = "RECEIVE OK";

int main() {
	string msg;
	
	// Start two servers
	Protocol::Server serv1("1234", n1);
	Protocol::Server serv2("1234", n2);

	serv1.slot_message = sigc::ptr_fun(on_message);
	serv1.slot_connected = sigc::ptr_fun(on_connect);
	serv1.listen();

	serv2.slot_message = sigc::ptr_fun(on_message);
	serv2.slot_connected = sigc::ptr_fun(on_connect);
	serv2.listen();
	sleep(1);
	
	Protocol::Client client("127.0.0.1","1234");
	client.connect();
	client.slot_message = sigc::ptr_fun(on_client_msg);
	sleep(1);
	
	fprintf(stderr, "client.write(\"%s\");\n", msg1.c_str());
	client.write(msg1);

	fprintf(stderr, "client.write(\"%s\");\n", msg2.c_str());
	client.write(msg2);
	
	sleep(1);
	
	if (retval == 0)
		fprintf(stderr, "protocol-test.cc SUCCESS!\n");
	else
		fprintf(stderr, "protocol-test.cc FAILED!\n");

	return retval;
}

void on_connect(Connection *connection, bool status) {
	fprintf(stderr, "%s:on_connected: %d\n", connection->server->name.c_str(), status);
}

void on_message(Connection *connection, string line) {
	fprintf(stderr, "%s:on_message: %s\n", connection->server->name.c_str(), line.c_str());

	if ((connection->server->name == n1 && line != msg1) ||
			(connection->server->name == n2 && line != msg2)) {
		fprintf(stderr, "%s:on_message: ERROR!\n", connection->server->name.c_str());
		retval = -1;
	}
	
	string word;
	while ((word = popword(line)).length())
		fprintf(stderr, "%s:on_message: %s\n", connection->server->name.c_str(), word.c_str());
	
	fprintf(stderr, "%s:on_message: writing response\n", connection->server->name.c_str());
	connection->write(resp);
}

void on_client_msg(std::string line) {
	fprintf(stderr, "cli:on_client_msg: %s\n", line.c_str());
	if (line != resp) {
		fprintf(stderr, "cli:on_client_msg: ERROR!\n");
		retval=-1;
	}
}
