#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <climits>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>

#include "ServerConnection.h"
#include "FileSystem.h"
#include "UserManager.h"

using namespace std;

//____________________SPLIT________________________

vector<string>& ServerConnection::split_helper(const string &s, char delim, vector<string> &elems) 
{
    
    std::stringstream ss(s);
    std::string item;
    
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> ServerConnection::split(const string &s, char delim) 
{
    vector<string> elems;
    split_helper(s, delim, elems);

    return elems;
}

void is_valid(int connection_object, std::string entity) 
{
	if(connection_object < 0) 
		cout << "Failure detected at: " << entity <<"\n";
}

void ServerConnection::init_brute_force()
{
	for(int i = 0; i<MAX_CLIENTS; i++)
	{
		brute_force[i] = 0;
	}
}

void ServerConnection::init_connection_parameters(char* port)
{
	 _connection_data.sin_family = AF_INET;

	 _connection_data.sin_addr.s_addr = INADDR_ANY;

     _connection_data.sin_port = htons(atoi(port));
}

void ServerConnection::initialize_server(char* port)
{
	init_connection_parameters(port);

	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	is_valid(_server_socket, "socket");

	int bind_result = bind(_server_socket, (struct sockaddr *) &_connection_data, sizeof(struct sockaddr));
	is_valid(bind_result, "bind");

	int listen_result = listen(_server_socket, MAX_CLIENTS);
	is_valid(listen_result, "listen");

	FD_ZERO(&_read_fds);
    	FD_ZERO(&_tmp_fds);

	FD_SET(_server_socket, &_read_fds);
	FD_SET(0, &_read_fds);
	_max_fd_range = _server_socket;

	init_brute_force();
}

void ServerConnection::select_handler() 
{

	if(!pending_transfers.empty()) 	
	{
		TransferFile file = pending_transfers.front();

		pending_transfers.pop();

		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 12;			// Bytes bloc code 
        
		int read_result = read(file.read_fd, buffer + 1, 4095);

		if(read_result == 0)	// If I am at the end of the file
		{
			buffer[0] = 14;		//	End block - bytes

			send_message(buffer, 1);

			close(file.read_fd);
		} 
		else
		{
			pending_transfers.push(file);

			send_message(buffer, read_result + 1);
		}
	}

	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = 5000; 	// 5 ms

	_tmp_fds = _read_fds;

	int select_result = 0;	

	if(!pending_transfers.empty())
		select_result = select(_max_fd_range + 1, &_tmp_fds, NULL, NULL, &tv);
	else 
	 	select_result = select(_max_fd_range + 1, &_tmp_fds, NULL, NULL, NULL);
	
	is_valid(select_result, "select");

	unset_flags();

	if(select_result != 0)
	{
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
					_current_client = i;
					set_flag("client_msg");
				}
			}
		}
	}
}

void ServerConnection::accept_handler() 
{
	unsigned int len = sizeof(_client_conn_data);

	int new_sock_fd = accept(_server_socket, (struct sockaddr *)&_client_conn_data, &len);
	is_valid(new_sock_fd, "accept");

	FD_SET(new_sock_fd, &_read_fds);

	if(new_sock_fd > _max_fd_range)
		_max_fd_range = new_sock_fd;
}

void ServerConnection::send_message(char* message, int size)
{
	send(_current_client, message , size, 0);
}

void ServerConnection::handle_login(UserManager user_manager, char* credentials)
{
	vector<string> tokens = split(credentials, ' ');


	if(user_manager.check_credentials(tokens[0], tokens[1])) 
	{
		_online_clients.push_back(tokens[0]);	// Add the client on the current session
		_online_clients_fd.push_back(_current_client);

		brute_force[_current_client] = 0;		// Update the "brute force" vector

		char message[1];						// Send success
		message[0] = 11; 

		send_message(message, 1);
	}
	else
	{
		char message[1];
		
		if(brute_force[_current_client] == 2)	// Brute force detected
		{
			FD_CLR(_current_client, &_read_fds);	// Close the connection

			message[0] = 13;					// Brute force - code
			send_message(message, 1);
		}
		else 
		{
			message[0] = 10; 					// Incorrect data

			brute_force[_current_client]++;		// Possible to try brute - force

			send_message(message, 1);
		}
	}
}

void ServerConnection::handle_logout(char* user_name)
{
	string name(user_name);

	int i = 0;

	vector<string>::iterator it;

	for(it = _online_clients.begin(); it != _online_clients.end(); it++)
	{
		if((*it) == name)	// Searching the client in the connected clients list
		{
			break;
		}
		i++;
	}
							// Delete the client from the connected clients list
	_online_clients.erase(_online_clients.begin() + i);
	_online_clients_fd.erase(_online_clients_fd.begin() + i);
}

