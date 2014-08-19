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

    virtual const char* c_str() const { return s; }

    yajl_gen g;
    const char* s;
};

class YajlTest : public TestBase {
public:
	YajlTest() : TestBase("YAJL") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        YajlParseResult* pr = new YajlParseResult;
        pr->root = yajl_tree_parse(json, NULL, 0);
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const YajlParseResult* pr = static_cast<const YajlParseResult*>(parseResult);
        YajlStringResult* sr = new YajlStringResult;
        sr->g = yajl_gen_alloc(NULL);
        yajl_gen_status status = GenVal(sr->g, pr->root);
        if (status != yajl_gen_status_ok) {
            printf("yajl Strinify error %d\n", status);
            delete sr;
            return 0;
        }

        size_t len;
        status = yajl_gen_get_buf(sr->g, (const unsigned char**)&sr->s, &len);

        return sr;
    }

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
        status = yajl_gen_get_buf(sr->g, (const unsigned char**)&sr->s, &len);

        return sr;
    }

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const YajlParseResult* pr = static_cast<const YajlParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
};

REGISTER_TEST(YajlTest);
