#include "../test.h"
#include "../cjsonlibs/centijson_renaming.h"
#include "centijson/src/json.h"
#include "centijson/src/json-dom.h"
#include "centijson/src/value.h"

#include <vector>

static void GenStat(Stat* s, VALUE* v) {
    switch (value_type(v)) {
    case VALUE_DICT:
        {
            size_t count = value_dict_size(v);

            const VALUE* keys[count];
            value_dict_keys_sorted(v, keys, count);

            for (size_t i = 0; i < count; i++) {
                const char* name = value_string(keys[i]);
                GenStat(s, value_dict_get(v, name));
                s->stringLength += value_string_length(keys[i]);
            }
            s->objectCount++;
            s->memberCount += count;
            s->stringCount += count;
        }
        break;

    case VALUE_ARRAY:
        {
            size_t count = value_array_size(v);
            for (size_t i = 0; i < count; i++)
                GenStat(s, value_array_get(v, i));
            s->arrayCount++;
            s->elementCount += count;
        }
        break;

    case VALUE_STRING:
        s->stringCount++;
        s->stringLength += value_string_length(v);
        break;

    case VALUE_INT32:
    case VALUE_UINT32:
    case VALUE_INT64:
    case VALUE_UINT64:
    case VALUE_FLOAT:
    case VALUE_DOUBLE:
        s->numberCount++;
        break;

    case VALUE_BOOL:
        if (value_bool(v))
            s->trueCount++;
        else
            s->falseCount++;
        break;

    case VALUE_NULL:
        s->nullCount++;
        break;
    }
}

class CentijsonParseResult : public ParseResultBase {
public:
    CentijsonParseResult()  { value_init_null(&root); }
    ~CentijsonParseResult() { value_fini(&root); }

    VALUE root;
};

class CentijsonStringResult : public StringResultBase {
public:
    CentijsonStringResult() {}
    ~CentijsonStringResult() {}
    virtual const char* c_str() const { return buffer.data(); }

    std::vector<char> buffer;

    void append(const char* str, size_t size)
        { buffer.insert(buffer.end(), str, str + size); }
    void finish()
        { buffer.insert(buffer.end(), '\0'); }
};

static int
write_callback(const char* str, size_t size, void* userdata)
{
    CentijsonStringResult* sr = (CentijsonStringResult*) userdata;
    sr->append(str, size);
    return 0;
}


/* Config to disable all limitations. */
static const JSON_CONFIG cfg = {
    0,  /* max_total_len */
    0,  /* max_total_values */
    0,  /* max_number_len */
    0,  /* max_string_len */
    0,  /* max_key_len */
    0,  /* max_nesting_level */
    0   /* flags */
};


class CentijsonTest : public TestBase {
public:
    CentijsonTest() {
    }
#if TEST_INFO
    virtual const char* GetName() const { return "Centijson (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        CentijsonParseResult* pr = new CentijsonParseResult;
        int err = json_dom_parse(json, length, &cfg, 0, &pr->root, NULL);
        if (err != 0) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const CentijsonParseResult* pr = static_cast<const CentijsonParseResult*>(parseResult);
        CentijsonStringResult* sr = new CentijsonStringResult;
        json_dom_dump(&pr->root, write_callback, sr, 0, JSON_DOM_DUMP_MINIMIZE);
        sr->finish();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const CentijsonParseResult* pr = static_cast<const CentijsonParseResult*>(parseResult);
        CentijsonStringResult* sr = new CentijsonStringResult;
        json_dom_dump(&pr->root, write_callback, sr, 0, 0);
        sr->finish();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const CentijsonParseResult* pr = static_cast<const CentijsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, (VALUE*) &pr->root);
        return true;
    }
#endif

