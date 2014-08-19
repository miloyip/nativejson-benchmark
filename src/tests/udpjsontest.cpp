#include "../test.h"
#include <cstdio>

extern "C" {

#include "udp-json-parser/json.h"
#include "udp-json-builder/json-builder.h"

} // extern "C"

static void GenStat(Stat* s, const json_value* v) {
    switch (v->type) {
    case json_object:
        for (size_t i = 0; i < v->u.object.length; i++) {
            const json_object_entry* e = v->u.object.values + i;
            GenStat(s, e->value);
            s->stringLength += e->name_length;
        }
        s->stringCount += v->u.object.length;
        s->memberCount += v->u.object.length;
        s->objectCount++;
        break;

    case json_array:
        for (size_t i = 0; i < v->u.array.length; i++)
            GenStat(s, v->u.array.values[i]);
        s->arrayCount++;
        s->elementCount += v->u.array.length;
        break;

    case json_string:
        s->stringCount++;
        s->stringLength += v->u.string.length;
        break;

    case json_integer:
    case json_double:
        s->numberCount++; 
        break;

    case json_boolean:
        if (v->u.boolean)
            s->trueCount++;
        else
            s->falseCount++;
        break;

    case json_null:
        s->nullCount++;
        break;
    }
}

class UdpjsonParseResult : public ParseResultBase {
public:
    UdpjsonParseResult() : root() {}
    ~UdpjsonParseResult() { json_value_free(root); }

    json_value *root;
};

class UdpjsonStringResult : public StringResultBase {
public:
    UdpjsonStringResult() : s() {}
    ~UdpjsonStringResult() { free(s); }

    virtual const char* c_str() const { return s; }

    char* s;
};

class UdpjsonTest : public TestBase {
public:
    UdpjsonTest() : TestBase("udp/json-parser|builder") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        UdpjsonParseResult* pr = new UdpjsonParseResult;
        json_settings settings = {};
        settings.value_extra = json_builder_extra;  /* space for json-builder state */
        char error[128];
        pr->root = json_parse_ex(&settings, json, length, error);
        if (!pr->root) {
            delete pr;
            return 0;
        }
    	return pr;
    }

    // Very slow in the current version.
#if 0
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const UdpjsonParseResult* pr = static_cast<const UdpjsonParseResult*>(parseResult);
        UdpjsonStringResult* sr = new UdpjsonStringResult;
        json_serialize_opts opts = { json_serialize_mode_packed, 0, 0 };
        sr->s = (char*)malloc(json_measure_ex(pr->root, opts));
        json_serialize_ex(sr->s, pr->root, opts);
        return sr;
    }

    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const UdpjsonParseResult* pr = static_cast<const UdpjsonParseResult*>(parseResult);
        UdpjsonStringResult* sr = new UdpjsonStringResult;
        json_serialize_opts opts = { json_serialize_mode_multiline, 0, 4 };
        sr->s = (char*)malloc(json_measure_ex(pr->root, opts));
        json_serialize_ex(sr->s, pr->root, opts);
        return sr;
    }
#endif

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const UdpjsonParseResult* pr = static_cast<const UdpjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
};

REGISTER_TEST(UdpjsonTest);
