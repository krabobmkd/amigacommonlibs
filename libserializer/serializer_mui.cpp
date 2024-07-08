#include "serializer_mui.h"

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

extern "C" {
#include <libraries/mui.h>
#include <inline/muimaster.h>
}

MUISerializer::MUISerializer()
{
	
}
void MUISerializer::operator()(const char *sMemberName, ASerializable &subconf) 
{
	
}
void MUISerializer::operator()(const char *sMemberName, std::string &str) 
{
	
}
// for sliders
void MUISerializer::operator()(const char *sMemberName, int &v, int min, int max)
{
	
}
// for cycling
void MUISerializer::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values)
{
	
}
// for checkbox
void MUISerializer::operator()(const char *sMemberName, bool &v) 
{
	
}




