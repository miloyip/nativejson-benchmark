#include "../test.h"

extern "C" {

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
    JanssonTest() : TestBase("Jansson") {
#ifdef USE_MEMORYSTAT
        json_set_alloc_funcs(MemoryStatMalloc, MemoryStatFree);
#endif
	}
	
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

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JanssonParseResult* pr = static_cast<const JanssonParseResult*>(parseResult);
        JanssonStringResult* sr = new JanssonStringResult;
        sr->s = json_dumps(pr->root, 0);
        if (!sr->s) {
            delete sr;
            return 0;
        }
        return sr;
    }

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

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JanssonParseResult* pr = static_cast<const JanssonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
};

REGISTER_TEST(JanssonTest);
