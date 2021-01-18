#include <string>

int SizeOf( const std::string &s);
char * Serialize(const std::string & name, char * buffer);
char * DeSerialize(char * buffer, std::string & name);
