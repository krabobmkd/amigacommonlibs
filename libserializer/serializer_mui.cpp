#include "serializer_mui.h"

#include <vector>

#include <proto/alib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

#include <stdio.h>
#include <string.h>
#include "asmmacros.h"

extern "C" {
    #include <libraries/mui.h>
    #include <inline/muimaster.h>
    #include <libraries/asl.h>
}

typedef ULONG (*RE_HOOKFUNC)(); // because C++ type issue.

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
    // tested ok
    for(Level *plv : _stack) if(plv) delete plv;
}
// ---------------------------------------------------------------------------------
// -------      Fullfil Serializer API by creating bridge Objects to mirror MUI ----
// --------------------------------------------------------------------------------
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
    LString *plevel = new LString(str,flags);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;

}
// for sliders
void MUISerializer::operator()(const char *sMemberName, int &v, int min, int max)
{
    LSlider *plevel = new LSlider(v,min,max);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;

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
// for screen ids
void MUISerializer::operator()(const char *sMemberName, ULONG_SCREENMODEID &v)
{
    LScreenModeReq *plevel = new LScreenModeReq(v);
    _stack.push_back(plevel);

    plevel->_pMemberName = sMemberName;

    *_pGrower = plevel;
    _pGrower = &plevel->_pNextBrother;
}
// per optional subconf
void MUISerializer::operator()(const char *sMemberName, AStringMap &confmap)
{
    ASerializable &subconf = confmap.get("current"); // we need one to build UI.

    Level *plevel;
    // let's add a sub level.
    plevel = new LSwitchGroup(0,confmap);
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
// -----------------------------------------------------------------------------
// -------   MUISerializer compile function to finalize bridge objects   --------
// ----------------------------------------------------------------------------
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
   // printf("compile stacksize:%d\n",(int)_stack.size());
    //  leaf to root widget creation
    list<Level *>::reverse_iterator rit = _stack.rbegin();
    int i=0;
    while(rit != _stack.rend())
    {
        Level *level = *rit++;
     //   printf("compile:%d\n",i);
        if(!level->_Object) level->compile(); // create object if not done
        i++;
    }
  // printf("end compile\n");
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
void MUISerializer::selectGroup(std::string groupurl,std::string selection)
{
    // "Display.Per Screen Mode"
    Level *p = _pRoot;
    if(!p) return;
    size_t i=0;
    while(i != string::npos)
    {
        size_t j=groupurl.find(".",i);

        string str;
        if(j != string::npos) str = groupurl.substr(i,(j-i));
        else str = groupurl.substr(i);
       // printf("str:%s\n",str.c_str());

        p = p->getChild(str.c_str());

        if(!p) return;
        if(j != string::npos) i = j+1;
        else i=j;
    }
    if(!p) return;
    LSwitchGroup *pGroup = (LSwitchGroup *)p;
  //  printf("got from url::%s \n",pGroup->_pMemberName);
    pGroup->setGroup(selection.c_str());

}
// -----------------------------------------------------------------------------
// -------   The inner bridge classes that links MUI Gadgets to configs .... ----
// ----------------------------------------------------------------------------
// - - - - - - - - - - - - - - -
// post compilation of mui gadgets...
MUISerializer::Level::Level()
 : _Object(NULL)
 , _pMemberName(NULL)
 , _pFirstChild(NULL)
 , _pNextBrother(NULL)
{}
MUISerializer::Level *MUISerializer::Level::getChild(const char *pMemberName)
{
    Level *plevel = _pFirstChild;
    int nbadded=0;
    while(plevel)
    {
        if(plevel->_Object && plevel->_pMemberName &&
                strcmp(pMemberName,plevel->_pMemberName)==0)
        {
            return plevel;
        }
        plevel = plevel->_pNextBrother;
    }
    return NULL;
}
// - - - - - - - - - - - - - - -
MUISerializer::LTabs::LTabs() : LGroup(0)
{

}

void MUISerializer::LTabs::compile()
{
 //   printf("LTabs::compile\n");
  //  std::vector<const char *> registerTitles;
    _registerTitles.clear();

    Level *plevel = _pFirstChild;
    while(plevel)
    {
      //  printf("name: plevel->_Object:%08x name:%s\n",(int)plevel->_Object,plevel->_pMemberName);

         if(plevel->_Object)
         {
         //   printf("name:%s\n",plevel->_pMemberName);
            _registerTitles.push_back(plevel->_pMemberName);
         }
        plevel = plevel->_pNextBrother;
    }
    _registerTitles.push_back(NULL);
  //  printf("end names\n");

    std::vector<ULONG> tagitems={
        MUIA_Register_Titles,(ULONG)_registerTitles.data()
    };

    plevel = _pFirstChild;
    while(plevel)
    {
        if(plevel->_Object)
        {
      //  printf("add child:%s\n",plevel->_pMemberName);
            tagitems.push_back(Child);
            tagitems.push_back((ULONG)plevel->_Object);
        }
        plevel = plevel->_pNextBrother;
    }
  //  printf("end childs\n");
    tagitems.push_back(TAG_DONE);

    _Object = MUI_NewObjectA(MUIC_Register, (struct TagItem *) tagitems.data());


}
// - - - - - - - - - - - - - - -
MUISerializer::LGroup::LGroup(int flgs) : Level(), _flgs(flgs)
{
}
void MUISerializer::LGroup::compile()
{
    int nbcolumns = 2;
    if(_flgs & SERFLAG_GROUP_2COLUMS) nbcolumns=4;
    vector<ULONG> tagitems= {MUIA_Group_Columns,nbcolumns,MUIA_HorizWeight,1000};
    Level *plevel = _pFirstChild;
    int nbadded=0;
    while(plevel)
    {
        if(plevel->_Object && plevel->_pMemberName)
        {
       // printf("group: add member:%s\n",plevel->_pMemberName);
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

    _Object = compileOuterFrame(InnerGroup);

}
Object *MUISerializer::LGroup::compileOuterFrame(Object *pinnerGroup)
{
    return MUI_NewObject(MUIC_Group,
        Child, (ULONG)HVSpace,
        Child, (ULONG)MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
            Child, (ULONG)HSpace(0),
            Child,(ULONG)pinnerGroup,
            Child, (ULONG)HSpace(0),
            TAG_DONE
            ),
        Child, (ULONG)HVSpace,
        TAG_DONE
        );
}

void MUISerializer::LGroup::update()
{
    Level *plevel = _pFirstChild;
    while(plevel)
    {
        plevel->update();
        plevel = plevel->_pNextBrother;
    }
}
// - - - - - - - - - - - - - - -
MUISerializer::LSwitchGroup::LSwitchGroup(int flgs,AStringMap &map) : LGroup(flgs)
  ,_map(&map), _displayName("(select a driver)")
{

}
Object *MUISerializer::LSwitchGroup::compileOuterFrame(Object *pinnerGroup)
{
    Object *obj = MUI_NewObject(MUIC_Group,
            GroupFrameT((ULONG)_displayName.c_str()),
            MUIA_Disabled, TRUE,
            Child,(ULONG)pinnerGroup,
//        Child, (ULONG)HVSpace,
//        Child, (ULONG)MUI_NewObject(MUIC_Group,MUIA_Group_Horiz,TRUE,
//            Child, (ULONG)HSpace(0),
//            Child,(ULONG)pinnerGroup,
//            Child, (ULONG)HSpace(0),
//            TAG_DONE
//            ),
//        Child, (ULONG)HVSpace,
        TAG_DONE
        );
    return obj;
}
void MUISerializer::LSwitchGroup::setGroup(const char *pid)
{
    if(!_map || !_Object) return;
    _displayName = "Configuration for: ";
    _displayName += pid;
    ASerializable &actual = _map->get(pid); // we need one to build UI.
    ReAssigner reassigner(*this);
    actual.serialize(reassigner); // change pointed data and does updates.
    // change title name
     SetAttrs(_Object, MUIA_Disabled, FALSE,TAG_DONE);
     SetAttrs(_Object,MUIA_FrameTitle,(ULONG)_displayName.c_str(),TAG_DONE);
}
// - - - - - - - - - - - - - - -

MUISerializer::LString::LString(std::string &str,int flgs): Level()
 , _str(&str),_flgs(flgs),_STRING_Path(NULL)
{

}
ULONG ASM MUISerializer::LString::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), const char **par REG(a1))
{
    if(!par || !hook) return 0;

    LString *plevel = (LString *)hook->h_Data;
    (*plevel->_str) = *par;

    return 0;
}
void MUISerializer::LString::compile()
{
  //  printf("LString::compile\n");
    if(_flgs & SERFLAG_STRING_ISPATH)
    {
        // if manage path, have a requester and all.
        _Object = MUI_NewObject(MUIC_Popasl,
                  MUIA_Popstring_String,(ULONG)(_STRING_Path = OString(0, 2048)),
                  MUIA_Popstring_Button, (ULONG)(PopButton(MUII_PopDrawer)),
                  ASLFR_DrawersOnly,    TRUE,
                TAG_DONE);
    } else
    {
        // if just a simple editable string
        _STRING_Path = OString(0, 2048);
        _Object = _STRING_Path;
    }

    if(_STRING_Path)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&MUISerializer::LString::HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_STRING_Path,MUIM_Notify,
           MUIA_String_Contents/*MUIA_String_Acknowledge*/,
           MUIV_EveryTime,
           _Object,
           3, //?
            MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue
           );
    }


}
void  MUISerializer::LString::update()
{
    if(!_str || !_Object || !_STRING_Path) return;
   // printf("LString::update:%s\n",_str.c_str());
    SetAttrs(_STRING_Path,MUIA_String_Contents,(ULONG)_str->c_str(),TAG_DONE);
}
// - - - - - - - - - - - - - - -
ULONG ASM MUISerializer::LSlider::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    if(!hook || !par) return 0;
    LSlider *plevel = (LSlider *)hook->h_Data;
    *plevel->_value = *par;
   // printf("LSlider::HNotify:%d\n",*par);
   return 0;
}
MUISerializer::LSlider::LSlider(int &value,int min,int max): Level()
 , _value(&value),_min(min),_max(max)
{

}
void MUISerializer::LSlider::compile()
{
    _Object = MUI_NewObject(MUIC_Slider,
              MUIA_Slider_Min,    _min,
              MUIA_Slider_Max,    _max,
            TAG_DONE);
    if(_Object)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_Object,MUIM_Notify,
                    MUIA_Slider_Level, // attribute that triggers the notification.
                    MUIV_EveryTime, // TrigValue ,  every time when TrigAttr changes
                    _Object, // object on which to perform the notification method. or MUIV_Notify_Self
                    3, // FollowParams  number of following parameters (in hook ?)
                    MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue);

    }

}
void MUISerializer::LSlider::update()
{
    if(!_Object) return;
    SetAttrs(_Object,MUIA_Slider_Level,*_value,TAG_DONE);
}
// - - - - - - - - - - - - - - -
MUISerializer::LCycle::LCycle(int &value,const std::vector<std::string> &values): Level()
 ,_value(&value),_values(values)
{
    _valuesptr.reserve(_values.size()+1);
    for(const string &str : _values) _valuesptr.push_back(str.c_str());
    _valuesptr.push_back(NULL);
}

