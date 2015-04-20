#include "../test.h"

#ifndef _MSC_VER // VC have compilation error

#include "nbsdxSimpleJSON/json.hpp"

using namespace json;
/*
static void GenStat(Stat& stat, const JSON& v) {
    switch (v.JSONType()) {
    case JSON::Class::Array:
        {
            const Array& a = v.getArray();
            for (Array::const_iterator itr = a.begin(); itr != a.end(); ++itr)
                GenStat(stat, *itr);
            stat.arrayCount++;
            stat.elementCount += a.size();
        }
        break;

    case JSON::OBJECT:
        {
            const Object& o = v.getObject();
            for (Object::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
                GenStat(stat, itr->second);
                stat.stringLength += itr->first.size();
            }
            stat.objectCount++;
            stat.memberCount += o.size();
            stat.stringCount += o.size();
        }
        break;

    case JSON::STRING: 
        stat.stringCount++;
        stat.stringLength += v.getString().size();
        break;

    case JSON::INTEGER:
    case JSON::DOUBLE:
        stat.numberCount++;
        break;

    case JSON::BOOLEAN:
        if (v.getBoolean())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case JSON::NULL_VALUE:
        stat.nullCount++;
        break;
    }
}
*/
class NbsdxsimplejsonParseResult : public ParseResultBase {
public:
    JSON root;
};

class NbsdxsimplejsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};
class NbsdxsimplejsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "nbsdx/SimpleJSON (C++11)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        NbsdxsimplejsonParseResult* pr = new NbsdxsimplejsonParseResult;
        try {
            pr->root = JSON::Load(json);
        }
        catch (...) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const NbsdxsimplejsonParseResult* pr = static_cast<const NbsdxsimplejsonParseResult*>(parseResult);
        NbsdxsimplejsonStringResult* sr = new NbsdxsimplejsonStringResult;
        sr->s = pr->root.dump();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const NbsdxsimplejsonParseResult* pr = static_cast<const NbsdxsimplejsonParseResult*>(parseResult);
        NbsdxsimplejsonStringResult* sr = new NbsdxsimplejsonStringResult;
        sr->s = pr->root.dump(1, "    ");
        return sr;
    }
#endif

// #if TEST_STATISTICS
//     virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
//         const NbsdxsimplejsonParseResult* pr = static_cast<const NbsdxsimplejsonParseResult*>(parseResult);
//         memset(stat, 0, sizeof(Stat));
//         GenStat(*stat, pr->root);
//         return true;
//     }
// #endif
};

REGISTER_TEST(NbsdxsimplejsonTest);

#endif
