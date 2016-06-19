#ifndef MESSAGE_H
#define MESSAGE_H

/*
	Structura mesajului:

	+----+--------------+
	|	 |				|
	| ID |	  PAYLOAD	|
	|	 |				|
	+----+--------------+
	
	Unde ID = 

	1 - login
	2 - logout
	3 - getfilelist
	4 - getuserlist
	5 - upload
	6 - download
	7 - share
	8 - unshare
	9 - delete
	10 - server_error
	11 - server_success
	12 - data_block
	13 - server_brute_force_detected
	14 - end_data_block

*/

class Message
{
	public:

		void load_message(int type, char* payload, int size);
		void print_message();
		char* get_message();

	private:
		char content[4096];
};

#endif