ULONG ASM MUISerializer::LCycle::HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1))
{
    if(!hook || !par) return 0;
    MUISerializer::LCycle *plevel = (MUISerializer::LCycle *)hook->h_Data;
    *plevel->_value = *par;
   return 0;
  //  printf("LCycle::HNotify:%d\n",(int)plevel->_value);
}

void MUISerializer::LCycle::compile()
{
   // printf("LCycle::compile _values size:%d\n",(int)_values.size());

    _Object = MUI_NewObject(MUIC_Cycle,MUIA_Cycle_Entries,(ULONG)_valuesptr.data(),TAG_DONE);
    if(_Object)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_Object, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
                   _Object,
                    3, //number of following parameters. If you e.g.
                    // have a notification method with three parts
                    // (maybe MUIM_Set,attr,val), you have to set
                    // FollowParams to 3.
                    // - - - notification methods
                    MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue);

    }
}
void MUISerializer::LCycle::update()
{
    if(!_Object) return;
    int v = *_value;
    if(v<0) v=0;
    if(_values.size()>0 && v>=(int)_values.size()) v =(int)_values.size()-1;
    SetAttrs(_Object,MUIA_Cycle_Active,v,TAG_DONE);
}
// - - - - - - - - - - - - - - -
MUISerializer::LCheckBox::LCheckBox(bool &value): Level()
, _value(&value)
{

}
void MUISerializer::LCheckBox::compile()
{
    _Object = MUI_NewObject(MUIC_Image,
                ImageButtonFrame,
                MUIA_InputMode        , MUIV_InputMode_Toggle,
                MUIA_Image_Spec       , MUII_CheckMark,
                MUIA_Image_FreeVert   , TRUE,
                MUIA_Selected         , (ULONG)(_value?1:0),
                MUIA_Background       , MUII_ButtonBack,
                MUIA_ShowSelState     , FALSE,
                TAG_DONE);
    if(_Object)
    {
        _notifyHook.h_Entry =(RE_HOOKFUNC)&MUISerializer::LCheckBox::HNotify;
        _notifyHook.h_Data = this;
        DoMethod(_Object,MUIM_Notify,
                    MUIA_Selected, // attribute that triggers the notification.
                    MUIV_EveryTime, // TrigValue ,  every time when TrigAttr changes
                    _Object, // object on which to perform the notification method. or MUIV_Notify_Self
                    3, // FollowParams  number of following parameters (in hook ?)
                    MUIM_CallHook,(ULONG) &_notifyHook,  MUIV_TriggerValue);

    }

}
void MUISerializer::LCheckBox::update()
{
    if(!_Object || !_value) return;
    SetAttrs(_Object,MUIA_Selected,(ULONG)(*_value?1:0),TAG_DONE);

}
ULONG ASM MUISerializer::LCheckBox::HNotify(
        struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1)
        )
{
    if(!hook || !par ) return 0;
    MUISerializer::LCheckBox *plevel = (MUISerializer::LCheckBox *)hook->h_Data;
    if(plevel->_value)
    {
        *plevel->_value = (*par != 0);
    }
    return 0;
}
// ---------------------

