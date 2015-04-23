#include "../test.h"

#include "jsoncpp/src/lib_json/json_reader.cpp"
#include "jsoncpp/src/lib_json/json_value.cpp"
#include "jsoncpp/src/lib_json/json_writer.cpp"

using namespace Json;

static void GenStat(Stat& stat, const Value& v) {
    switch (v.type()) {
    case arrayValue:
        for (ValueConstIterator itr = v.begin(); itr != v.end(); ++itr)
            GenStat(stat, *itr);
        stat.arrayCount++;
        stat.elementCount += v.size();
        break;

    case objectValue:
        for (ValueConstIterator itr = v.begin(); itr != v.end(); ++itr) {
            GenStat(stat, *itr);
            stat.stringLength += strlen(itr.memberName());
        }
        stat.objectCount++;
        stat.memberCount += v.size();
        stat.stringCount += v.size();   // member names
        break;

    case stringValue: 
        stat.stringCount++;
        stat.stringLength += v.asString().size();
        break;

    case intValue:
    case uintValue:
    case realValue:
        stat.numberCount++;
        break;

    case booleanValue:
        if (v.asBool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case nullValue:
        stat.nullCount++;
        break;
    }
}

class JsoncppParseResult : public ParseResultBase {
public:
    Value root;
};

class JsoncppStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};
class JsoncppTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "JsonCpp (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JsoncppParseResult* pr = new JsoncppParseResult;
        Reader reader;
        if (!reader.parse(json, pr->root)) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JsoncppParseResult* pr = static_cast<const JsoncppParseResult*>(parseResult);
        FastWriter writer;
        writer.omitEndingLineFeed();
        JsoncppStringResult* sr = new JsoncppStringResult;
        sr->s = writer.write(pr->root);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JsoncppParseResult* pr = static_cast<const JsoncppParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        Reader reader;
        Value root;
        if (reader.parse(json, root) && 
            root.isArray() &&
            root.size() == 1 &&
            root[0].isDouble())
        {
            *d = root[0].asDouble();
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, const char** s, size_t *length) const {
        Reader reader;
        Value root;
        if (reader.parse(json, root) && 
            root.isArray() &&
            root.size() == 1 &&
            root[0].isString())
        {
            *s = root[0].asString().c_str();
            *length = root[0].asString().size();
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(JsoncppTest);
