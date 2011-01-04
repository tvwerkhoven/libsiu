#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <string>
#include <map>
#include <set>
#include <sigc++/signal.h>
#include <string>

#include "format.h"
#include "socket.h"
#include "pthread++.h"

namespace Protocol {
	class exception: public std::runtime_error {
		public:
		exception(const std::string reason): runtime_error(reason) {}
	};

	/*!
	 @author Guus Sliepen
	 @brief Client class abstracting sockets.
	*/
	class Client {
		Socket socket;
		bool running; //!< Is the handler running or not
		bool setup_flag; //!< Are the connection settings setup (host, port, name) or not
		
		pthread::attr attr;
		pthread::thread thread;
		
		void handler();
		void setup(const std::string &h, const std::string &p, const std::string &n);

		protected:
		std::string prefix;

		public:
		std::string host; //!< Hostname of target machine
		std::string port; //!< Port on target machine
		std::string name;

		sigc::slot<void, std::string> slot_message; //!< Slot for data handler function
		sigc::slot<void, bool> slot_connected; //!< Slot for on (dis)connection handler function

		Client();
		Client(const std::string &host, const std::string &port, const std::string &name = "");
		~Client();

		void connect();
		void connect(const std::string &h, const std::string &p, const std::string &n);
		void close();
		void disconnect() { close(); }

		bool is_connected();

		void write(const std::string &msg);
		void write(const void *buf, size_t len);
		std::string read();
		bool read(void *buf, size_t len);
		std::string getpeername();
		std::string getsockname();
	};

	class Server {
		class Port;

		public:
		class Connection {
			friend class Server;

			Port *port;
			Socket *socket;
			std::set<std::string> tags;

			bool running;
			pthread::attr attr;
			pthread::thread thread;
			void handler();

			public:
			Server *server;
			const void *data;

			Connection(Port *port, Socket *socket, const void *data = 0);
			~Connection();

			void addtag(const std::string &tag);
			void deltag(const std::string &tag);
			bool hastag(const std::string &tag) const;
			bool hastag(const std::string &tag, const std::string &prefix) const;

			void write(const std::string &msg) const;
			void write(const std::string &msg, const std::string &tag) const;
			void write(const void *buf, size_t len) const;
			std::string read() const;
			bool read(void *buf, const size_t len) const;
			std::string getpeername() const;
			std::string getsockname() const;
			bool is_connected() const;
			void close();
		};

		private:
		class Port {
			friend class Connection;
			friend class Server;

			Socket socket;
			pthread::attr attr;
			pthread::thread thread;
			void handler();

			pthread::mutex mutex;
			std::map<std::string, Server *> users;
			std::set<Connection *> connections;

			static pthread::mutex globalmutex;
			static std::map<std::string, Port *> ports;

			Port(const std::string &port);
			~Port();

			void listen();
			void close();

			public:
			const std::string port;

			static Port *get(Server *server);
			static void release(Server *server);
		} *theport;

		protected:
		std::string prefix;

		public:
		const std::string port;
		const std::string name;

		sigc::slot<void, Connection *, std::string> slot_message;
		sigc::slot<void, Connection *, bool> slot_connected;

		Server(const std::string &port, const std::string &name = "");
		~Server();
		
		void listen();

		void broadcast(const std::string &msg);
		void broadcast(const std::string &msg, const std::string &tag);
		void broadcast(const void *buf, size_t len);
	};
};

#endif
