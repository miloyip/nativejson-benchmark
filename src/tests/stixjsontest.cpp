#include "../test.h"

#define JSMN_HUGE_FILES
#include "stix-json/JsonParser.cpp"

static void GenStat(Stat& stat, const JsonValue& v) {
    switch (v.GetType()) {
    case JSMN_ARRAY:
        for (size_t i = 0; i < v.GetElementsCount(); i++)
            GenStat(stat, v[i]);
        stat.arrayCount++;
        stat.elementCount += v.GetElementsCount();
        break;

    case JSMN_OBJECT:
        for (size_t i = 0; i < v.GetElementsCount(); i++) {
            const JsonValue key = v[i];
            JsonString keyStr = key.AsString();
            stat.stringLength += keyStr.GetLength();
            GenStat(stat, key.GetValue());
        }
        stat.objectCount++;
        stat.memberCount += v.GetElementsCount();
        stat.stringCount += v.GetElementsCount();   // Key
        break;

    case JSMN_STRING:
        stat.stringCount++;
        stat.stringLength += v.AsString().GetLength();
        break;

    case JSMN_PRIMITIVE:
        if (v.IsNull())
            stat.nullCount++;
        else {
            // There is no IsTrue()/IsFalse(), use its internal representation
            JsonString s = v.AsString();
            if (s.IsEqualTo("t"))
                stat.trueCount++;
            else if (s.IsEqualTo("f"))
                stat.falseCount++;
            else {
                v.AsNumber();    // read it to parse number here. To prevent lazy parsing.
                stat.numberCount++;
            }
        }
        break;
    }
}

class StixjsonParseResult : public ParseResultBase {
public:
    JsonParser parser;
};

class StixjsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const {
        return s.c_str();
    }

    std::string s;
};

class StixjsonTest : public TestBase {
public:
	StixjsonTest() : TestBase("StiX Json") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        StixjsonParseResult* pr = new StixjsonParseResult;
        jsmnerr_t ret = pr->parser.ParseJsonString(json);
        if (ret != JSMN_SUCCESS) {
            printf("Stix Json parse error: %d\n", ret);
            delete pr;
            return 0;
        }
    	return pr;
    }

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const StixjsonParseResult* pr = static_cast<const StixjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->parser.GetRoot());
        return true;
    }
};

REGISTER_TEST(StixjsonTest);
