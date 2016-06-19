#ifndef FILE_H
#define FILE_H

#include <string>
using namespace std;

class File
{
	public:

		File(string owner_name, string file_name, int file_dimension)
		{
			name = file_name;
			dimension = file_dimension;
			owner = owner_name;
			
			type = "SHARED";
		}

		File(string owner_name, string file_name, int file_dimension, string file_type)
		{
			name = file_name;
			dimension = file_dimension;
			owner = owner_name;
			
			type = file_type;
		}

		bool operator==(const File file);	// Overload this op for the "remove" method from STL

		int dimension;

		string name;
		string owner;
		string type;	// private / shared
};

#endif