#ifndef KRB_SERIALIZER_XPAT_H_
#define KRB_SERIALIZER_XPAT_H_

#include "serializer.h"


struct xmlwriter : public struct ASerialization {
	
	xmlwriter();
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    void operator()(const char *sMemberName, std::string &str) override;
    void operator()(const char *sMemberName, int &v, int min, int max) override;
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    void operator()(const char *sMemberName, bool &v) override;

};

struct xmlreader : public struct ASerialization {
	
	xmlreader();
    void operator()(const char *sMemberName, ASerializable &subconf, int flags=0) override;
    void operator()(const char *sMemberName, std::string &str) override;
    void operator()(const char *sMemberName, int &v, int min, int max) override;
    void operator()(const char *sMemberName, int &v,const std::vector<std::string> &values) override;
    void operator()(const char *sMemberName, bool &v) override;

};

#endif