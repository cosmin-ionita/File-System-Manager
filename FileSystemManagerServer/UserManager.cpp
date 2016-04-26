#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include "UserManager.h"

using namespace std;

void UserManager::load_users(char* file_name)
{
	int N;
	string user, pass;

	ifstream f(file_name);

	f>>N;

	for(int i = 0; i<N; i++) 
	{
		f>>user;
		f>>pass;

		add_user(user, pass);
	}
}

void UserManager::add_user(string name, string pass)
{
	User new_user(name, pass);

	_users.push_back(new_user);
}

list<string> UserManager::get_users() 
{
	list<string> user_list;

	std::list<User>::iterator it;

	for(it = _users.begin(); it != _users.end(); it++) 
	{
		user_list.push_back((*it).name);
	}

	return user_list;
}