void ServerConnection::handle_getuserlist(UserManager user_manager)
{
	char buffer[4096];
	memset(buffer, 0, 4096);

	buffer[0] = 11;			//	Sucess
	
	strcpy(buffer + 1, user_manager.get_user_list());

	send_message(buffer, strlen(buffer));
}

void ServerConnection::handle_getfilelist(char* user, FileSystem file_system, UserManager user_manager)
{

	char userName[100];

	memset(userName, 0, 100);

	int lung = user[0] - 'A';			// Extract the user name length

	memcpy(userName, user + 1, lung);	// Extract the user name

	string user_name(userName);			// Convert to string

	if(user_manager.exist_user(user_name))
	{
		char buffer[4096];
		memset(buffer, 0, 4096);

		strcpy(buffer, file_system.get_files_info(user_name));

		send_message(buffer, strlen(buffer));
	}
	else
	{
		char buffer[4096];

		buffer[0] = 10;					// Error code 

		strcpy(buffer + 1, "-11 The user does not exist\n");

		send_message(buffer, strlen(buffer));
	}
}

void ServerConnection::handle_share(char* file_info, FileSystem &file_system)
{
	
	string info(file_info);

	vector<string> tokens = split(info, ' ');

	if(file_system.exist_file(tokens[0].c_str(), tokens[1].c_str())) 
	{
		file_system.share(tokens[0], tokens[1]);

		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 11;			// Success

		send_message(buffer, 1);
	}
	else
	{
		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 10;			// Error

		memcpy(buffer + 1, "-4 The file does not exist\n", 22);

		send_message(buffer, strlen(buffer + 1) + 1);
	}
}

void ServerConnection::handle_unshare(char* file_info, FileSystem &file_system)
{
	string info(file_info);

	vector<string> tokens = split(info, ' ');

	if(file_system.exist_file(tokens[0].c_str(), tokens[1].c_str())) 
	{
		file_system.unshare(tokens[0], tokens[1]);

		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 11;			// Success
		send_message(buffer, 1);
	}
	else
	{
		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 10;			// Error

		memcpy(buffer + 1, "-4 The file does not exist", 20);

		send_message(buffer, strlen(buffer + 1) + 1);
	}
}

bool ServerConnection::is_in_transfer(string file_name)
{
	vector<string>::iterator it;

	for(it = unfinished_transfers.begin(); it!= unfinished_transfers.end(); it++)
	{
		if((*it).compare(file_name) == 0)
			return true;
	}

	return false;
}

void ServerConnection::handle_delete(char* file_info, FileSystem &file_system)
{
	string info(file_info);

	vector<string> tokens = split(info, ' ');

	if(is_in_transfer(tokens[1]))
	{
		char buffer[50];
		memset(buffer, 0, 50);

		buffer[0] = 10;				// Error

		strcpy(buffer + 1, "-10 The file is in a pending transfer\n");

		send_message(buffer, 33);
	}
	else if(file_system.exist_file(tokens[0].c_str(), tokens[1].c_str())) 
	{
		file_system.delete_file(tokens[0], tokens[1]);

		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 11;				// Success
		send_message(buffer, 1);
	}
	else
	{
		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 10;				// Eroare

		memcpy(buffer + 1, "-4 The file does not exist\n", 22);

		send_message(buffer, strlen(buffer + 1) + 1);
	}
}

void ServerConnection::handle_upload(char* file_name, FileSystem file_system)
{
	string file_info(file_name);

	vector<string> tokens  = split(file_info, ' ');


	if(file_system.exist_file(tokens[0].c_str(), tokens[1].c_str()))
	{
		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 10;					//Error, the file exists on the server
		send_message(buffer, 1);
	}
	else
	{
		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 11;					// Success

		TransferFile file;				// The transfer - object

		file.name = tokens[1];			// Set the parameters of the transfer - object
		file.op_type = 0;		
		file.dest_socket = _current_client;

		string path = "";

		path += tokens[0];
		path += "/";
		path += tokens[1];

		int fd = creat(path.c_str(), S_IRUSR | S_IXUSR);	// Creating the file

		file.write_fd = fd;

		pending_transfers.push(file);				// Add the object in the transfer queue
		unfinished_transfers.push_back(tokens[1]);

		memcpy(buffer + 1, tokens[1].c_str(), tokens[1].size());		
		send_message(buffer, tokens[1].size() + 1);
	}
}
									
void ServerConnection::handle_data_block(char* data, int bytes_received)	// Scrie in fisier un bloc de bytes
{
	TransferFile file = (TransferFile)pending_transfers.front();
	pending_transfers.pop();

	pending_transfers.push(file);
	write(file.write_fd, data, bytes_received - 1);
}

