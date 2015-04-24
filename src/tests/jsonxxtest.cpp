#include "../test.h"
#ifdef _MSC_VER
#pragma warning (disable:4127) // conditional expression is constant
#pragma warning (disable:4244) // conversion from 'int' to 'char', possible loss of data
#endif
#include "jsonxx/jsonxx.cc"

using namespace jsonxx;

static void GenStat(Stat& s, const Value& v) {
    switch (v.type_) {
    case Value::OBJECT_:
        {
            const Object& o = v.get<Object>();
            Object::container::const_iterator itr = o.kv_map().begin();
            Object::container::const_iterator end = o.kv_map().end();
            for (; itr != end; ++itr) {
                s.stringLength += itr->first.size(); // key
                GenStat(s, *itr->second);
            }
            s.objectCount++;
            s.memberCount += o.size();
            s.stringCount += o.size();  // Key
        }
        break;

    case Value::ARRAY_:
        {
            const Array& a = v.get<Array>();
            Array::container::const_iterator itr = a.values().begin();
            Array::container::const_iterator end = a.values().end();
            for (; itr != end; ++itr)
                GenStat(s, **itr);
            s.arrayCount++;
            s.elementCount += a.size();
        }
        break;

    case Value::STRING_:
        s.stringCount++;
        s.stringLength += v.get<String>().size();
        break;

    case Value::NUMBER_:
        s.numberCount++;
        break;

    case Value::BOOL_:
        if (v.get<bool>())
            s.trueCount++;
        else
            s.falseCount++;
        break;

    case Value::NULL_:
        s.nullCount++;
        break;

    case Value::INVALID_:
        assert(false);
    }
}

class JsonxxParseResult : public ParseResultBase {
public:
    Value v;
};

class JsonxxStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class JsonxxTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "hjiang/JSON++ (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JsonxxParseResult* pr = new JsonxxParseResult;
        if (!pr->v.parse(json)) {
            delete pr;
            return 0;
        }
        return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JsonxxParseResult* pr = static_cast<const JsonxxParseResult*>(parseResult);
        JsonxxStringResult* sr = new JsonxxStringResult;
        sr->s = pr->v.is<Object>() ? pr->v.get<Object>().json() : pr->v.get<Array>().json();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JsonxxParseResult* pr = static_cast<const JsonxxParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->v);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        Value v;
        if (v.parse(json) && v.is<Array>() && v.get<Array>().size() == 1) {
            *d = (double)v.get<Array>().get<Number>(0);
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        Value v;
        if (v.parse(json) && v.is<Array>() && v.get<Array>().size() == 1) {
            s = v.get<Array>().get<String>(0);
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(JsonxxTest);
