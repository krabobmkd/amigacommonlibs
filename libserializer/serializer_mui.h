#ifndef KRB_SERIALIZER_MUI_H_
#define KRB_SERIALIZER_MUI_H_

#include "serializer.h"
#include <vector>
#include <list>
extern "C" {
	#include <exec/types.h>
    #include <intuition/classusr.h>
    #include <utility/hooks.h>
}
#ifdef __GNUC__
#define ASM
#define REG(r) __asm(#r)
#endif

struct ScreenModeRequester;

/** Will mirror MUI panels and gadgets to serialized data.
*/
struct MUISerializer : public ASerializer {
	
	MUISerializer();
    virtual ~MUISerializer();
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;

    void operator()(const char *sMemberName, std::string &str, int flags=0) override;
    // for sliders
    void operator()(const char *sMemberName, int &v, int min, int max) override;
    // for cycling
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    // for checkbox
    void operator()(const char *sMemberName, bool &v) override;
    // for screen ids
    void operator()(const char *sMemberName, ULONG_SCREENMODEID &v) override;
	// serialize abstract class string map
    void operator()(const char *sMemberName, AStringMap &m) override;

	// - - - - - -	
    // allow insertion of tabs before compile...
    void insertFirstPanel(Object *pPanel,const char *pName);

    Object *compile();

    void updateUI();

    // switch LGroupSubMap
    void selectGroup(std::string groupurl,std::string selection);

protected:

	struct Level {
    	Level();
        Object *_Object; // if NULL, need compilation at end, else leaf are done at once
        const char *_pMemberName;
        struct Level *_pFirstChild;
        struct Level *_pNextBrother;
        virtual void compile() {}
        virtual void update() {}
        Level *getChild(const char *pMemberName);
	};
    struct LGroup : public Level {
        LGroup(int flgs);
        void compile() override;
        void update() override;
        virtual Object *compileOuterFrame(Object *pinnerGroup);
        int _flgs;
	};
    struct LSwitchGroup : public LGroup {
        LSwitchGroup(int flgs,AStringMap &map);
        Object *compileOuterFrame(Object *pinnerGroup) override;
        void setGroup(const char *pid);
        AStringMap *_map;
        std::string _displayName;
	};
    struct LTabs : public LGroup {
        LTabs();
        void compile() override;
        std::vector<const char *> _registerTitles;
	};

//    struct LSwitchGroup : public Level {
//        LSwitchGroup();
//        void compile() override;
//        void update() override;
//        void setTo(const char *pGroupName);
//        std::map<std::string,std::unique_ptr<ASerializable>> *_confmap;
//	};
    struct LString : public Level {
        static ULONG ASM HNotify(struct Hook *hook REG(a0), APTR obj REG(a2),const char **par REG(a1));
        LString(std::string &str, int flgs);
        void compile() override;
        void update() override;
        std::string *_str;
        int _flgs;
        Object *_STRING_Path;
        struct Hook _notifyHook;
	};
    struct LSlider : public Level {
        static ULONG ASM HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
        LSlider(int &value,int min,int max);
        void compile() override;
        void update() override;
        int *_value;
        int _min,_max;
        struct Hook _notifyHook;
	};
    struct LCycle : public Level {
        static ULONG ASM HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
        LCycle(int &value,const std::vector<std::string> &values);
        void compile() override;
        void update() override;
        int *_value;
        std::vector<std::string> _values;
        std::vector<const char *> _valuesptr;
        struct Hook _notifyHook;
	};
    struct LCheckBox : public Level {
        LCheckBox(bool &value);
        void compile() override;
        void update() override;
        bool *_value;
        static ULONG ASM HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
        struct Hook _notifyHook;
	};
    struct LScreenModeReq : public Level {
        LScreenModeReq(ULONG_SCREENMODEID &value);
        void compile() override;
        void update() override;
        ULONG_SCREENMODEID *_value;
        Object *_DisplayName;
        Object *_PopUpScreenMode;
        struct Hook _ScreenModeStartHook;
        struct Hook _ScreenModeStopHook;
        static ULONG ASM PopupStart(struct Hook *hook REG(a0), APTR popasl REG(a2), struct TagItem *taglist REG(a1));
        static ULONG ASM PopupStop(struct Hook *hook REG(a0), APTR popasl REG(a2), struct ScreenModeRequester *smreq REG(a1));
        void SetDisplayName(ULONG displayid);
        std::vector<struct TagItem> _ScreenModeTags;
        std::string _strDisplay;
	};
    std::list<Level *> _stack;
    int _irecurse;
    Level **_pGrower;
    Level *_pRoot;

    // used to change asignment by LGroupSubMap.
    struct ReAssigner : public ASerializer {
        ReAssigner(Level &group);
        void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
        void operator()(const char *sMemberName, std::string &str, int flags=0) override;
        void operator()(const char *sMemberName, int &v, int min, int max) override;
        void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
        void operator()(const char *sMemberName, bool &v) override;
        void operator()(const char *sMemberName, ULONG_SCREENMODEID &v) override;
        void operator()(const char *sMemberName, AStringMap &m) override;
        std::list<Level *> _stack;
    };

};


#endif
