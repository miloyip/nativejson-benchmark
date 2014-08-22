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

class ParsonTest : public TestBase {
public:
    ParsonTest() : TestBase("Parson (C)") {
	}
	
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

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const ParsonParseResult* pr = static_cast<const ParsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, (JSON_Value*)pr->root);
        return true;
    }
};

REGISTER_TEST(ParsonTest);
