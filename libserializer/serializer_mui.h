#ifndef KRB_SERIALIZER_MUI_H_
#define KRB_SERIALIZER_MUI_H_

#include "serializer.h"
#include <vector>

extern "C" {
	#include <exec/types.h>
}

struct MUISerializer : public ASerializer {
	
	MUISerializer();
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    void operator()(const char *sMemberName, std::string &str) override;
    // for sliders
    void operator()(const char *sMemberName, int &v, int min, int max) override;
    // for cycling
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    // for checkbox
    void operator()(const char *sMemberName, bool &v) override;
	
	// - - - - - -	
	struct Level {
		ULONG _obj;
		std::vector<ULONG> _taglist;
	};
	
	ULONG _rootobj;
};


#endif