MUISerializer::LScreenModeReq::LScreenModeReq(ULONG_SCREENMODEID &value) : Level()
 ,_value(&value)
{
    _ScreenModeTags =
    {
      { ASLSM_InitialDisplayID,   0 },
//      { ASLSM_InitialDisplayDepth,  8 },
//      { ASLSM_DoDepth,        TRUE  },
      { TAG_END,0 }
    };
#define SMT_DISPLAYID 0
//#define SMT_DEPTH     1
}
void MUISerializer::LScreenModeReq::compile()
{
    _ScreenModeStartHook.h_Entry = (RE_HOOKFUNC) PopupStart;
    _ScreenModeStartHook.h_Data = this;
    _ScreenModeStopHook.h_Entry = (RE_HOOKFUNC) PopupStop;
    _ScreenModeStopHook.h_Data = this;

     _Object = MUI_NewObject(MUIC_Popasl,

              MUIA_Popstring_String,(ULONG)( _DisplayName = MUI_NewObject(MUIC_Text,
                    TextFrame,
                    MUIA_Background, MUII_TextBack,
                  TAG_DONE)),

              MUIA_Popstring_Button,(ULONG)(_PopUpScreenMode = PopButton(MUII_PopUp)),
              MUIA_Popasl_Type,     ASL_ScreenModeRequest,
              MUIA_Popasl_StartHook,  (ULONG) &_ScreenModeStartHook,
              MUIA_Popasl_StopHook, (ULONG) &_ScreenModeStopHook,
    //          MUIA_Disabled, (ULONG) (/*Config[CFG_SCREENTYPE] != CFGST_CUSTOM*/1),
            TAG_DONE);


}
void MUISerializer::LScreenModeReq::update()
{
    if(!_value) return;
    _ScreenModeTags[SMT_DISPLAYID].ti_Data = *_value;
    SetDisplayName(*_value);
}


