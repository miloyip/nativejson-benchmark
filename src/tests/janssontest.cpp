#include "../test.h"

extern "C" {

#include "../cjsonlibs/jansson_renaming.h"
#include "jansson/src/jansson.h"

} // extern "C"

class JanssonParseResult : public ParseResultBase {
public:
    JanssonParseResult() : root() {}
    ~JanssonParseResult() { json_decref(root); }

    json_t *root;
};

class JanssonStringResult : public StringResultBase {
public:
    JanssonStringResult() : s() {}
    ~JanssonStringResult() { free(s); }

    virtual const char* c_str() const { return s; }

    char* s;
};

static void GenStat(Stat* s, json_t* v) {
    switch (json_typeof(v)) {
    case JSON_OBJECT:
        {
            const char *key;
            json_t *value;
            json_object_foreach(v, key, value) {
                GenStat(s, value);
                s->stringLength += strlen(key);
                s->stringCount++;
                s->memberCount++;
            }
            s->objectCount++;
        }
        break;

    case JSON_ARRAY:
        {
            size_t arraySize = json_array_size(v);
            for (size_t i = 0; i < arraySize; i++)
                GenStat(s, json_array_get(v, i));
            s->arrayCount++;
            s->elementCount += arraySize;
        }
        break;

    case JSON_STRING:
        s->stringCount++;
        s->stringLength += json_string_length(v);
        break;

    case JSON_REAL: 
    case JSON_INTEGER:
        s->numberCount++; 
        break;

    case JSON_TRUE: s->trueCount++; break;
    case JSON_FALSE: s->falseCount++; break;
    case JSON_NULL: s->nullCount++; break;
    }
}

class JanssonTest : public TestBase {
public:
    JanssonTest() {
#if USE_MEMORYSTAT
        json_set_alloc_funcs(MemoryStatMalloc, MemoryStatFree);
#endif
	}

#if TEST_INFO
    virtual const char* GetName() const { return "Jansson (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JanssonParseResult* pr = new JanssonParseResult;
        json_error_t error;
        pr->root = json_loads(json, 0, &error);
        if (!pr->root) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JanssonParseResult* pr = static_cast<const JanssonParseResult*>(parseResult);
        JanssonStringResult* sr = new JanssonStringResult;
        sr->s = json_dumps(pr->root, JSON_COMPACT);
        if (!sr->s) {
            delete sr;
            return 0;
        }
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const JanssonParseResult* pr = static_cast<const JanssonParseResult*>(parseResult);
        JanssonStringResult* sr = new JanssonStringResult;
        sr->s = json_dumps(pr->root, JSON_INDENT(4));
        if (!sr->s) {
            delete sr;
            return 0;
        }
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JanssonParseResult* pr = static_cast<const JanssonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        JanssonParseResult pr;
        json_error_t error;
        pr.root = json_loads(json, 0, &error);
        if (pr.root && 
            json_is_array(pr.root) && 
            json_array_size(pr.root) == 1 &&
            json_is_real(json_array_get(pr.root, 0)))
        {
            *d = json_real_value(json_array_get(pr.root, 0));
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        JanssonParseResult pr;
        json_error_t error;
        pr.root = json_loads(json, JSON_ALLOW_NUL, &error);
        if (pr.root && 
            json_is_array(pr.root) && 
            json_array_size(pr.root) == 1 &&
            json_is_string(json_array_get(pr.root, 0)))
        {
            s = std::string(
                json_string_value(json_array_get(pr.root, 0)),
                json_string_length(json_array_get(pr.root, 0)));
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(JanssonTest);
