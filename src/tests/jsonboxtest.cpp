#include "../test.h"

#if !defined(_MSC_VER) // runtime crash in Visual Studio

#include "jsonbox/src/Convert.cpp"
#include "jsonbox/src/Escaper.cpp"
#include "jsonbox/src/IndentCanceller.cpp"
#include "jsonbox/src/Indenter.cpp"
#include "jsonbox/src/JsonParsingError.cpp"
#include "jsonbox/src/JsonWritingError.cpp"
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
        try {
            pr->root.loadFromString(json);
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

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        try {
            Value root;
            root.loadFromString(json);
            *d = root.getArray()[0].getDouble();
            return true;
        }
        catch (...) {
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        try {
            Value root;
            root.loadFromString(json);
            s = root.getArray()[0].getString();
            return true;
        }
        catch (...) {
        }
        return false;
    }
#endif
};

REGISTER_TEST(JsonboxTest);

#endif
