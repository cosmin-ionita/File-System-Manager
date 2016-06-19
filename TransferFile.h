#ifndef TRANSFERFILE_H
#define TRANSFERFILE_H

#include <string>

class TransferFile
{
	public:

		std::string name;
		int op_type;	// 2 === download, 1 === upload

		int dest_socket, source_socket;
		int dimension;

		int read_fd, write_fd;
};

#endif