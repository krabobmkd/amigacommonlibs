#include "serializer_mui.h"

#include <vector>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <stdio.h>

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
    printf("~MUISerializer() delete stack\n");
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
        plevel = new LGroup(flags);
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
    printf("compile stacksize:%d\n",(int)_stack.size());
    //  leaf to root widget creation
    list<Level *>::reverse_iterator rit = _stack.rbegin();
    int i=0;
    while(rit != _stack.rend())
    {
        Level *level = *rit++;
        printf("compile:%d\n",i);
        if(!level->_Object) level->compile(); // create object if not done
        i++;
    }
   printf("end compile\n");
    return _pRoot->_Object;
}
void MUISerializer::updateUI()
{
    list<Level *>::reverse_iterator rit = _stack.rbegin();
    while(rit != _stack.rend())
    {
        Level *plevel = *rit++;
        if(plevel->_Object) plevel->update();
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
    printf("LTabs::compile\n");
    std::vector<const char *> registerTitles;

    Level *plevel = _pFirstChild;
    while(plevel)
    {
        printf("name: plevel->_Object:%08x name:%s\n",(int)plevel->_Object,plevel->_pMemberName);

         if(plevel->_Object)
         {
            printf("name:%s\n",plevel->_pMemberName);
            registerTitles.push_back(plevel->_pMemberName);
         }
        plevel = plevel->_pNextBrother;
    }
    registerTitles.push_back(NULL);
    printf("end names\n");

    std::vector<ULONG> tagitems={
        MUIA_Register_Titles,(ULONG)registerTitles.data()
    };

    plevel = _pFirstChild;
    while(plevel)
    {
        if(plevel->_Object)
        {
        printf("add child:%s\n",plevel->_pMemberName);
            tagitems.push_back(Child);
            tagitems.push_back((ULONG)plevel->_Object);
        }
        plevel = plevel->_pNextBrother;
    }
    printf("end childs\n");
    tagitems.push_back(TAG_DONE);

    _Object = MUI_NewObjectA(MUIC_Register, (struct TagItem *) tagitems.data());


}
// - - - - - - - - - - - - - - -
MUISerializer::LGroup::LGroup(int flg) : Level()
{
    _flags = flg;
}
void MUISerializer::LGroup::compile()
{
    printf("LGroup::compile\n");

    //if(_ordertype ==0)
    {
        int nbcolumns = 2;
        if(_flags & SERFLAG_GROUP_2COLUMS) nbcolumns=4;
        vector<ULONG> tagitems= {MUIA_Group_Columns,nbcolumns,MUIA_HorizWeight,1000};
        Level *plevel = _pFirstChild;
        int nbadded=0;
        while(plevel)
        {
            if(plevel->_Object && plevel->_pMemberName)
            {
            printf("group: add member:%s\n",plevel->_pMemberName);
                tagitems.push_back(Child);
                tagitems.push_back((ULONG)Label((ULONG)plevel->_pMemberName));
                tagitems.push_back(Child);
                tagitems.push_back((ULONG)plevel->_Object);
                nbadded++;
            }
            plevel = plevel->_pNextBrother;
        }
        if(nbadded==0)
        {
            tagitems.push_back(Child);
            tagitems.push_back((ULONG)Label((ULONG)"-"));
            tagitems.push_back(Child);
            tagitems.push_back((ULONG)Label((ULONG)"-"));
        }
        tagitems.push_back(TAG_DONE);

        Object *InnerGroup = MUI_NewObjectA(MUIC_Group,(struct TagItem *) tagitems.data());

        // recenter/ expand
        _Object = MUI_NewObject(MUIC_Group,
                    Child, (ULONG)HVSpace,
                    Child, (ULONG)MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
                        Child, (ULONG)HSpace(0),
                        Child,(ULONG)InnerGroup,
                        Child, (ULONG)HSpace(0),
                        TAG_DONE
                        ),
                    Child, (ULONG)HVSpace,
                    TAG_DONE
                    );
    } // end if _ordertype horiz/vert.
}
// - - - - - - - - - - - - - - -
MUISerializer::LPath::LPath(std::string &str): Level()
 , _str(str), STRING_Path(NULL)
{

}
void MUISerializer::LPath::compile()
{
    printf("LPath::compile (empty)\n");
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
    printf("LSlider::compile (todo)\n");
}
// - - - - - - - - - - - - - - -
MUISerializer::LCycle::LCycle(int &value,const std::vector<std::string> &values): Level()
 ,_value(value),_values(values)
{
    _valuesptr.reserve(_values.size()+1);
    for(const string &str : _values) _valuesptr.push_back(str.c_str());
    _valuesptr.push_back(NULL);
}

ULONG ASM MUISerializer::LCycle::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    MUISerializer::LCycle *plevel = (MUISerializer::LCycle *)hook->h_Data;
    plevel->_value = *par;
  //  printf("LCycle::HNotify:%d\n",(int)plevel->_value);
}

void MUISerializer::LCycle::compile()
{
   // printf("LCycle::compile _values size:%d\n",(int)_values.size());

    _Object = MUI_NewObject(MUIC_Cycle,MUIA_Cycle_Entries,(ULONG)_valuesptr.data(),TAG_DONE);
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
    printf("LCheckBox::compile (todo)\n");
}
// - - - - - - - - - - - - - - -
