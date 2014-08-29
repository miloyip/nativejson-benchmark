#include "../test.h"

#ifdef _WIN32
#define WIN32
#endif
#include "simplejson/src/JSON.cpp"
#include "simplejson/src/JSONValue.cpp"
#include <clocale>

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
    SimplejsonStringResult() : s() {}
    ~SimplejsonStringResult() { free(s); }

    virtual const char* c_str() const { return s; }

    char* s;
};
class SimplejsonTest : public TestBase {
public:
	SimplejsonTest() : TestBase("SimpleJSON (C++)") {
	}
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        SimplejsonParseResult* pr = new SimplejsonParseResult;

        char* backupLocale = std::setlocale(LC_ALL, 0);
        std::setlocale(LC_ALL, "en_US.UTF-8");
        
        pr->root = JSON::Parse(json); // Use mcbstowcs() internally

        std::setlocale(LC_ALL, backupLocale);

        if (!pr->root) {
            delete pr;
            pr = 0;
        }

    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const SimplejsonParseResult* pr = static_cast<const SimplejsonParseResult*>(parseResult);
        SimplejsonStringResult* sr = new SimplejsonStringResult;
        std::wstring s = JSON::Stringify(pr->root);

        // Doing reverse conversion as in JSON::Prase(const char*) with UTF8 locale

        char* backupLocale = std::setlocale(LC_ALL, 0);
        std::setlocale(LC_ALL, "en_US.UTF-8");

        size_t length = s.size() * 2 + 1;
        sr->s = (char*)malloc(length);
        if (wcstombs(sr->s, s.c_str(), length) == (size_t)-1) {
            delete sr;
            sr = 0;
        }

        std::setlocale(LC_ALL, backupLocale);

        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const SimplejsonParseResult* pr = static_cast<const SimplejsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, *pr->root);
        return true;
    }
#endif
};

REGISTER_TEST(SimplejsonTest);
