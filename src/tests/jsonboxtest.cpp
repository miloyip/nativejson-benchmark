#include "../test.h"

#include "jsonbox/src/Array.cpp"
#include "jsonbox/src/Convert.cpp"
#include "jsonbox/src/Escaper.cpp"
#include "jsonbox/src/IndentCanceller.cpp"
#include "jsonbox/src/Indenter.cpp"
#include "jsonbox/src/Object.cpp"
#include "jsonbox/src/SolidusEscaper.cpp"
#include "jsonbox/src/Value.cpp"

using namespace JsonBox;

static void GenStat(Stat& stat, const Value& v) {
    switch (v.getType()) {
    case Value::ARRAY:
        {
            const Array& a = v.getArray();
            for (Array::const_iterator itr = a.begin(); itr != a.end(); ++itr)
                GenStat(stat, *itr);
            stat.arrayCount++;
            stat.elementCount += a.size();
        }
        break;

    case Value::OBJECT:
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

    case Value::STRING: 
        stat.stringCount++;
        stat.stringLength += v.getString().size();
        break;

    case Value::INTEGER:
    case Value::DOUBLE:
        stat.numberCount++;
        break;

    case Value::BOOLEAN:
        if (v.getBoolean())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case Value::NULL_VALUE:
        stat.nullCount++;
        break;
    }
}

class JsonboxParseResult : public ParseResultBase {
public:
    Value root;
};

class JsonboxStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};
class JsonboxTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "JsonBox (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JsonboxParseResult* pr = new JsonboxParseResult;
        pr->root.loadFromString(json);
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JsonboxParseResult* pr = static_cast<const JsonboxParseResult*>(parseResult);
        JsonboxStringResult* sr = new JsonboxStringResult;
        std::ostringstream os;
        pr->root.writeToStream(os, false, false);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const JsonboxParseResult* pr = static_cast<const JsonboxParseResult*>(parseResult);
        JsonboxStringResult* sr = new JsonboxStringResult;
        std::ostringstream os;
        pr->root.writeToStream(os, true, false);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JsonboxParseResult* pr = static_cast<const JsonboxParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif
};

REGISTER_TEST(JsonboxTest);
