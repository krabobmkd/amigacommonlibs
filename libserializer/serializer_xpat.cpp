#include "serializer_xpat.h"

#include <expat.h>

using namespace std;

xmlwriter::xmlwriter()
{
}
void xmlwriter::operator()(const char *sMemberName, ASerializable &subconf) 
{
}
void xmlwriter::operator()(const char *sMemberName, std::string &str)
{
}
void xmlwriter::operator()(const char *sMemberName, int &v, int min, int max) 
{
}
void xmlwriter::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) 
{
}
void xmlwriter::operator()(const char *sMemberName, bool &v) 
{
}

// -------------------------------

xmlreader::xmlreader()
{
}
void xmlreader::operator()(const char *sMemberName, ASerializable &subconf) 
{
}
void xmlreader::operator()(const char *sMemberName, std::string &str) 
{
}
void xmlreader::operator()(const char *sMemberName, int &v, int min, int max) 
{
}
void xmlreader::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values)
{
}
void xmlreader::operator()(const char *sMemberName, bool &v) 
{
}
