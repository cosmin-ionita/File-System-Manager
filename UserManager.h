#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <set>
#include <string>
#include <list>
#include "User.h"
using namespace std;

class UserManager 
{
	public:
		void add_user(string name, string pass);
		void load_users(char* file_name);
		bool exist_user(string user_name);
		bool check_credentials(string user, string pass);
		char* get_user_list();

		list<string> get_users();

	private:
		list<User> _users;
};


#endif