#include "../test.h"

#include "jsoncpp/src/lib_json/json_reader.cpp"
#include "jsoncpp/src/lib_json/json_value.cpp"
#include "jsoncpp/src/lib_json/json_writer.cpp"

using namespace Json;

void GenStat(Stat& stat, const Value& v) {
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
	JsoncppTest() : TestBase("JsonCpp") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JsoncppParseResult* pr = new JsoncppParseResult;
        Reader reader;
        reader.parse(json, pr->root);
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JsoncppParseResult* pr = static_cast<const JsoncppParseResult*>(parseResult);
        FastWriter writer;
        JsoncppStringResult* sr = new JsoncppStringResult;
        sr->s = writer.write(pr->root);
        return sr;
    }

    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const JsoncppParseResult* pr = static_cast<const JsoncppParseResult*>(parseResult);
        FastWriter writer;
        JsoncppStringResult* sr = new JsoncppStringResult;
        sr->s = writer.write(pr->root);
        return sr;
    }

    virtual Stat Statistics(const ParseResultBase* parseResult) const {
        const JsoncppParseResult* pr = static_cast<const JsoncppParseResult*>(parseResult);
        Stat s;
        memset(&s, 0, sizeof(s));
        GenStat(s, pr->root);
        return s;
    }
};

REGISTER_TEST(JsoncppTest);
