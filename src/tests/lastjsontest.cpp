#include "../test.h"

#include "lastjson/include/lastjson/parse.hpp"
#include "lastjson/include/lastjson/stringify.hpp"

using namespace lastjson;

static void GenStat(Stat& stat, const value& v) {
    switch (v.get_type()) {
    case ARRAY:
        {
            const value::array_type& a = v.get_array();
            for (value::array_type::const_iterator itr = a.begin(); itr != a.end(); ++itr)
                GenStat(stat, *itr);
            stat.arrayCount++;
            stat.elementCount += a.size();
        }
        break;
    
    case OBJECT:
        {
            const value::object_type& o = v.get_object();
            for (value::object_type::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
                GenStat(stat, itr->second);
                stat.stringLength += itr->first.size();
            }
            stat.objectCount++;
            stat.memberCount += o.size();
            stat.stringCount += o.size();
        }
        break;

    case STRING:
        stat.stringCount++;
        stat.stringLength += v.get_string().size();
        break;
    
    case INT:
    case FLOAT:
        stat.numberCount++;
        break;

    case BOOL:
        if (v.get_bool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;
    
    case JSONNULL:
        stat.nullCount++;
        break;
    }
}

class LastjsonParseResult : public ParseResultBase {
public:
    value root;
};

class LastjsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};
class LastjsonTest : public TestBase {
public:
	LastjsonTest() : TestBase("lastjson (C++)") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        LastjsonParseResult* pr = new LastjsonParseResult;
        std::string s(json, length);
        try {
            pr->root = parse(s.begin(), s.end());
        }
        catch (...) {
            delete pr;
            pr = 0;
        }
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const LastjsonParseResult* pr = static_cast<const LastjsonParseResult*>(parseResult);
        LastjsonStringResult* sr = new LastjsonStringResult;
        sr->s = stringify(pr->root);
        return sr;
    }

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const LastjsonParseResult* pr = static_cast<const LastjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
};

REGISTER_TEST(LastjsonTest);
