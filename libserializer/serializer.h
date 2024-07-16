#ifndef KRB_SERIALIZER_H_
#define KRB_SERIALIZER_H_

#include <vector>
#include <map>
#include <memory>
#include <string>

struct ASerializer;

#define SERFLAG_STRING_ISPATH 1

#define SERFLAG_GROUP_TABS 1
#define SERFLAG_GROUP_2COLUMS 2


struct ASerializable {
    virtual void serialize(ASerializer &serializer)=0;
};

typedef std::string strcomment;

struct ASerializer {

    virtual void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) = 0;
    virtual void operator()(const char *sMemberName, std::string &str, int flags) = 0;
    // for sliders
    virtual void operator()(const char *sMemberName, int &v, int min, int max) = 0;
    // for cycling
    virtual void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) = 0;
    // for checkbox
    virtual void operator()(const char *sMemberName, bool &v) = 0;
   //re? virtual void operator()(const char *sMemberName, strcomment &str) = 0;
    // per known mode.
//    virtual void operator()(const char *sMemberName,
//            std::map<std::string,std::unique_ptr<ASerializable>> &confmap) = 0;
};



#endif