ULONG ASM MUISerializer::LScreenModeReq::PopupStart(struct Hook *hook REG(a0), APTR popasl REG(a2), struct TagItem *taglist REG(a1))
{
    if(!hook || !taglist || !hook->h_Data) return 1;
    MUISerializer::LScreenModeReq *plevel = (MUISerializer::LScreenModeReq *)hook->h_Data;

    LONG  i;
    for(i = 0; taglist[i].ti_Tag != TAG_END; i++);

    taglist[i].ti_Tag = TAG_MORE;
    taglist[i].ti_Data  = (ULONG) plevel->_ScreenModeTags.data();

    return(TRUE);
}

ULONG ASM MUISerializer::LScreenModeReq::PopupStop(struct Hook *hook REG(a0), APTR popasl REG(a2), struct ScreenModeRequester *smreq REG(a1))
{
    if(!hook || !popasl || !hook->h_Data) return 1;
    MUISerializer::LScreenModeReq *plevel = (MUISerializer::LScreenModeReq *)hook->h_Data;

    plevel->_ScreenModeTags[SMT_DISPLAYID].ti_Data = smreq->sm_DisplayID;
 //no more   plevel->_ScreenModeTags[SMT_DEPTH].ti_Data   = smreq->sm_DisplayDepth;

    *(plevel->_value) = (ULONG_SCREENMODEID)smreq->sm_DisplayID;
    plevel->SetDisplayName(smreq->sm_DisplayID);
}

