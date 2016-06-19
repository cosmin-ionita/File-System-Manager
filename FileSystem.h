#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <list>
#include <string>

#include "File.h"
#include "UserManager.h"

class FileSystem
{

	public:
		void load_files(UserManager user_manager, char* file_name);
		bool exist_file(const char* directory_name, const char* file_name);
		char* get_files_info(string user_name);
		long get_file_dimension(string directory_name, string file_name);

		bool is_loaded_file(char* file_name);

		void share(string user, string file_name);
		void unshare(string user, string file_name);
		void delete_file(string directory, string file);

		bool is_shared(string user, string file);


		void add_file(string user_name, string file_name);

		void print_files();

	private:
		list<File> _files;
};


#endif