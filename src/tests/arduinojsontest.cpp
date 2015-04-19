#include "../test.h"

#include "ArduinoJson/src/DynamicJsonBuffer.cpp"
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
#include "ArduinoJson/test/Printers.cpp"
#include <string>
#include <sstream>

using namespace ArduinoJson;

static void GenStat(Stat& stat, const JsonVariant& v) {
    if (v.is<const JsonArray&>()) {
        const JsonArray& a = v.asArray();
        size_t size = a.size();
        for (size_t i = 0; i < size; i++)
            GenStat(stat, a[i]);
        stat.arrayCount++;
        stat.elementCount += size;
    }
    else if (v.is<const JsonObject&>()) {
        const JsonObject& o = v.asObject();
        for (JsonObject::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
            GenStat(stat, itr->value);
            stat.stringLength += strlen(itr->key);
        }
        stat.objectCount++;
        stat.memberCount += o.size();
        stat.stringCount += o.size();
    }
    else if (v.is<const char*>()) {
        if (v.asString()) {
            stat.stringCount++;
            stat.stringLength += strlen(v.asString());
        }
        else
            stat.nullCount++; // JSON null value is treat as string null pointer
    }
    else if (v.is<long>() || v.is<double>())
        stat.numberCount++;
    else if (v.is<bool>()) {
        if ((bool)v)
            stat.trueCount++;
        else
            stat.falseCount++;
    }
}

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

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const ArduinojsonParseResult* pr = static_cast<const ArduinojsonParseResult*>(parseResult);
        ArduinojsonStringResult* sr = new ArduinojsonStringResult;
        std::ostringstream os;
        StreamPrintAdapter adapter(os);
        pr->root->printTo(adapter);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const ArduinojsonParseResult* pr = static_cast<const ArduinojsonParseResult*>(parseResult);
        ArduinojsonStringResult* sr = new ArduinojsonStringResult;
        std::ostringstream os;
        StreamPrintAdapter adapter(os);
        pr->root->prettyPrintTo(adapter);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const ArduinojsonParseResult* pr = static_cast<const ArduinojsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        JsonVariant v;
        v.set(const_cast<JsonObject&>(*pr->root));
        GenStat(*stat, v);
        return true;
    }
#endif
};

REGISTER_TEST(ArduinojsonTest);
