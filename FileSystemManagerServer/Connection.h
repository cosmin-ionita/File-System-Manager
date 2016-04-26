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

		void accept();
		void select_handler();
		void handle_stdin();
		
		void set_flag(std::string flag);
		bool get_flag(std::string flag);
		void unset_flags();

		void initialize_connection(char* port);
		void init_connection_parameters(char* port);
		

	private:
		int _server_socket, _max_fd_range;
		
		bool incoming_connection, client_msg, stdin_msg;

		struct sockaddr_in _connection_data;

		fd_set _read_fds, _tmp_fds;
};


#endif