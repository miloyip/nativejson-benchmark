#include "../test.h"
#ifdef _MSC_VER
#pragma warning (disable:4127) // conditional expression is constant
#pragma warning (disable:4244) // conversion from 'int' to 'char', possible loss of data
#endif
#include "jsonxx/jsonxx.cc"

using namespace jsonxx;

void GenObjectStat(Stat& s, const Object& o);

void GenStat(Stat& s, const Value& v) {
    switch (v.type_) {
    case Value::OBJECT_:
        GenObjectStat(s, v.get<Object>());
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

void GenObjectStat(Stat& s, const Object& o) {
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

class JsonxxParseResult : public ParseResultBase {
public:
    Object o;
};

class JsonxxStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class JsonxxTest : public TestBase {
public:
    JsonxxTest() : TestBase("JSON++") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JsonxxParseResult* pr = new JsonxxParseResult;
        if (!pr->o.parse(json)) {
            delete pr;
            return 0;
        }
        return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JsonxxParseResult* pr = static_cast<const JsonxxParseResult*>(parseResult);
        JsonxxStringResult* sr = new JsonxxStringResult;
        sr->s = pr->o.json();
        return sr;
    }

    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const JsonxxParseResult* pr = static_cast<const JsonxxParseResult*>(parseResult);
        JsonxxStringResult* sr = new JsonxxStringResult;
        sr->s = pr->o.json();
        return sr;
    }

    virtual Stat Statistics(const ParseResultBase* parseResult) const {
        const JsonxxParseResult* pr = static_cast<const JsonxxParseResult*>(parseResult);
        Stat s;
        memset(&s, 0, sizeof(s));
        GenObjectStat(s, pr->o);
        return s;
    }
};

REGISTER_TEST(JsonxxTest);
