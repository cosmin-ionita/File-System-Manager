#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <set>
#include <list>
#include "User.h"
using namespace std;

class UserManager 
{

	public:
		void add_user(string name, string pass);
		void load_users(char* file_name);

		list<string> get_users();

	private:
		list<User> _users;
};


#endif