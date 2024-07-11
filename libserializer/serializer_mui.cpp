#include "serializer_mui.h"

#include <vector>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include "macros.h"

extern "C" {
    #include <libraries/mui.h>
    #include <inline/muimaster.h>
    #include <libraries/asl.h>
}

typedef ULONG (*RE_HOOKFUNC)();

extern struct Library *MUIMasterBase;

#define OString(contents,maxlen)\
	MUI_NewObject(MUIC_String,\
		StringFrame,\
		MUIA_String_MaxLen  , maxlen,\
		MUIA_String_Contents, contents,\
		TAG_DONE)

using namespace std;

MUISerializer::MUISerializer()
    : ASerializer()
    , _irecurse(0)
    , _pGrower(&_pRoot)
    , _pRoot(NULL)
{
}
MUISerializer::~MUISerializer()
{
    for(Level *plv : _stack) if(plv) delete plv;
}
void MUISerializer::operator()(const char *sMemberName, ASerializable &subconf, int flags)
{
    Level *plevel;
    // let's add a sub level.
    if(_irecurse==0)
    {
        plevel = new LTabs();
    } else
    {
        plevel = new LGroup();
    }
    if(!plevel) return;

    _stack.push_back(plevel);
    plevel->_pMemberName = sMemberName;
    // attach
    *_pGrower = plevel;
    // next will attach down
    _pGrower = &plevel->_pFirstChild;

    _irecurse++;

        subconf.serialize(*this);

    _irecurse--;

    // next wil be bro
    _pGrower = &plevel->_pNextBrother;


}
void MUISerializer::operator()(const char *sMemberName, std::string &str,int flags)
{
    if(flags & SERFLAG_STRING_ISPATH)
    {
        LPath *plevel = new LPath(str);
        _stack.push_back(plevel);

        plevel->_pMemberName = sMemberName;

        *_pGrower = plevel;
        _pGrower = &plevel->_pNextBrother;

        // what compile would do:
        plevel->_Object = MUI_NewObject(MUIC_Popasl,
                  MUIA_Popstring_String,(ULONG)(plevel->STRING_Path = OString(0, 2048)),
                  MUIA_Popstring_Button, (ULONG)(PopButton(MUII_PopDrawer)),
                  ASLFR_DrawersOnly,    TRUE,
                TAG_DONE);
    } else
    {
        // TODO
    }

}
// for sliders
void MUISerializer::operator()(const char *sMemberName, int &v, int min, int max)
{
    LSlider *plevel = new LSlider(v,min,max);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;

    //TODO



}
// for cycling
void MUISerializer::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values)
{
	 LCycle *plevel = new LCycle(v,values);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;
}
// for checkbox
void MUISerializer::operator()(const char *sMemberName, bool &v) 
{
    LCheckBox *plevel = new LCheckBox(v);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;
}
// --------------------------------------------------------------------
// optional hack
void MUISerializer::insertFirstPanel(Object *pPanel,const char *pName)
{
    if(!_pRoot) return;
    Level *plevel = new Level();
    if(!plevel) return;

    _stack.insert(_stack.begin(),plevel); // maybe not exact place but would work and will be deleted.

    plevel->_Object = pPanel;
    plevel->_pMemberName = pName;
    plevel->_pNextBrother = _pRoot->_pFirstChild;
    _pRoot->_pFirstChild = plevel;

}
// finalize
Object *MUISerializer::compile()
{
    if(!_pRoot) return NULL;
    //  leaf to root widget creation
    list<Level *>::reverse_iterator rit = _stack.rbegin();
    while(rit != _stack.rend())
    {
        Level *level = *rit++;
        if(!level->_Object) level->compile();
    }
    return _pRoot->_Object;
}
void MUISerializer::updateUI()
{
    list<Level *>::reverse_iterator rit = _stack.rbegin();
    while(rit != _stack.rend())
    {
        Level *level = *rit++;
        if(!level->_Object) level->update();
    }

}

// - - - - - - - - - - - - - - -
// post compilation of mui gadgets...
MUISerializer::Level::Level()
 : _Object(NULL)
 , _pMemberName(NULL)
 , _pFirstChild(NULL)
 , _pNextBrother(NULL)
{}
// - - - - - - - - - - - - - - -
MUISerializer::LTabs::LTabs() : Level()
{

}

void MUISerializer::LTabs::compile()
{
    std::vector<const char *> registerTitles;

    Level *plevel = _pFirstChild;
    while(!plevel)
    {
         if(plevel->_Object) registerTitles.push_back(plevel->_pMemberName);
        plevel = plevel->_pNextBrother;
    }
    registerTitles.push_back(NULL);

    std::vector<ULONG> tagitems={
        MUIA_Register_Titles,(ULONG)registerTitles.data()
    };

    plevel = _pFirstChild;
    while(!plevel)
    {
        if(plevel->_Object)
        {
            tagitems.push_back(Child);
            tagitems.push_back((ULONG)plevel->_Object);
        }
        plevel = plevel->_pNextBrother;
    }
    tagitems.push_back(TAG_DONE);

    _Object = MUI_NewObjectA(MUIC_Register, (struct TagItem *) tagitems.data());


}
// - - - - - - - - - - - - - - -
MUISerializer::LGroup::LGroup(): Level()
,_ordertype(0)
{

}
void MUISerializer::LGroup::compile()
{
    std::vector<ULONG> tagitems;
    tagitems.push_back(Child);
    tagitems.push_back((ULONG)HVSpace);
    if(_ordertype ==0)
    {
        Level *plevel = _pFirstChild;
        while(!plevel)
        {
            //TODO  if(plevel->_Object)
            plevel = plevel->_pNextBrother;
        }

    } // end if _ordertype horiz/vert.


    tagitems.push_back(Child);
    tagitems.push_back((ULONG)HVSpace);
    tagitems.push_back(TAG_DONE);

    _Object = MUI_NewObjectA(MUIC_Group, (struct TagItem *) &tagitems);
}
// - - - - - - - - - - - - - - -
MUISerializer::LPath::LPath(std::string &str): Level()
 , _str(str), STRING_Path(NULL)
{

}
void MUISerializer::LPath::compile()
{
    // already done
}
void  MUISerializer::LPath::update()
{
    if(!_Object) return;
    SetAttrs(_Object,MUIA_String_Contents,(ULONG)_str.c_str(),TAG_DONE);
}
// - - - - - - - - - - - - - - -
MUISerializer::LSlider::LSlider(int &value,int min,int max): Level()
 , _value(value),_min(min),_max(max)
{

}
void MUISerializer::LSlider::compile()
{

}
// - - - - - - - - - - - - - - -
MUISerializer::LCycle::LCycle(int &value,const std::vector<std::string> &values): Level()
 ,_value(value),_values(values)
{

}

ULONG ASM MUISerializer::LCycle::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    MUISerializer::LCycle *plevel = (MUISerializer::LCycle *)hook->h_Data;
    plevel->_value = *par;
}

void MUISerializer::LCycle::compile()
{
    vector<const char *> valuesptr;
    for(const string &str : _values) valuesptr.push_back(str.c_str());

    _Object = MUI_NewObject(MUIC_Cycle,MUIA_Cycle_Entries,(ULONG)valuesptr.data(),TAG_DONE);
    if(_Object)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&MUISerializer::LCycle::HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_Object, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
                   _Object, (ULONG)_values.size(), MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue);

    }
}
// - - - - - - - - - - - - - - -
MUISerializer::LCheckBox::LCheckBox(bool &value): Level()
, _value(value)
{

}
void MUISerializer::LCheckBox::compile()
{

}
// - - - - - - - - - - - - - - -
