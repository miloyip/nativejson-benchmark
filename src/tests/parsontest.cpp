#include "../test.h"
#include "../cjsonlibs/parson_renaming.h"
#include "parson/parson.h"

static void GenStat(Stat* s, JSON_Value* v) {
    switch (json_value_get_type(v)) {
    case JSONObject:
        {
            JSON_Object* o = json_value_get_object(v);
            size_t count = json_object_get_count(o);

            for (size_t i = 0; i < count; i++) {
                const char* name = json_object_get_name(o, i);
                GenStat(s, json_object_get_value(o, name));
                s->stringLength += strlen(name);
            }
            s->objectCount++;
            s->memberCount += count;
            s->stringCount += count;
        }
        break;

    case JSONArray:
        {
            JSON_Array* a = json_value_get_array(v);
            size_t count = json_array_get_count(a);
            for (int i = 0; i < count; i++)
                GenStat(s, json_array_get_value(a, i));
            s->arrayCount++;
            s->elementCount += count;
        }
        break;

    case JSONString:
        s->stringCount++;
        s->stringLength += strlen(json_value_get_string(v));
        break;

    case JSONNumber:
        s->numberCount++;
        break;

    case JSONBoolean:
        if (json_value_get_boolean(v))
            s->trueCount++;
        else
            s->falseCount++;
        break;

    case JSONNull:
        s->nullCount++;
        break;
    }
}

class ParsonParseResult : public ParseResultBase {
public:
    ParsonParseResult() : root() {}
    ~ParsonParseResult() { json_value_free(root); }

    JSON_Value *root;
};

class ParsonStringResult : public StringResultBase {
public:
    ParsonStringResult() : s() {}
    ~ParsonStringResult() { free(s); }
    virtual const char* c_str() const { return s; }
    
    char* s;
};

class ParsonTest : public TestBase {
public:
    ParsonTest() {
        json_set_allocation_functions(malloc, free);
    }
#if TEST_INFO
    virtual const char* GetName() const { return "Parson (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        ParsonParseResult* pr = new ParsonParseResult;
        pr->root = json_parse_string(json);
        if (!pr->root) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const ParsonParseResult* pr = static_cast<const ParsonParseResult*>(parseResult);
        ParsonStringResult* sr = new ParsonStringResult;
        sr->s = json_serialize_to_string(pr->root);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const ParsonParseResult* pr = static_cast<const ParsonParseResult*>(parseResult);
        ParsonStringResult* sr = new ParsonStringResult;
        sr->s = json_serialize_to_string(pr->root);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const ParsonParseResult* pr = static_cast<const ParsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, (JSON_Value*)pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        ParsonParseResult pr;
        pr.root = json_parse_string(json);
        if (pr.root &&
            json_value_get_type(pr.root) == JSONArray &&
            json_array_get_count(json_value_get_array(pr.root)) == 1 &&
            json_value_get_type(json_array_get_value(json_value_get_array(pr.root), 0)) == JSONNumber)
        {
            *d = json_value_get_number(json_array_get_value(json_value_get_array(pr.root), 0));
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        ParsonParseResult pr;
        pr.root = json_parse_string(json);
        if (pr.root &&
            json_value_get_type(pr.root) == JSONArray &&
            json_array_get_count(json_value_get_array(pr.root)) == 1 &&
            json_value_get_type(json_array_get_value(json_value_get_array(pr.root), 0)) == JSONString)
        {
            s = json_value_get_string(json_array_get_value(json_value_get_array(pr.root), 0));
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(ParsonTest);
