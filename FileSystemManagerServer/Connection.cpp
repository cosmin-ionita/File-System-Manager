#include <iostream>
#include <cstdlib>
#include <string>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Connection.h"

void is_valid(int connection_object, std::string entity) 
{
	if(connection_object < 0) 
		std::cout << "Failure detected at: " << entity <<"\n";
}

void Connection::init_connection_parameters(char* port)
{
	 connection_data.sin_family = AF_INET;
     connection_data.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
     connection_data.sin_port = htons(atoi(port));
}

void Connection::initialize_connection(char* port)
{
	init_connection_parameters(port);

	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	is_valid(_server_socket, "socket");

	int bind_result = bind(_server_socket, (struct sockaddr *) &connection_data, sizeof(struct sockaddr));
	is_valid(bind_result, "bind");

	int listen_result = listen(_server_socket, MAX_CLIENTS);
	is_valid(listen_result, "listen");
}
