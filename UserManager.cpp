#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <list>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include "UserManager.h"

using namespace std;

bool UserManager::exist_user(string user_name)
{
	std::list<User>::iterator it;

	for(it = _users.begin(); it != _users.end(); it++) 
	{
		if((*it).name == user_name)
			return true;
	}

	cout<<"The user does not exist";

	return false;
}

bool exist_directory(const char* directory_name)
{
	DIR *dir;

	if ((dir = opendir (directory_name)) == NULL) 	
		return false;

	return true;
}

void create_directory(const char* directory_name)
{
	mkdir(directory_name, 0755);
}

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

		if(!exist_directory(user.c_str()))
			create_directory(user.c_str());

		add_user(user, pass);
	}
}

bool UserManager::check_credentials(string user, string pass)
{
	std::list<User>::iterator it;

	for(it = _users.begin(); it != _users.end(); it++) 
	{
		if(((*it).name == user) && ((*it).password == pass))
			return true;
	}
	return false;
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

char* UserManager::get_user_list()
{
	char* result = (char*)malloc(4096 * sizeof(char));

	strcpy(result, "");

	list<string> users = get_users();

	list<string>::iterator it;

	for(it = users.begin(); it != users.end(); it++)
	{
		strcat(result, (*it).c_str());

		if( it != users.end() )
			strcat(result, "\n");
	}

	return result;
}