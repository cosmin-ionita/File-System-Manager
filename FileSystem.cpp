#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "FileSystem.h"

using namespace std;

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


bool FileSystem::is_shared(string user, string file)
{
	list<File>::iterator it;

	for(it = _files.begin(); it != _files.end(); it++)
	{
		if((*it).name.compare(file) == 0 && (*it).owner.compare(user) == 0)
			if((*it).type.compare("SHARED") == 0)
				return true;
	}

	return false;
}

bool FileSystem::exist_file(const char* directory_name, const char* file_name)
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
	   cout<<"Directorul nu poate fi deschis"<<endl;
	   return false;
	}

	cout<<"Fisierul "<<file_name<<" nu exista"<<endl;
	return false;
}

// Intoarce dimensiunea unui fisier

long FileSystem::get_file_dimension(string directory_name, string file_name)
{

	string path = "";

	path += directory_name;
	path += "/";
	path += file_name;

    struct stat stat_buf;

    int rc = stat(path.c_str(), &stat_buf);
    
    return rc == 0 ? stat_buf.st_size : -1;
}

// Verific daca fisierul dat ca parametru este deja incarcat in sistem

bool FileSystem::is_loaded_file(char* file_name)
{
	list<File>::iterator it;

	for(it = _files.begin(); it != _files.end(); it++)
	{
		if(strcmp((*it).name.c_str(), file_name) == 0)
			return true;
	}

	return false;
}

void FileSystem::add_file(string user_name, string file_name)
{
	File file(user_name, file_name, get_file_dimension(user_name, file_name), "PRIVATE");
	_files.push_back(file);
}


// In aceasta functie incarc fisierele publice ale fiecarui utilizator

void FileSystem::load_files(UserManager user_manager, char* file_name)
{
	int N = 0;
	string line;

	ifstream f(file_name);

	f>>N;

	getline(f, line);	// Citesc un newline inainte

	for(int i = 0; i<N; i++)
	{
		getline(f, line);

		vector<string> tokens = split(line, ':');

		if(user_manager.exist_user(tokens[0]) && exist_file(tokens[0].c_str(), tokens[1].c_str()))
		{
			File new_file(tokens[0], tokens[1], get_file_dimension(tokens[0], tokens[1]));

			_files.push_back(new_file);
		}	
	}

	// Adaug fisierele private (cele care sunt deja in folderul userului)

	list<string>::iterator it;

	list<string> users = user_manager.get_users();

	for(it = users.begin(); it != users.end(); it++)
	{
		string user_name = (*it);

		DIR *dir;
		struct dirent *ent;

		if ((dir = opendir (user_name.c_str())) != NULL) 
		{
		  	while ((ent = readdir (dir)) != NULL) 
		  	{
		    	 if(is_loaded_file(ent->d_name) == false && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
	    	 	 {
	    	 	 	string name(ent->d_name);

		    		File new_file(user_name, name, get_file_dimension(user_name, name), "PRIVATE");

		    		_files.push_back(new_file); 		
	    	 	 }
		 	}

		  	closedir (dir);
		} 
	}
}

char* FileSystem::get_files_info(string user_name)
{
	char* result = (char*)malloc(4096 * sizeof(char));

	strcpy(result, "");

	list<File>::iterator it;

	for(it = _files.begin(); it != _files.end(); it++)
	{
		File current_file = (*it);

		if(current_file.owner.compare(user_name) == 0) 
		{

			char dim[100];

			sprintf(dim, "%d", current_file.dimension);

			strcat(result, current_file.name.c_str());
			strcat(result, " ");
			strcat(result, dim);
			strcat(result, " bytes ");
			strcat(result, current_file.type.c_str());
			strcat(result, "\n");
		}
	}

	return result;
}

void FileSystem::share(string user, string file_name)
{
	list<File>::iterator it;

	for(it = _files.begin(); it != _files.end(); it++)
	{
		if(((*it).name == file_name) && ((*it).owner == user))
		{
			File &new_file = (*it);

			new_file.type = "SHARED";

			break;
		}
	}
}

void FileSystem::delete_file(string directory, string file)
{
	string remove_path = "";

	remove_path += directory;			// Construiesc calea fisierului de sters
	remove_path += "/";
	remove_path += file;

	unlink(remove_path.c_str());		// Sterg fisierul de pe disc

	int i = 0;
	
	list<File>::iterator it;

	for(it = _files.begin(); it != _files.end(); it++) // Sterg fisierul din sistem
	{
		if(((*it).owner == directory) && ((*it).name == file))
		{
			File remove_element = (*it);
			_files.remove(remove_element);
			break;
		}
		i++;
	}
}

void FileSystem::unshare(string user, string file_name)
{
	list<File>::iterator it;

	for(it = _files.begin(); it != _files.end(); it++)
	{
		if(((*it).name == file_name) && ((*it).owner == user))
		{
			File &new_file = (*it);

			new_file.type = "PRIVATE";

			break;
		}
	}
}

void FileSystem::print_files()
{

	list<File>::iterator it;

	for(it = _files.begin(); it != _files.end(); it++)
	{
		File file = (*it);

		cout<<file.name<<" - "<<file.owner<<" - "<<file.type<<" - "<<file.dimension<<endl<<flush;
	}
}

