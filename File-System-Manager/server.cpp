#include <iostream>
#include <fstream>
#include <list>
#include <vector>

#include "ServerConnection.h"
#include "UserManager.h"
#include "FileSystem.h"

using namespace std;


int main(int argc, char** argv)
{
	ServerConnection connection;
	FileSystem file_system;
	UserManager user_manager;

	user_manager.load_users(argv[2]);
	file_system.load_files(user_manager, argv[3]);
	
	connection.initialize_server(argv[1]);
	
	
	while(1) 
	{
		connection.select_handler();

		if(connection.get_flag("incoming_connection")) {
			connection.accept_handler();
		}

		else if(connection.get_flag("client_msg")) {
			connection.handle_client_message(user_manager, file_system);
		}
		
		else if(connection.get_flag("stdin_msg"))
			connection.handle_stdin();

		if(connection.get_flag("exit") == true)
			break;
	}

    return 0;
}
