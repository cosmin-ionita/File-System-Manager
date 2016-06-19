#include "File.h"

bool File::operator==(const File file)
{
	if(file.owner == this->owner && 
		file.name == this->name && 
		file.dimension == this->dimension &&
		file.type == this->type)

		return true;
	
	return false;
}