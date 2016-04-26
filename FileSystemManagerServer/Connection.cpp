#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <climits>

#include "Connection.h"

void is_valid(int connection_object, std::string entity) 
{
	if(connection_object < 0) 
		std::cout << "Failure detected at: " << entity <<"\n";
}

void Connection::init_connection_parameters(char* port)
{
	 _connection_data.sin_family = AF_INET;
     _connection_data.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
     _connection_data.sin_port = htons(atoi(port));
}

void Connection::initialize_connection(char* port)
{
	init_connection_parameters(port);

	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	is_valid(_server_socket, "socket");

	int bind_result = bind(_server_socket, (struct sockaddr *) &_connection_data, sizeof(struct sockaddr));
	is_valid(bind_result, "bind");

	int listen_result = listen(_server_socket, MAX_CLIENTS);
	is_valid(listen_result, "listen");

	FD_SET(_server_socket, &_read_fds);
	FD_SET(0, &_read_fds);
	_max_fd_range = _server_socket;
}

void Connection::select_handler() 
{
	_tmp_fds = _read_fds;

	std::cout<<"Command: > "<<std::flush;

	int select_result = select(_max_fd_range + 1, &_tmp_fds, NULL, NULL, NULL);
	is_valid(select_result, "select");

	unset_flags();


	for(int i = 0; i<=_max_fd_range; i++) 
	{
		if(FD_ISSET(i, &_tmp_fds))
		{
			if(i == _server_socket) 
			{
				set_flag("incoming_connection");
			}

			else if(i == 0) 
			{
				set_flag("stdin_msg");
			}

			else  
			{
				set_flag("client_msg");
			}
		}
	}
}

void Connection::accept() 
{
		std::cout<<"It goes here?";
}

void Connection::handle_stdin() 
{
	std::string buffer;

	std::cin>>buffer;

	std::cout<<"You have just typed: "<<buffer<<std::endl;
}

void Connection::unset_flags() 
{
	incoming_connection = false;
	stdin_msg = false;
	client_msg = false;
}

bool Connection::get_flag(std::string flag) 
{
	if(flag == "incoming_connection")
		return incoming_connection;
	else if(flag == "stdin_msg")
		return stdin_msg;
	else
		return client_msg;
}

void Connection::set_flag(std::string flag)
{
	if(flag == "incoming_connection")
		incoming_connection = true;
	else if(flag == "stdin_msg")
		stdin_msg = true;
	else
		client_msg = true;
}
