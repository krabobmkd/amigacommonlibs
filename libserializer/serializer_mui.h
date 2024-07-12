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


/** Will mirror MUI panels and gadgets to serialized data.
*/
struct MUISerializer : public ASerializer {
	
	MUISerializer();
    virtual ~MUISerializer();
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    #define SERFLAG_STRING_ISPATH 1
    void operator()(const char *sMemberName, std::string &str, int flags=0) override;
    // for sliders
    void operator()(const char *sMemberName, int &v, int min, int max) override;
    // for cycling
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    // for checkbox
    void operator()(const char *sMemberName, bool &v) override;
	
	// - - - - - -	
    // allow insertion of tabs before compile...
    void insertFirstPanel(Object *pPanel,const char *pName);

    Object *compile();

    void updateUI();

protected:

	struct Level {
    	Level();
        Object *_Object; // if NULL, need compilation at end, else leaf are done at once
        const char *_pMemberName;
        struct Level *_pFirstChild;
        struct Level *_pNextBrother;
        virtual void compile() {}
        virtual void update() {}
	};
    struct LTabs : public Level {
        LTabs();
        void compile() override;
	};
    struct LGroup : public Level {
        LGroup();
        void compile() override;
        int _ordertype;
	};
    struct LPath : public Level {
        LPath(std::string &str);
        void compile() override;
        void update() override;
        std::string &_str;
        Object *STRING_Path;
	};
    struct LSlider : public Level {
        LSlider(int &value,int min,int max);
        void compile() override;
        int &_value;
        int _min,_max;
	};
    struct LCycle : public Level {
        static ULONG ASM HNotify(struct Hook *hook REG(a0), APTR obj REG(a2), ULONG *par REG(a1));
        LCycle(int &value,const std::vector<std::string> &values);
        void compile() override;
        int &_value;
        std::vector<std::string> _values;
        std::vector<const char *> _valuesptr;
        struct Hook _notifyHook;
	};
    struct LCheckBox : public Level {
        LCheckBox(bool &value);
        void compile() override;
        bool &_value;
	};

    std::list<Level *> _stack;
    int _irecurse;
    Level **_pGrower;
    Level *_pRoot;
//	Level _root;
};


#endif
