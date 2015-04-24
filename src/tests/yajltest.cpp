#include "../test.h"

extern "C" {
#include "yajl/yajl_common.h"
#undef YAJL_MAX_DEPTH 
#define YAJL_MAX_DEPTH 1024
#include "yajl/yajl_gen.h"
#include "yajl/yajl_parse.h"
#include "yajl/yajl_tree.h"

yajl_gen_status GenVal(yajl_gen g, yajl_val v) {
    yajl_gen_status status;
    switch (v->type) {
    case yajl_t_string: return yajl_gen_string(g, (unsigned char*)v->u.string, strlen(v->u.string));

    case yajl_t_number: 
        {
            char buffer[100];
            char *num = buffer;
            size_t len;
            if (YAJL_IS_INTEGER(v))
                return yajl_gen_integer(g, YAJL_GET_INTEGER(v));
            else if (YAJL_IS_DOUBLE(v))
                return yajl_gen_double(g, YAJL_GET_DOUBLE(v));
            else {
                num = YAJL_GET_NUMBER(v);
                len = strlen(buffer);
                return yajl_gen_number(g, num, len);
            }
        }

    case yajl_t_object:
        status = yajl_gen_map_open(g);
        if (status != yajl_gen_status_ok)
            return status;
        
        for (size_t i = 0; i < v->u.object.len; i++) {
            status = yajl_gen_string(g, (unsigned char *)v->u.object.keys[i], strlen(v->u.object.keys[i]));
            if (status != yajl_gen_status_ok)
                return status;
            status = GenVal(g, v->u.object.values[i]);
            if (status != yajl_gen_status_ok)
                return status;
        }
        return yajl_gen_map_close(g);

    case yajl_t_array:
        status = yajl_gen_array_open(g);
        if (status != yajl_gen_status_ok)
            return status;
        
        for (size_t i = 0; i < v->u.array.len; i++) {
            status = GenVal(g, v->u.array.values[i]);
            if (status != yajl_gen_status_ok)
                return status;
        }

        return yajl_gen_array_close(g);

    case yajl_t_true: return yajl_gen_bool(g, 1);
    case yajl_t_false: return yajl_gen_bool(g, 0);
    case yajl_t_null: return yajl_gen_null(g);
    default:
        return yajl_gen_in_error_state;
    }
}

static void GenStat(Stat* s, yajl_val v) {
    switch (v->type) {
    case yajl_t_string: s->stringCount++;  s->stringLength += strlen(v->u.string); break;
    case yajl_t_number: s->numberCount++;  break;
    case yajl_t_object: 
        for (size_t i = 0; i < v->u.object.len; i++) {
            s->stringCount++;
            s->stringLength += strlen(v->u.object.keys[i]);
            GenStat(s, v->u.object.values[i]);
        }
        s->objectCount++;
        s->memberCount += v->u.object.len;
        break;

    case yajl_t_array:
        for (size_t i = 0; i < v->u.array.len; i++)
            GenStat(s, v->u.array.values[i]);
        s->arrayCount++;
        s->elementCount += v->u.array.len;
        break;

    case yajl_t_true: s->trueCount++; break;
    case yajl_t_false: s->falseCount++; break;
    case yajl_t_null: s->nullCount++; break;
    default:;
    }
}

#define GEN_AND_RETURN(func)                      \
{                                                 \
    yajl_gen_status __stat = func;                \
    return __stat == yajl_gen_status_ok; }

static int reformat_null(void * ctx) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_null(g));
}

static int reformat_boolean(void * ctx, int boolean) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_bool(g, boolean));
}

static int reformat_integer(void * ctx, long long integerVal) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_integer(g, integerVal));
}

static int reformat_double(void * ctx, double doubleVal) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_double(g, doubleVal));
}

static int reformat_string(void * ctx, const unsigned char * stringVal, size_t stringLen) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_string(g, stringVal, stringLen));
}

