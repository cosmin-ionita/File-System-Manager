#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 1000 // TODO

class Connection
{
	public:
		Connection() 
			: _server_socket(0)
		{}

		void initialize_connection(char* port);
		void init_connection_parameters(char* port);

	private:
		int _server_socket;
		struct sockaddr_in connection_data;
};


#endif