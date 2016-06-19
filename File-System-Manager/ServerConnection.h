#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <queue>
#include "UserManager.h"
#include "FileSystem.h"
#include "TransferFile.h"

#define MAX_CLIENTS 1000 

using namespace std;

class ServerConnection
{
	public:
		ServerConnection() 
			: _server_socket(0)
		{}

		void accept_handler();
		void select_handler();

		void handle_stdin();
		void handle_client_message(UserManager user_manager, FileSystem &file_system);
		void handle_login(UserManager user_manager, char* credentials);
		void handle_logout(char* client);
		void handle_getuserlist(UserManager user_manager);
		void handle_getfilelist(char* user, FileSystem file_system, UserManager user_manager);

		void handle_share(char* file_info, FileSystem &file_system);
		void handle_unshare(char* file_info, FileSystem &file_system);
		void handle_delete(char* file_info, FileSystem &file_system);
		void handle_upload(char* file_name, FileSystem file_system);
		void handle_download(char* file_name, FileSystem file_system);
		void handle_quit();

		void handle_data_block(char* data, int bytes_received);
		void handle_end_data_block(FileSystem &file_system);

		void send_message(char* message, int size);
		
		void set_flag(std::string flag);
		bool get_flag(std::string flag);
		void unset_flags();
		void init_brute_force();

		bool is_in_transfer(string file_name);

		void initialize_server(char* port);
		void init_connection_parameters(char* port);

		vector<string> split(const string &s, char delim);
		vector<string>& split_helper(const string &s, char delim, vector<string> &elems);
		

	private:
		int _server_socket, _max_fd_range, _current_client;
		
		bool incoming_connection, client_msg, stdin_msg, exit_flag;

		vector<string> _online_clients;
		vector<int> _online_clients_fd;

		struct sockaddr_in 
				_connection_data, 
				_client_conn_data;

		fd_set _read_fds, _tmp_fds;

		queue<TransferFile> pending_transfers;

		vector<string> unfinished_transfers;

		int brute_force[MAX_CLIENTS];
};


#endif
