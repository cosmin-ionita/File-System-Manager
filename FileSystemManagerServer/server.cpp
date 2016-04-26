#include <iostream>
#include <list>
#include "Connection.h"
#include "UserManager.h"

using namespace std;


int main(int argc, char** argv)
{
	Connection connection;


	UserManager user_mgr;

	user_mgr.load_users(argv[2]);

	list<string> users = user_mgr.get_users();

	std::list<string>::iterator it;

	for(it = users.begin(); it != users.end(); it++) 
	{
		cout<<(*it)<<" ";
	}

	/*connection.initialize_connection(argv[1]);

	while(1) 
	{
		
		connection.select_handler();

		if(connection.get_flag("incoming_connection")) ;
			//connection.accept();

		else if(connection.get_flag("client_msg")) ;
			//TODO
		
		else if(connection.get_flag("stdin_msg")) ;
			connection.handle_stdin();
	}*/

    return 0;
}
