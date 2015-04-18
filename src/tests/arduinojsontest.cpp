#include "../test.h"

#include "ArduinoJson/src/JsonArray.cpp"
#include "ArduinoJson/src/JsonBuffer.cpp"
#include "ArduinoJson/src/JsonObject.cpp"
#include "ArduinoJson/src/JsonVariant.cpp"
#include "ArduinoJson/src/Arduino/Print.cpp"
#include "ArduinoJson/src/internals/IndentedPrint.cpp"
#include "ArduinoJson/src/internals/JsonParser.cpp"
#include "ArduinoJson/src/internals/List.cpp"
#include "ArduinoJson/src/internals/Prettyfier.cpp"
#include "ArduinoJson/src/internals/QuotedString.cpp"
#include "ArduinoJson/src/internals/StringBuilder.cpp"
#include "ArduinoJson/DynamicJsonBuffer.hpp"
#include <string>

using namespace ArduinoJson;
/*
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
*/
class ArduinojsonParseResult : public ParseResultBase {
public:
    ArduinojsonParseResult() : buffer(), root() {}
    ~ArduinojsonParseResult() { free(buffer); }

    char* buffer;
    DynamicJsonBuffer jsonBuffer;
    JsonObject* root;
};

class ArduinojsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class ArduinojsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "ArduinoJson (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        ArduinojsonParseResult* pr = new ArduinojsonParseResult;
        pr->buffer = (char*)malloc(length);
        memcpy(pr->buffer, json, length);
        pr->root = &pr->jsonBuffer.parseObject(pr->buffer);
    	return pr;
    }
#endif
/*
#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const ArduinojsonParseResult* pr = static_cast<const ArduinojsonParseResult*>(parseResult);
        ArduinojsonStringResult* sr = new ArduinojsonStringResult;
        std::ostringstream os;
        pr->root.writeToStream(os, false, false);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const ArduinojsonParseResult* pr = static_cast<const ArduinojsonParseResult*>(parseResult);
        ArduinojsonStringResult* sr = new ArduinojsonStringResult;
        std::ostringstream os;
        pr->root.writeToStream(os, true, false);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const ArduinojsonParseResult* pr = static_cast<const ArduinojsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif
*/
};

REGISTER_TEST(ArduinojsonTest);
