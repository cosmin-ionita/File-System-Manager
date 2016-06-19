#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>


class User 
{
	public:

		User(std::string user_name, std::string pass)
		{
			name = user_name;
			password = pass;
		}

		std::string name, password;
};


#endif