void ServerConnection::handle_end_data_block(FileSystem &file_system)		// Inchide un fisier transferat
{
	TransferFile file = (TransferFile)pending_transfers.front();
	pending_transfers.pop();

	string user_name;

	int i = 0;
	vector<int>::iterator it;

	for(it = _online_clients_fd.begin(); it != _online_clients_fd.end(); it++)
	{
		if((*it) == _current_client) 
		{
			user_name = _online_clients[i];
			break;
		}

		i++;
	}

	i = 0;
	vector<string>::iterator it_name;

	for(it_name = unfinished_transfers.begin(); it_name != unfinished_transfers.end(); it_name++)
	{
		if((*it_name) == file.name)
		{	
			unfinished_transfers.erase(unfinished_transfers.begin()  + i);
			break;
		}

		i++;
	}
	
	file_system.add_file(user_name, file.name);

	close(file.write_fd);
}

void ServerConnection::handle_quit()
{

}

void ServerConnection::handle_download(char* args, FileSystem file_system)
{
	string file_info(args);

	vector<string> tokens  = split(file_info, ' ');

	if(file_system.exist_file(tokens[0].c_str(), tokens[1].c_str()))
	{
		if(file_system.is_shared(tokens[0], tokens[1]))
		{

			char buffer[4096];
			memset(buffer, 0, 4096);

			
			TransferFile file;				

			file.name = tokens[1];			
			file.op_type = 1;				// upload 
			file.dest_socket = _current_client;

			string path = "";

			path += tokens[0];
			path += "/";
			path += tokens[1];

			int fd = open(path.c_str(), O_RDONLY);	// Open the file

			file.read_fd = fd;

			pending_transfers.push(file);				// Add the object in the transfer queue
			unfinished_transfers.push_back(tokens[1]);

			buffer[0] = 11;					// Success
			memcpy(buffer + 1, tokens[1].c_str(), tokens[1].size());	

			send_message(buffer, tokens[1].size() + 2);
		}
		else
		{		
			char buffer[4096];
			memset(buffer, 0, 4096);

			buffer[0] = 10;			// Error

			memcpy(buffer + 1, "-5 Denied download\n", 26);

			send_message(buffer, strlen(buffer + 1) + 1);
		}
	}
	else
	{
		char buffer[4096];
		memset(buffer, 0, 4096);

		buffer[0] = 10;			// Err

		memcpy(buffer + 1, "-4 The file does not exist\n", 22);

		send_message(buffer, strlen(buffer + 1) + 1);
	}

}

void ServerConnection::handle_client_message(UserManager user_manager, FileSystem &file_system)
{
	char buffer[4096];

	memset(buffer, 0, 4096);


	int bytes_received = recv(_current_client, buffer, 4096, 0);

	if(buffer[0] == 1) 			// login
	{
		handle_login(user_manager, buffer + 1);
	}
	else if(buffer[0] == 2) 	// logout
	{
		handle_logout(buffer + 1);
	}
	else if(buffer[0] == 3)		//getfilelist
	{
		handle_getfilelist(buffer + 1, file_system, user_manager);
	}
	else if(buffer[0] == 4) 	// getuserlist
	{
		handle_getuserlist(user_manager);
	}
	else if(buffer[0] == 5)		// upload
	{
		handle_upload(buffer + 1, file_system);
	}
	else if(buffer[0] == 6)		// download
	{
		handle_download(buffer + 1, file_system);
	}
	else if(buffer[0] == 7)		// share
	{
		handle_share(buffer + 1, file_system);
	}
	else if(buffer[0] == 8) 	// unshare
	{
		handle_unshare(buffer + 1, file_system);
	}
	else if(buffer[0] == 9)		// delete
	{
		handle_delete(buffer + 1, file_system);
	}
	else if(buffer[0] == 12) 	// data_block_received
	{
		handle_data_block(buffer + 1, bytes_received);
	}
	else if(buffer[0] == 14) 	// end data_block
	{
		handle_end_data_block(file_system);
	}
	else if(buffer[0] == 15)	// quit
	{
		handle_quit();
	}
}

void ServerConnection::handle_stdin() 
{
	std::string buffer;

	std::cin>>buffer;

	if(buffer.compare("quit") == 0)
	{
		set_flag("exit");
	}
}

void ServerConnection::unset_flags() 
{
	incoming_connection = false;
	stdin_msg = false;
	client_msg = false;
	exit_flag = false;
}

bool ServerConnection::get_flag(std::string flag) 
{
	if(flag == "incoming_connection")
		return incoming_connection;
	else if(flag == "stdin_msg")
		return stdin_msg;
	else if(flag == "exit")
		return exit_flag;
	else
		return client_msg;
}

void ServerConnection::set_flag(std::string flag)
{
	if(flag == "incoming_connection")
		incoming_connection = true;
	else if(flag == "stdin_msg")
		stdin_msg = true;
	else if(flag == "exit")
		exit_flag = true;
	else 
		client_msg = true;
}
