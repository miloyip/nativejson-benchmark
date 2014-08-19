#include "../test.h"
#ifdef _MSC_VER
#pragma warning (disable:4189) // local variable is initialized but not referenced
#pragma warning (disable:4127) // conditional expression is constant
#endif
#include "picojson/picojson.h"

using namespace picojson;

static void GenStat(Stat& s, const value& v) {
    // Note: No public API to get value::_type
    if (v.is<object>()) {
        const object& o = v.get<object>();
        for (object::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
            s.stringLength += itr->first.size(); // key
            GenStat(s, itr->second);
        }
        s.objectCount++;
        s.memberCount += o.size();
        s.stringCount += o.size();  // Key
    }
    else if (v.is<array>()) {
        const array& a = v.get<array>();
        for (array::const_iterator itr = a.begin(); itr != a.end(); ++itr)
            GenStat(s, *itr);
        s.arrayCount++;
        s.elementCount += a.size();
    }
    else if (v.is<std::string>()) {
        s.stringCount++;
        s.stringLength += v.get<std::string>().size();
    }
    else if (v.is<double>())
        s.numberCount++;
    else if (v.is<bool>()) {
        if (v.get<bool>())
            s.trueCount++;
        else
            s.falseCount++;
    }
    else if (v.is<null>())
        s.nullCount++;
}

class PicojsonParseResult : public ParseResultBase {
public:
    value v;
};

class PicojsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class PicojsonTest : public TestBase {
public:
	PicojsonTest() : TestBase("PicoJSON (C++)") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        PicojsonParseResult* pr = new PicojsonParseResult;
        std::string err;
        parse(pr->v, json, json + length, &err);
    	if (!err.empty()) {
    		delete pr;
    		return 0;
    	}
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const PicojsonParseResult* pr = static_cast<const PicojsonParseResult*>(parseResult);
        PicojsonStringResult* sr = new PicojsonStringResult;
        sr->s = pr->v.serialize();
        return sr;
    }

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const PicojsonParseResult* pr = static_cast<const PicojsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->v);
        return true;
    }
};

REGISTER_TEST(PicojsonTest);
