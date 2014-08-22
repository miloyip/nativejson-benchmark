#include "../test.h"

#ifdef _WIN32
#define WIN32
#endif
#include "simplejson/src/JSON.cpp"
#include "simplejson/src/JSONValue.cpp"

static void GenStat(Stat& stat, const JSONValue& v) {
    if (v.IsArray()) {
        const JSONArray& a = v.AsArray();
        for (JSONArray::const_iterator itr = a.begin(); itr != a.end(); ++itr)
            GenStat(stat, **itr);
        stat.arrayCount++;
        stat.elementCount += a.size();
    }
    else if (v.IsObject()) {
        const JSONObject& o = v.AsObject();
        for (JSONObject::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
            GenStat(stat, *itr->second);
            stat.stringLength += itr->first.size();
        }
        stat.objectCount++;
        stat.memberCount += o.size();
        stat.stringCount += o.size();
    }
    else if (v.IsString()) {
        stat.stringCount++;
        stat.stringLength += v.AsString().size();
    }
    else if (v.IsNumber())
        stat.numberCount++;
    else if (v.IsBool()) {
        if (v.AsBool())
            stat.trueCount++;
        else
            stat.falseCount++;
    }
    else if (v.IsNull())
        stat.nullCount++;
}

class SimplejsonParseResult : public ParseResultBase {
public:
    SimplejsonParseResult() : root() {}
    ~SimplejsonParseResult() { delete root; }

    JSONValue* root;
};

class SimplejsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return (const char*)s.c_str(); }

    std::wstring s;
};
class SimplejsonTest : public TestBase {
public:
	SimplejsonTest() : TestBase("SimpleJSON (C++)") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        SimplejsonParseResult* pr = new SimplejsonParseResult;
        pr->root = JSON::Parse(json);
        if (!pr->root) {
            delete pr;
            pr = 0;
        }
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const SimplejsonParseResult* pr = static_cast<const SimplejsonParseResult*>(parseResult);
        SimplejsonStringResult* sr = new SimplejsonStringResult;
        sr->s = JSON::Stringify(pr->root);
        return sr;
    }

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const SimplejsonParseResult* pr = static_cast<const SimplejsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, *pr->root);
        return true;
    }
};

REGISTER_TEST(SimplejsonTest);
