#if ((defined(_MSC_VER) && _MSC_VER >= 1700) || (__cplusplus >= 201103L))
#include "../test.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#include "dropbox-json11/json11.cpp"

using namespace json11;

static void GenStat(Stat& stat, const Json& v) {
    switch (v.type()) {
    case Json::ARRAY:
        for (auto const& i : v.array_items())
            GenStat(stat, i);
        stat.arrayCount++;
        stat.elementCount += v.array_items().size();
        break;

    case Json::OBJECT:
        for (auto const& i : v.object_items()) {
            GenStat(stat, i.second);
            stat.stringLength += i.first.size();
        }
        stat.objectCount++;
        stat.memberCount += v.object_items().size();
        stat.stringCount += v.object_items().size();
        break;

    case Json::STRING: 
        stat.stringCount++;
        stat.stringLength += v.string_value().size();
        break;

    case Json::NUMBER:
        stat.numberCount++;
        break;

    case Json::BOOL:
        if (v.bool_value())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case Json::NUL:
        stat.nullCount++;
        break;
    }
}

class Dropboxjson11ParseResult : public ParseResultBase {
public:
    Json root;
};

class Dropboxjson11StringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class Dropboxjson11Test : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "dropbox/json11 (C++11)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        Dropboxjson11ParseResult* pr = new Dropboxjson11ParseResult;
        std::string err;
        pr->root = Json::parse(json, err);
        if (!err.empty()) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const Dropboxjson11ParseResult* pr = static_cast<const Dropboxjson11ParseResult*>(parseResult);
        Dropboxjson11StringResult* sr = new Dropboxjson11StringResult;
        pr->root.dump(sr->s);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const Dropboxjson11ParseResult* pr = static_cast<const Dropboxjson11ParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        Json root;
        std::string err;
        root = Json::parse(json, err);
        if (err.empty() && root.is_array() && root.array_items().size() == 1 && root.array_items()[0].is_number()) {
            *d = root.array_items()[0].number_value();
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        Json root;
        std::string err;
        root = Json::parse(json, err);
        if (err.empty() && root.is_array() && root.array_items().size() == 1 && root.array_items()[0].is_string()) {
            s = root.array_items()[0].string_value();
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(Dropboxjson11Test);
#endif
