#include "../test.h"

#include "ujson/ujson/ujson.cpp"
#include "ujson/ujson/double-conversion.cc"

using namespace ujson;

static void GenStat(Stat& stat, const value& v) {
    switch (v.type()) {
    case value_type::array:
        {
            const array& a = array_cast(v);
            for (array::const_iterator itr = a.begin(); itr != a.end(); ++itr)
                GenStat(stat, *itr);
            stat.arrayCount++;
            stat.elementCount += a.size();
        }
        break;

    case value_type::object:
        {
            const object& o = object_cast(v);
            for (object::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
                GenStat(stat, itr->second);
                stat.stringLength += itr->first.size();
            }
            stat.objectCount++;
            stat.memberCount += o.size();
            stat.stringCount += o.size();   // member names
        }
        break;

    case value_type::string: 
        stat.stringCount++;
        stat.stringLength += string_cast(v).length();
        break;

    case value_type::number:
        stat.numberCount++;
        break;

    case value_type::boolean:
        if (bool_cast(v))
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case value_type::null:
        stat.nullCount++;
        break;
    }
}

class UjsonParseResult : public ParseResultBase {
public:
    value root;
};

class UjsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};
class UjsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "ujson (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        UjsonParseResult* pr = new UjsonParseResult;
        try {
            pr->root = parse(json, length);
        }
        catch (const ::ujson::exception&) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const UjsonParseResult* pr = static_cast<const UjsonParseResult*>(parseResult);
        UjsonStringResult* sr = new UjsonStringResult;
        sr->s = to_string(pr->root, compact_utf8);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const UjsonParseResult* pr = static_cast<const UjsonParseResult*>(parseResult);
        UjsonStringResult* sr = new UjsonStringResult;
        sr->s = to_string(pr->root, indented_utf8);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const UjsonParseResult* pr = static_cast<const UjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        try {
            value root = parse(json);
            *d = double_cast(array_cast(root)[0]);
            return true;
        }
        catch (const ::ujson::exception&) {
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        try {
            value root = parse(json);
            s = string_cast(array_cast(root)[0]);
            return true;
        }
        catch (const ::ujson::exception&) {
        }
        return false;
    }
#endif
};

REGISTER_TEST(UjsonTest);
