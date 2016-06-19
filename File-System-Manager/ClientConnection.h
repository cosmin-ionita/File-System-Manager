#ifndef CLIENTCONNECTION_H
#define CLIENTCONNECTION_H

#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <netinet/in.h>
#include <netdb.h> 
#include <queue>

#include "TransferFile.h"

using namespace std;

class ClientConnection
{

	public:
		ClientConnection() : _prompt("$ > "), _client_name("") { }

		void initialize_connection(char* ip, char* port);
		void init_connection_parameters(char* ip, char* port);
		void send_message(char* message, int size);
		void select_handler();
		
		void handle_response();
		void handle_stdin();
		void handle_login(string user, string pass);
		void handle_logout();
		void handle_getuserlist();
		void handle_getfilelist(string user);
		void handle_share(string file_name);
		void handle_unshare(string file_name);
		void handle_delete(string file_name);
		void handle_upload(string file_name);
		void handle_download(string user, string file);
		void handle_quit();

		bool exist_file(const char* directory_name, const char* file_name);
		void create_log_file();
		long get_file_size(int fd);

		void set_flag(std::string flag);
		bool get_flag(std::string flag);
		void unset_flags();

		void close_connection();

	private:
		int _client_socket, _max_fd_range;
		struct sockaddr_in _serv_addr;

		bool server_response, stdin_msg, brute_force_flag, exit_flag;

		fd_set _read_fds, _tmp_fds;
		string _last_command, _client_name, _prompt;


		queue<TransferFile> pending_transfers;
};

#endif