static int reformat_map_key(void * ctx, const unsigned char * stringVal, size_t stringLen) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_string(g, stringVal, stringLen));
}

static int reformat_start_map(void * ctx) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_map_open(g));
}

static int reformat_end_map(void * ctx) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_map_close(g));
}

static int reformat_start_array(void * ctx) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_array_open(g));
}

static int reformat_end_array(void * ctx) {
    yajl_gen g = (yajl_gen)ctx;
    GEN_AND_RETURN(yajl_gen_array_close(g));
}

static yajl_callbacks callbacks = {
    reformat_null,
    reformat_boolean,
    reformat_integer,
    reformat_double,
    NULL,
    reformat_string,
    reformat_start_map,
    reformat_map_key,
    reformat_end_map,
    reformat_start_array,
    reformat_end_array
};

struct StatContext {
    Stat* stat;
    bool after_key;
};

#define HANDLE_ELEMENTCOUNT() \
    if (c->after_key) \
        c->after_key = false;\
    else \
        stat->elementCount++

static int stat_null(void * ctx) {
    StatContext* c = (StatContext*)ctx;
    Stat* stat = c->stat;
    stat->nullCount++;
    HANDLE_ELEMENTCOUNT();
    return 1;
}

static int stat_boolean(void * ctx, int boolean) {
    StatContext* c = (StatContext*)ctx;
    Stat* stat = c->stat;
    if (boolean)
        stat->trueCount++;
    else
        stat->falseCount++;
    HANDLE_ELEMENTCOUNT();
    return 1;
}

static int stat_integer(void * ctx, long long integerVal) {
    (void)integerVal;
    StatContext* c = (StatContext*)ctx;
    Stat* stat = c->stat;
    stat->numberCount++;
    HANDLE_ELEMENTCOUNT();
    return 1;
}

static int stat_double(void * ctx, double doubleVal) {
    (void)doubleVal;
    StatContext* c = (StatContext*)ctx;
    Stat* stat = c->stat;
    stat->numberCount++;
    HANDLE_ELEMENTCOUNT();
    return 1;
}

static int stat_string(void * ctx, const unsigned char * stringVal, size_t stringLen) {
    (void)stringVal;
    StatContext* c = (StatContext*)ctx;
    Stat* stat = c->stat;
    stat->stringCount++;
    stat->stringLength += stringLen;
    HANDLE_ELEMENTCOUNT();
    return 1;
}

static int stat_map_key(void * ctx, const unsigned char * stringVal, size_t stringLen) {
    (void)stringVal;
    StatContext* c = (StatContext*)ctx; 
    Stat* stat = c->stat;
    stat->memberCount++;
    stat->stringCount++;
    stat->stringLength += stringLen;
    c->after_key = true;
    return 1;
}

static int stat_start_map(void * ctx) {
    StatContext* c = (StatContext*)ctx;
    Stat* stat = c->stat;
    HANDLE_ELEMENTCOUNT();
    return 1;
}

static int stat_end_map(void * ctx) {
    StatContext* c = (StatContext*)ctx;
    Stat* stat = c->stat;
    stat->objectCount++;
    return 1;
}

static int stat_start_array(void * ctx) {
    StatContext* c = (StatContext*)ctx;
    Stat* stat = c->stat;
    HANDLE_ELEMENTCOUNT();
    return 1;
}

static int stat_end_array(void * ctx) {
    StatContext* c = (StatContext*)ctx;
    Stat* stat = c->stat;
    stat->arrayCount++;
    return 1;
}

static yajl_callbacks statCallbacks = {
    stat_null,
    stat_boolean,
    stat_integer,
    stat_double,
    NULL,
    stat_string,
    stat_start_map,
    stat_map_key,
    stat_end_map,
    stat_start_array,
    stat_end_array
};

} // extern "C"

class YajlParseResult : public ParseResultBase {
public:
    YajlParseResult() : root() {}
    ~YajlParseResult() { yajl_tree_free(root); }

    yajl_val root;
};

