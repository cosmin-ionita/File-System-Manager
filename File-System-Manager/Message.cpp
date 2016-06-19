#include <iostream>
#include <cstdio>
#include "Message.h"

using namespace std;

void Message::load_message(char type, char* payload)
{
	content[0] = type;

	memcpy(content + 1, payload, sizeof(payload));
}

void Message::print_message()
{
	cout<<content<<" "<<endl;
}