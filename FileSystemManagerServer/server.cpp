#include <iostream>

#include "Connection.h"

using namespace std;


int main(int argc, char** argv)
{
	Connection connection;

	connection.initialize_connection(argv[1]);



    return 0;
}