class YajlStringResult : public StringResultBase {
public:
    YajlStringResult() : g(), s() {}
    ~YajlStringResult() { yajl_gen_free(g); }

    virtual const char* c_str() const { return (const char*)s; }

    yajl_gen g;
    const unsigned char* s;
};

class YajlTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "YAJL (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        YajlParseResult* pr = new YajlParseResult;
        pr->root = yajl_tree_parse(json, NULL, 0);
        if (!pr->root) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const YajlParseResult* pr = static_cast<const YajlParseResult*>(parseResult);
        YajlStringResult* sr = new YajlStringResult;
        sr->g = yajl_gen_alloc(NULL);
        yajl_gen_status status = GenVal(sr->g, pr->root);
        if (status != yajl_gen_status_ok) {
            delete sr;
            return 0;
        }

        size_t len;
        status = yajl_gen_get_buf(sr->g, &sr->s, &len);

        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const YajlParseResult* pr = static_cast<const YajlParseResult*>(parseResult);
        YajlStringResult* sr = new YajlStringResult;
        sr->g = yajl_gen_alloc(NULL);
        yajl_gen_config(sr->g, yajl_gen_beautify, 1);
        yajl_gen_config(sr->g, yajl_gen_indent_string, "    ");

        yajl_gen_status status = GenVal(sr->g, pr->root);
        if (status != yajl_gen_status_ok) {
            delete sr;
            return 0;
        }

        size_t len;
        status = yajl_gen_get_buf(sr->g, &sr->s, &len);

        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const YajlParseResult* pr = static_cast<const YajlParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
#endif

#if TEST_SAXROUNDTRIP
    virtual StringResultBase* SaxRoundtrip(const char* json, size_t length) const {
        // https://github.com/lloyd/yajl/blob/master/reformatter/json_reformat.c
        (void)length;
        YajlStringResult* sr = new YajlStringResult;
        sr->g = yajl_gen_alloc(NULL);
        yajl_handle hand = yajl_alloc(&callbacks, NULL, (void *)sr->g);
        if (yajl_parse(hand, (const unsigned char*)json, length) != yajl_status_ok || 
            yajl_complete_parse(hand) != yajl_status_ok) {
            delete sr;
            sr = 0;
        }
        else {
            size_t len;
            yajl_gen_get_buf(sr->g, &sr->s, &len);
            yajl_gen_clear(sr->g);
        }
        yajl_free(hand);
        return sr;
    }
#endif

#if TEST_SAXSTATISTICS
    virtual bool SaxStatistics(const char* json, size_t length, Stat* stat) const {
        (void)length;
        memset(stat, 0, sizeof(Stat));

        StatContext context;
        context.stat = stat;
        context.after_key = true;
        yajl_handle hand = yajl_alloc(&statCallbacks, NULL, &context);
        bool ret = true;
        if (yajl_parse(hand, (const unsigned char*)json, length) != yajl_status_ok ||
            yajl_complete_parse(hand) != yajl_status_ok) {
            ret = false;
        }
        yajl_free(hand);
        return ret;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        YajlParseResult pr;
        pr.root = yajl_tree_parse(json, NULL, 0);
        if (YAJL_IS_ARRAY(pr.root) && 
            YAJL_GET_ARRAY(pr.root)->len == 1 && 
            YAJL_IS_DOUBLE(YAJL_GET_ARRAY(pr.root)->values[0]))
        {
            *d = YAJL_GET_DOUBLE(YAJL_GET_ARRAY(pr.root)->values[0]);
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        YajlParseResult pr;
        pr.root = yajl_tree_parse(json, NULL, 0);
        if (YAJL_IS_ARRAY(pr.root) && 
            YAJL_GET_ARRAY(pr.root)->len == 1 && 
            YAJL_IS_STRING(YAJL_GET_ARRAY(pr.root)->values[0]))
        {
            s = YAJL_GET_STRING(YAJL_GET_ARRAY(pr.root)->values[0]);
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(YajlTest);
