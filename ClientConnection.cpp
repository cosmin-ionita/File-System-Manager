#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <dirent.h>
#include <sys/time.h>

#include "ClientConnection.h"
#include "Message.h"

#include "TransferFile.h"

using namespace std;


ofstream log;				// File descriptorul fisierului de log
bool exit_token = false;		// Token pentru comanda quit

//____________________SPLIT________________________

std::vector<std::string> &split_helper(const std::string &s, char delim, std::vector<std::string> &elems) 
{
    
    std::stringstream ss(s);
    std::string item;
    
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> split(const string &s, char delim) 
{
    vector<string> elems;
    split_helper(s, delim, elems);

    return elems;
}

void is_valid(int connection_object, std::string entity) 
{
	if(connection_object < 0) 
		std::cout << "Failure detected at: " << entity <<"\n";
}

void ClientConnection::send_message(char* message, int size)
{
	send(_client_socket, message , size, 0);
}

long ClientConnection::get_file_size(int fd)
{
    struct stat stat_buf;
    int rc = fstat(fd, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

void ClientConnection::handle_response()
{
	char buffer[4096];
	memset(buffer, 0, 4096);

	int bytes_received = recv(_client_socket, buffer, 4096, 0);

	if(_last_command == "login")
	{
		if(buffer[0] == 11)	// success
		{
			_prompt = _client_name;
			_prompt += " > ";

			cout<<_prompt<<flush;
			log<<_prompt<<flush;
		}
		else if(buffer[0] == 10) // error
		{
			_client_name = "";

			cout<<"-3 User / parola gresita"<<endl<<flush;
			cout<<_prompt<<flush;

			log<<"-3 User / parola gresita"<<endl<<flush;
			log<<_prompt<<flush;
		}
		else if(buffer[0] == 13) // brute-force code
		{
			cout<<"-8 Brute-force detectat"<<endl<<flush;
			log<<"-8 Brute-force detectat"<<endl<<flush;

			set_flag("bruteforce");
		}
	}

	else if(_last_command == "getuserlist")
	{
		cout<<buffer + 	1<<flush;
		cout<<_prompt<<flush;

		log<<buffer + 	1<<flush;
		log<<_prompt<<flush;
	}
	else if(_last_command == "getfilelist")
	{
		cout<<buffer<<flush;
		cout<<_prompt<<flush;

		log<<buffer<<flush;
		log<<_prompt<<flush;		
	}
	else if(_last_command == "share")
	{
		cout<<buffer + 	1<<flush;
		cout<<_prompt<<flush;

		log<<buffer + 	1<<flush;
		log<<_prompt<<flush;
	}
	else if(_last_command == "unshare")
	{
		cout<<buffer + 	1<<flush;
		cout<<_prompt<<flush;

		log<<buffer + 	1<<flush;
		log<<_prompt<<flush;
	}
	else if(_last_command == "delete")
	{
		if(buffer[0] != 11)
		{
			cout<<buffer + 	1<<flush;
			cout<<_prompt<<flush;
		}
	}
	else if(_last_command == "upload")
	{
		if(buffer[0] == 11)		// serverul trimite inapoi 11nume_fisier
		{
			TransferFile file;	// Obiectul aferent transferului in curs

			int fd = open(buffer + 1, O_RDONLY);

			file.name = buffer + 1;
			file.dest_socket = _client_socket;
			file.read_fd = fd;
			file.op_type = 1;	
			file.dimension =  get_file_size(fd);

			pending_transfers.push(file);

			cout<<_prompt<<flush;

			log<<_prompt<<flush;
		}
		else 
		{
			cout<<"-9 Fisier existent pe server"<<endl<<flush;
			cout<<_prompt<<flush;

			log<<"-9 Fisier existent pe server"<<endl<<flush;
			log<<_prompt<<flush;
		}
	}
	else if(_last_command == "download")
	{

		if(buffer[0] == 11)		// serverul trimite inapoi 11nume_fisier
		{
			TransferFile file;	// Obiectul aferent transferului in curs

			int fd = creat(buffer + 1, O_WRONLY);

			file.name = buffer + 1;
			file.dest_socket = _client_socket;
			file.write_fd = fd;
			file.op_type = 2;		// download

			pending_transfers.push(file);

			cout<<_prompt<<flush;

			log<<_prompt<<flush;
		}
		else 
		{
			//cout<<buffer + 1<<flush;
			//cout<<_prompt<<flush;

			log<<buffer + 1<<flush;
			log<<_prompt<<flush;
		}
	}
}

void ClientConnection::handle_login(string user, string pass)
{
	
	if(_client_name == "")	// Daca nu sunt logat
	{
		int size = user.size() + pass.size() + 1;

		char* content = (char*)malloc(size * sizeof(char));

		content[0] = 1;			// Cod de login

		strcpy(content + 1, user.c_str());
		strcat(content, " ");
		strcat(content, pass.c_str());
		strcat(content, "\0");


		_last_command = "login";
		_client_name = user;

		send_message(content, size + 1);
	}
	else 
	{
		cout<<"-2 Sesiune deja deschisa"<<endl<<flush;
		cout<<_prompt<<flush;
	}
}

void ClientConnection::handle_logout()
{
							// Trimite semnal la server sa inchida sesiunea
	if(_client_name != "") 
	{
		char message[4096];
		memset(message, 0, 4096);

		message[0] = 2;		// Cod de logout

		memcpy(message + 1, _client_name.c_str(), strlen(_client_name.c_str()));

		send_message(message, _client_name.size() + 1);

		_client_name = "";
		_prompt = "$ > ";

		cout<<_prompt<<flush;
		log<<_prompt;
	}
	else 
	{
		cout<<"-1 Client neautentificat"<<endl<<flush;
		cout<<_prompt<<flush;

		log<<"-1 Client neautentificat"<<endl<<flush;
		log<<_prompt<<flush;		
	}
}

void ClientConnection::handle_getuserlist()
{
	if(_client_name != "") 
	{
		char message[1];

		memset(message, 0, 1);
		message[0] = 4;		 // Cod de getuserlist

		_last_command = "getuserlist";
		send_message(message, 1);
	}
	else 
	{
		cout<<"-1 Client neautentificat"<<endl<<flush;
		cout<<_prompt<<flush;	

		log<<"-1 Client neautentificat"<<endl<<flush;
		log<<_prompt<<flush;	
	}
}

void ClientConnection::handle_getfilelist(string user)
{
	if(_client_name != "") 
	{
		char message[user.size() + 3];

		memset(message, 0, user.size() + 3);

		message[0] = 3;
		message[1] = 'A' + user.size();

		memcpy(message + 2, user.c_str(), user.size());
		
		_last_command = "getfilelist";

		send_message(message, user.size() + 2);
	}
	else 
	{
		cout<<"-1 Client neautentificat"<<endl<<flush;
		cout<<_prompt<<flush;	

		log<<"-1 Client neautentificat"<<endl<<flush;
		log<<_prompt<<flush;	
	}
}

void ClientConnection::handle_share(string file_name)
{
	if(_client_name != "")
	{
		char message[4096];
		message[0] = 7;				// Cod de share

		string full_message = "";

		full_message += _client_name;	// Adaug numele clientului in mesaj pentru identificare pe server
		full_message += " ";
		full_message += file_name;

		memcpy(message + 1, full_message.c_str(), full_message.size());

		_last_command = "share";
		send_message(message, full_message.size() + 1);
	}
	else 
	{
		cout<<"-1 Client neautentificat"<<endl<<flush;
		cout<<_prompt<<flush;

		log<<"-1 Client neautentificat"<<endl<<flush;
		log<<_prompt<<flush;	
	}
}

void ClientConnection::handle_unshare(string file_name)
{
	if(_client_name != "")
	{
		char message[4096];
		message[0] = 8;			//Cod de unshare

		string full_message = "";

		full_message += _client_name;	// Adaug numele clientului in mesaj pentru identificare pe server
		full_message += " ";
		full_message += file_name;

		memcpy(message + 1, full_message.c_str(), full_message.size());

		_last_command = "unshare";
		send_message(message, full_message.size() + 1);
	}
	else 
	{
		cout<<"-1 Client neautentificat"<<endl<<flush;
		cout<<_prompt<<flush;

		log<<"-1 Client neautentificat"<<endl<<flush;
		log<<_prompt<<flush;		
	}
}

void ClientConnection::handle_delete(string file_name)
{
	if(_client_name != "")
	{
		char message[4096];

		message[0] = 9;			// Cod de delete

		string full_message = "";

		full_message += _client_name;	// Adaug numele clientului in mesaj pentru identificare pe server
		full_message += " ";
		full_message += file_name;

		memcpy(message + 1, full_message.c_str(), full_message.size());
		_last_command = "delete";

		send_message(message, full_message.size() + 1);
	}
	else 
	{
		cout<<"-1 Client neautentificat"<<endl<<flush;
		cout<<_prompt<<flush;

		log<<"-1 Client neautentificat"<<endl<<flush;
		log<<_prompt<<flush;	
	}
}

bool ClientConnection::exist_file(const char* directory_name, const char* file_name)
{
	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir (directory_name)) != NULL) 
	{
		  	/* print all the files and directories within directory */
		  	while ((ent = readdir (dir)) != NULL) 
		  	{
		    	 if(strcmp(ent->d_name, file_name) == 0)
		    	 	return true;
		 	}
		  	
		  	closedir (dir);
	} 
	else 
	{
	   //cout<<"Directorul nu poate fi deschis"<<endl;
	   return false;
	}

	//cout<<"Fisierul "<<file_name<<" nu exista"<<endl;
	return false;
}

void ClientConnection::handle_upload(string file_name)
{
	if(exist_file("./", file_name.c_str()))
	{
		char message[4096];

		message[0] = 5;		// Cod de upload

		string full_message = "";

		full_message += _client_name;
		full_message += " ";
		full_message += file_name;
		
		_last_command = "upload";

		memcpy(message + 1, full_message.c_str(), full_message.size());

		send_message(message, full_message.size() + 1);
	}
	else
	{
		cout<<"-4 Fisier inexistent"<<endl<<flush;
		cout<<_prompt<<flush;

		log<<"-4 Fisier inexistent"<<endl<<flush;
		log<<_prompt<<flush;
	}
}

void ClientConnection::handle_quit()
{
	log.close();
	exit_token = true;

	char buffer[1];
	buffer[0] = 15;		//quit

	send_message(buffer, 1);
}

void ClientConnection::handle_download(string user, string file)
{
	int size = user.size() + file.size() + 1;

	char* content = (char*)malloc(size * sizeof(char));

	content[0] = 6;			// Cod de download

	strcpy(content + 1, user.c_str());
	strcat(content, " ");
	strcat(content, file.c_str());
	strcat(content, "\0");

	_last_command = "download";

	send_message(content, size + 1);
}

void ClientConnection::handle_stdin()
{
	string buffer;

	getline(cin, buffer);
	log<<buffer<<endl;

	if(buffer == "")
	{
		cout<<"Comanda invalida"<<endl<<flush;
		log<<"Comanda invalida"<<endl<<flush;

		cout<<_prompt<<flush;
		log<<_prompt<<flush;
	} 
	else 
	{
		vector<string> tokens = split(buffer, ' ');

		if(tokens[0] == "login")
				handle_login(tokens[1], tokens[2]);

		else if(tokens[0] == "logout")
				handle_logout();

		else if(tokens[0] == "getfilelist")
				handle_getfilelist(tokens[1]);

		else if(tokens[0] == "getuserlist")
				handle_getuserlist();

		else if(tokens[0] == "upload")
				handle_upload(tokens[1]);
			
		else if(tokens[0] == "download")
				handle_download(tokens[1], tokens[2]);

		else if(tokens[0] == "share")
				handle_share(tokens[1]);
			
		else if(tokens[0] == "unshare")
				handle_unshare(tokens[1]);

		else if(tokens[0] == "delete")
				handle_delete(tokens[1]);
	
		else if(tokens[0] == "quit")
				handle_quit();
		else 
		{
			cout<<"Comanda invalida"<<endl<<flush;
			cout<<_prompt<<flush;

			log<<"Comanda invalida"<<endl<<flush;
			log<<_prompt<<flush;
		}
	}
}

void ClientConnection::select_handler() 
{
	if(!pending_transfers.empty()) 
	{
		TransferFile file = pending_transfers.front();

		pending_transfers.pop();

		if(file.op_type == 1)	// 1 == Upload
		{
			char buffer[4096];
			memset(buffer, 0, 4096);

			buffer[0] = 12;			// Cod de bloc de bytes

			int read_result = read(file.read_fd, buffer + 1, 4095);

			if(read_result == 0)	// Daca am ajuns la finalul fisierului
			{
				buffer[0] = 14;		//	Sfarsit de bloc de bytes

				send_message(buffer, 1);

				cout<<"Upload finished: "<<file.name<<" - "<<file.dimension<<" bytes "<<endl<<flush;
				log<<"Upload finished: "<<file.name<<" - "<<file.dimension<<" bytes "<<endl<<flush;

				cout<<_prompt<<flush;
				log<<_prompt<<flush;

				close(file.read_fd);
			} 
			else
			{
				pending_transfers.push(file);

				//cout<<"Am trimis: "<<read_result + 1<<" bytes"<<endl<<flush;
				send_message(buffer, read_result + 1);
			}
		}
		else if(file.op_type == 2)	// primesc ceva de la server, deci fac download
		{
			char buffer[4096];
			memset(buffer, 0, 4096);

			int bytes_received = recv(_client_socket, buffer, 4096, 0);

			if(buffer[0] == 12)		// bytes block
			{
				pending_transfers.push(file);

				write(file.write_fd, buffer + 1, bytes_received - 1);
			}
			else if(buffer[0] == 14)	// end data block
			{
				//cout<<"Download finished: "<<file.name<<" - "<<file.dimension<<" bytes "<<endl<<flush;
				log<<"Download finished: "<<file.name<<" - "<<file.dimension<<" bytes "<<endl<<flush;

				//cout<<_prompt<<flush;
				log<<_prompt<<flush;

				close(file.write_fd);
			}
		}
	} 
	else if(exit_token == true)
	{
		set_flag("exit");
		return;
	}

	struct timeval tv;

	tv.tv_sec = 0;
	tv.tv_usec = 5000; 	// 5 ms


	_tmp_fds = _read_fds;

	int select_result = 0;

	if(!pending_transfers.empty())			// Daca am transferuri in curs, astept doar 10 ms
		select_result = select(_max_fd_range + 1, &_tmp_fds, NULL, NULL, &tv);

	else if(exit_token == false)			// Altfel, astept pana cand am input de la user sau mesaj de la server
		select_result = select(_max_fd_range + 1, &_tmp_fds, NULL, NULL, NULL);

	is_valid(select_result, "select");
	
	unset_flags();

	if(select_result != 0) 
	{
		for(int i = 0; i<=_max_fd_range; i++) 
		{
			if(FD_ISSET(i, &_tmp_fds))
			{
				if(i == _client_socket) 
					set_flag("server_response");

				else if(i == 0) 
					set_flag("stdin_msg");
			}
		}
	}
}

void ClientConnection::close_connection()
{
	close(_client_socket);
}

void ClientConnection::init_connection_parameters(char* ip, char* port)
{
	_serv_addr.sin_family = AF_INET;
    _serv_addr.sin_port = htons(atoi(port));
    inet_aton(ip, &_serv_addr.sin_addr);
}

void ClientConnection::initialize_connection(char* ip, char* port)
{
	init_connection_parameters(ip, port);

	_client_socket = socket(AF_INET, SOCK_STREAM, 0);
	is_valid(_client_socket, "socket");

	int connect_result = connect(_client_socket,(struct sockaddr*) &_serv_addr,sizeof(_serv_addr));
	is_valid(connect_result, "connect");

	FD_ZERO(&_read_fds);
    FD_ZERO(&_tmp_fds);

	FD_SET(_client_socket, &_read_fds);
	FD_SET(0, &_read_fds);
	_max_fd_range = _client_socket;

	create_log_file();
	
	cout<<_prompt<<flush;
}

void ClientConnection::create_log_file()
{	
	char pid[10];
	char name[100];

	strcpy(name, "client");
	strcat(name, "-");
	
	sprintf(pid, "%d", getpid());

	strcat(name, pid);
	strcat(name, ".log");

	log.open(name);
}

void ClientConnection::unset_flags() 
{
	server_response = false;
	stdin_msg = false;
	brute_force_flag = false;
	exit_flag = false;
}

bool ClientConnection::get_flag(std::string flag) 
{
	if(flag == "server_response")
		return server_response;
	else if(flag == "stdin_msg")
		return stdin_msg;
	else if(flag == "bruteforce")
		return brute_force_flag;
	else if(flag == "exit")
		return exit_flag;
}

void ClientConnection::set_flag(std::string flag)
{
	if(flag == "server_response")
		server_response = true;
	else if(flag == "stdin_msg")
		stdin_msg = true;
	else if(flag == "bruteforce")
		brute_force_flag = true;
	else if(flag == "exit")
		exit_flag = true;
}