void MUISerializer::LScreenModeReq::SetDisplayName(ULONG displayid)
{
  if(!_DisplayName) return;
  if(displayid == INVALID_ID)
  {
    _strDisplay = "Invalid";
  }
  else
  {
    LONG v;
    struct NameInfo DisplayNameInfo;
    v = GetDisplayInfoData(NULL, (UBYTE *) &DisplayNameInfo, sizeof(DisplayNameInfo),
                         DTAG_NAME, displayid);

    if(v > sizeof(struct QueryHeader))
    {
        _strDisplay = (const char *) DisplayNameInfo.Name;
    }
    // doesnt fit, and no one care.
//    char temp[16];//    char temp[16];
    //    snprintf(temp,15," (0x%08x)",(int)displayid);
    //    _strDisplay += temp;
//    snprintf(temp,15," (0x%08x)",(int)displayid);
//    _strDisplay += temp;
  }

  SetAttrs(_DisplayName, MUIA_Text_Contents, (ULONG) _strDisplay.c_str(),TAG_DONE);
}

// - - - - - - - - - - - - - - -
// used to change edited object assignment.
MUISerializer::ReAssigner::ReAssigner::ReAssigner(MUISerializer::Level &group)
{
    _stack.push_back(&group);
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, ASerializable &subconf, int flags)
{
    //TODO if you want recursion
    //    LGroup *pgroup = (LGroup *)_stack.back();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, std::string &str, int flags)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LString *pstr = (LString *) pgroup->getChild(sMemberName);
    if(!pstr) return;
    pstr->_str = &str;
    pstr->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, int &v, int min, int max)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LSlider *pslider = (LSlider *) pgroup->getChild(sMemberName);
    if(!pslider) return;
    pslider->_value  = &v;
    pslider->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, int &v,const std::vector<std::string> &values)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LCycle *pcycle = (LCycle *) pgroup->getChild(sMemberName);
    if(!pcycle) return;
    pcycle->_value  = &v;
    pcycle->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, bool &v)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LCheckBox *pcb = (LCheckBox *) pgroup->getChild(sMemberName);
    if(!pcb) return;
    pcb->_value  = &v;
    pcb->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, ULONG_SCREENMODEID &v)
{
    LGroup *pgroup = (LGroup *)_stack.back();
    LScreenModeReq *preq = (LScreenModeReq *) pgroup->getChild(sMemberName);
    if(!preq) return;
    preq->_value  = &v;
    preq->update();
}
void MUISerializer::ReAssigner::operator()(const char *sMemberName, AStringMap &m)
{
    // todo... shouldnt be recursive...
}
