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
        size_t size = 0;
        for (JsonArray::const_iterator itr = a.begin(); itr != a.end(); ++itr)
            GenStat(stat, *itr);
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
    ArduinojsonParseResult() : buffer() {}
    ~ArduinojsonParseResult() { free(buffer); }

    char* buffer;
    DynamicJsonBuffer jsonBuffer;
    JsonVariant root;
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

        // Determine object or array
        for (size_t i = 0; i < length; i++) {
            switch (json[i]) {
                case '{':
                    {
                        JsonObject& o = pr->jsonBuffer.parseObject(pr->buffer);
                        if (!o.success()) {
                            delete pr;
                            return 0;
                        }
                        pr->root.set(o);
                    }
                    return pr;
                case '[':
                    {
                        JsonArray& a = pr->jsonBuffer.parseArray(pr->buffer);
                        if (!a.success()) {
                            delete pr;
                            return 0;
                        }
                        pr->root.set(a);
                    }
                    return pr;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    continue;
            }
            // Unknown first non-whitespace character
            break;
        }
        delete pr;
        return 0;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const ArduinojsonParseResult* pr = static_cast<const ArduinojsonParseResult*>(parseResult);
        ArduinojsonStringResult* sr = new ArduinojsonStringResult;
        std::ostringstream os;
        StreamPrintAdapter adapter(os);
        pr->root.printTo(adapter);
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
        pr->root.prettyPrintTo(adapter);
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

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        ArduinojsonParseResult pr;
        pr.buffer = strdup(json);
        JsonArray& a = pr.jsonBuffer.parseArray(pr.buffer);
        if (a.success() && a.size() == 1) {
            *d = (double)a[0];
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        ArduinojsonParseResult pr;
        pr.buffer = strdup(json);
        JsonArray& a = pr.jsonBuffer.parseArray(pr.buffer);
        if (a.success() && a.size() == 1) {
            s = a[0].asString();
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(ArduinojsonTest);
