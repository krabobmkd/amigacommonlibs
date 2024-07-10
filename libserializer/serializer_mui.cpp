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

using namespace std;

MUISerializer::MUISerializer(ASerializable &root)
{
    //Level lroot{root,0L,{}};
//	_levels.push_back(Level());
//	 Level &level = _levels.back();
//    level._serialized = &root;

}
void MUISerializer::operator()(const char *sMemberName, ASerializable &subconf, int flags)
{
    // let's add a level.

//    Level &level = _levels.back();
//    Level lroot{root,0L,{}};
//	_levels.push_back(lroot);

//    Object *pObj=NULL;

//    if(pObj)
//    {
//        level._taglist.push_back(Child);
//        level._taglist.push_back(Child);
//    }
//    level.
/*
    vector<ULONG> taglist;
   Level &level = _levels.back();

	level. =  MUI_NewObjectA(MUIC_Group, (struct TagItem *) taglist.data() );

	LONG w= UMUINO(MUIC_Group,
    Child, HVSpace,
    Child, UMUINO(MUIC_Group,MUIA_Group_Horiz,TRUE,
      Child, HSpace(0),
*/
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

Object *MUISerializer::Level::compile()
{
    _taglist.push_back(TAG_DONE);

}

// finalize
ULONG MUISerializer::compile()
{
    return _level.compile();
}