#if TEST_SAXROUNDTRIP
    struct SaxContext {
        CentijsonStringResult* sr;
        std::vector<JSON_TYPE> stack;
        bool beginFlag;
    };

    static int
    sax_process_callback(JSON_TYPE type, const char* data, size_t size, void* userdata)
    {
        SaxContext* ctx = (SaxContext*) userdata;
        CentijsonStringResult* sr = ctx->sr;

        if(!ctx->beginFlag &&
            ((ctx->stack.back() == JSON_ARRAY_BEG && type != JSON_ARRAY_END && type != JSON_OBJECT_END) ||
             (ctx->stack.back() == JSON_OBJECT_BEG && type == JSON_KEY)))
        {
            write_callback(",", 1, sr);
        }

        switch(type) {
            case JSON_NULL:
                write_callback("null", 4, sr);
                break;

            case JSON_FALSE:
                write_callback("false", 5, sr);
                break;

            case JSON_TRUE:
                write_callback("true", 4, sr);
                break;

            case JSON_NUMBER:
                write_callback(data, size, sr);
                break;

            case JSON_STRING:
            case JSON_KEY:
                json_dump_string(data, size, write_callback, sr);
                if(type == JSON_KEY)
                    write_callback(":", 1, sr);
                break;

            case JSON_ARRAY_BEG:
            case JSON_OBJECT_BEG:
                write_callback(type == JSON_ARRAY_BEG ? "[" : "{", 1, sr);
                ctx->stack.push_back(type);
                break;

            case JSON_ARRAY_END:
            case JSON_OBJECT_END:
                write_callback(type == JSON_ARRAY_END ? "]" : "}", 1, sr);
                ctx->stack.pop_back();
                break;
        }

        ctx->beginFlag = (type == JSON_ARRAY_BEG || type == JSON_OBJECT_BEG);

        return 0;
    }

    virtual StringResultBase* SaxRoundtrip(const char* json, size_t length) const {
        static const JSON_CALLBACKS sax_callbacks = {
            sax_process_callback
        };

        SaxContext ctx;
        ctx.sr = new CentijsonStringResult;
        ctx.beginFlag = true;
        int err = json_parse(json, length, &sax_callbacks, &cfg, &ctx, NULL);
        if (err != 0) {
            delete ctx.sr;
            return 0;
        }
        ctx.sr->finish();
        return ctx.sr;
    }
#endif

#if TEST_SAXSTATISTICS
    struct SaxStatContext {
        Stat* stat;
        std::vector<JSON_TYPE> stack;
    };

    static int
    saxstat_process_callback(JSON_TYPE type, const char* data, size_t size, void* userdata)
    {
        (void) data;    // unused variable

        SaxStatContext* ctx = (SaxStatContext*) userdata;
        Stat* s = ctx->stat;

        if(!ctx->stack.empty()) {
            if(ctx->stack.back() == JSON_ARRAY_BEG && type != JSON_ARRAY_END)
                s->elementCount++;
            if(ctx->stack.back() == JSON_OBJECT_BEG && type != JSON_OBJECT_END && type != JSON_KEY)
                s->memberCount++;
        }

        switch(type) {
            case JSON_NULL:
                s->nullCount++;
                break;

            case JSON_FALSE:
                s->falseCount++;
                break;

            case JSON_TRUE:
                s->trueCount++;
                break;

            case JSON_NUMBER:
                s->numberCount++;
                break;

            case JSON_STRING:
            case JSON_KEY:
                s->stringCount++;
                s->stringLength += size;
                break;

            case JSON_ARRAY_BEG:
            case JSON_OBJECT_BEG:
                ctx->stack.push_back(type);
                if(type == JSON_ARRAY_BEG)
                    s->arrayCount++;
                else
                    s->objectCount++;
                break;

            case JSON_ARRAY_END:
            case JSON_OBJECT_END:
                ctx->stack.pop_back();
                break;
        }

        return 0;
    }

    virtual bool SaxStatistics(const char* json, size_t length, Stat* stat) const {
        static const JSON_CALLBACKS sax_callbacks = {
            saxstat_process_callback
        };

        SaxStatContext ctx;
        ctx.stat = stat;
        memset(ctx.stat, 0, sizeof(Stat));

        int err = json_parse(json, length, &sax_callbacks, &cfg, &ctx, NULL);
        return (err == 0);
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        CentijsonParseResult pr;
        int err = json_dom_parse(json, strlen(json), &cfg, 0, &pr.root, NULL);
        if (err == 0 &&
            value_type(&pr.root) == VALUE_ARRAY &&
            value_array_size(&pr.root) == 1 &&
            value_is_compatible(value_array_get(&pr.root, 0), VALUE_DOUBLE))
        {
            *d = value_double(value_array_get(&pr.root, 0));
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        CentijsonParseResult pr;
        int err = json_dom_parse(json, strlen(json), &cfg, 0, &pr.root, NULL);
        if (err == 0 &&
            value_type(&pr.root) == VALUE_ARRAY &&
            value_array_size(&pr.root) == 1 &&
            value_is_compatible(value_array_get(&pr.root, 0), VALUE_STRING))
        {
            const VALUE* str = value_array_get(&pr.root, 0);
            s = std::string(value_string(str), value_string_length(str));
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(CentijsonTest);
