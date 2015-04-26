#include "../test.h"

#if (!defined(_MSC_VER) || _MSC_VER >= 1800) && !(defined(__CYGWIN__) && defined(__i386__)) // Before VC2013 do not support C99 variable declaration, and crash in cygwin 32-bit

#include "json-c/json.h"

static void GenStat(Stat* s, json_object* v) {
    switch (json_object_get_type(v)) {
    case json_type_object:
        {
            json_object_object_foreach(v, key, val) {
                GenStat(s, val);
                s->stringCount++;
                s->stringLength += strlen(key);
                s->memberCount++;
            }
            s->objectCount++;
        }
        break;

    case json_type_array:
        for (int i = 0; i < json_object_array_length(v); i++)
            GenStat(s, json_object_array_get_idx(v, i));
        s->elementCount += json_object_array_length(v);
        s->arrayCount++;
        break;

    case json_type_string:
        s->stringCount++;
        s->stringLength += json_object_get_string_len(v);
        break;

    case json_type_int:
    case json_type_double:
        s->numberCount++;
        break;

    case json_type_boolean:
        if (json_object_get_boolean(v))
            s->trueCount++;
        else
            s->falseCount++;
        break;

        break;

    case json_type_null:
        s->nullCount++;
        break;
    }
}

class JsoncParseResult : public ParseResultBase {
public:
    JsoncParseResult() : root() {}
    ~JsoncParseResult() { json_object_put(root); }

    json_object *root;
};

class JsoncStringResult : public StringResultBase {
public:
    JsoncStringResult() : s() {}
    ~JsoncStringResult() { free(s); }

    virtual const char* c_str() const { return s; }

    char* s;
};

class JsoncTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "json-c (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JsoncParseResult* pr = new JsoncParseResult;
        pr->root = json_tokener_parse(json);
        if (!pr->root) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JsoncParseResult* pr = static_cast<const JsoncParseResult*>(parseResult);
        JsoncStringResult* sr = new JsoncStringResult;
        sr->s = StrDup(json_object_to_json_string_ext(pr->root, JSON_C_TO_STRING_PLAIN));
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const JsoncParseResult* pr = static_cast<const JsoncParseResult*>(parseResult);
        JsoncStringResult* sr = new JsoncStringResult;
        sr->s = StrDup(json_object_to_json_string_ext(pr->root, JSON_C_TO_STRING_PRETTY));
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JsoncParseResult* pr = static_cast<const JsoncParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, (json_object*)pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        JsoncParseResult pr;
        pr.root = json_tokener_parse(json);
        if (pr.root && 
            json_object_get_type(pr.root) == json_type_array &&
            json_object_array_length(pr.root) == 1 &&
            json_object_get_type(json_object_array_get_idx(pr.root, 0)) == json_type_double) 
        {
            *d = json_object_get_double(json_object_array_get_idx(pr.root, 0));
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        JsoncParseResult pr;
        pr.root = json_tokener_parse(json);
        if (pr.root && 
            json_object_get_type(pr.root) == json_type_array &&
            json_object_array_length(pr.root) == 1 &&
            json_object_get_type(json_object_array_get_idx(pr.root, 0)) == json_type_string) 
        {
            s = std::string(
                json_object_get_string(json_object_array_get_idx(pr.root, 0)),
                json_object_get_string_len(json_object_array_get_idx(pr.root, 0)));
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(JsoncTest);

#endif