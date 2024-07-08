#ifndef KRB_SERIALIZER_H_
#define KRB_SERIALIZER_H_

#include <vector>
#include <string>

struct ASerialization;

struct ASerializable {
    virtual void serialize(ASerialization &serializer)=0;
};

struct ASerializer {
	
    virtual void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) = 0;
    virtual void operator()(const char *sMemberName, std::string &str) = 0;
    // for sliders
    virtual void operator()(const char *sMemberName, int &v, int min, int max) = 0;
    // for cycling
    virtual void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) = 0;
    // for checkbox
    virtual void operator()(const char *sMemberName, bool &v) = 0;

};



#endif