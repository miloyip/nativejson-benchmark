#include "../test.h"

#if !defined(__GNUC__) || defined(__clang__) // gcc crash in Travis https://github.com/udp/json-builder/issues/7

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

    default:
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
#if TEST_INFO
    virtual const char* GetName() const { return "udp/json-parser (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        UdpjsonParseResult* pr = new UdpjsonParseResult;
        json_settings settings = json_settings();
        settings.value_extra = json_builder_extra;  /* space for json-builder state */
        char error[128];
        pr->root = json_parse_ex(&settings, json, length, error);
        if (!pr->root) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

    // Very slow in the current version.
#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const UdpjsonParseResult* pr = static_cast<const UdpjsonParseResult*>(parseResult);
        UdpjsonStringResult* sr = new UdpjsonStringResult;
        json_serialize_opts opts = { json_serialize_mode_packed, 0, 0 };
        sr->s = (char*)malloc(json_measure_ex(pr->root, opts));
        json_serialize_ex(sr->s, pr->root, opts);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const UdpjsonParseResult* pr = static_cast<const UdpjsonParseResult*>(parseResult);
        UdpjsonStringResult* sr = new UdpjsonStringResult;
        json_serialize_opts opts = { json_serialize_mode_multiline, 0, 4 };
        sr->s = (char*)malloc(json_measure_ex(pr->root, opts));
        json_serialize_ex(sr->s, pr->root, opts);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const UdpjsonParseResult* pr = static_cast<const UdpjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        UdpjsonParseResult pr;
        json_settings settings = json_settings();
        settings.value_extra = json_builder_extra;  /* space for json-builder state */
        char error[128];
        pr.root = json_parse_ex(&settings, json, strlen(json), error);
        if (pr.root &&
            pr.root->type == json_array &&
            pr.root->u.array.length == 1 &&
            pr.root->u.array.values[0]->type == json_double)
        {
            *d = pr.root->u.array.values[0]->u.dbl;
            return true;
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        UdpjsonParseResult pr;
        json_settings settings = json_settings();
        settings.value_extra = json_builder_extra;  /* space for json-builder state */
        char error[128];
        pr.root = json_parse_ex(&settings, json, strlen(json), error);
        if (pr.root &&
            pr.root->type == json_array &&
            pr.root->u.array.length == 1 &&
            pr.root->u.array.values[0]->type == json_string)
        {
            s = std::string(
                pr.root->u.array.values[0]->u.string.ptr,
                pr.root->u.array.values[0]->u.string.length);
            return true;
        }
        return false;
    }
#endif
};

REGISTER_TEST(UdpjsonTest);

#endif
