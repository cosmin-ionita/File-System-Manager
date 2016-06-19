#include <iostream>

#include "ClientConnection.h"


using namespace std;

int main(int argc, char** argv)
{

	ClientConnection connection;

	connection.initialize_connection(argv[1], argv[2]);

	while(1)
	{
		connection.select_handler();

		if(connection.get_flag("server_response")) 
		{
			connection.handle_response();

			if(connection.get_flag("bruteforce"))
				break;
		}


		if(connection.get_flag("exit") == true) 
				break;

		else if(connection.get_flag("stdin_msg"))
			connection.handle_stdin();
	}

	connection.close_connection();

    return 0;
}
