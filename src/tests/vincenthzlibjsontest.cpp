#include "../test.h"
#include "../cjsonlibs/vincenthz_libjson_renaming.h"
#include "vincenthz-libjson/json.h"
#include <stdio.h>

#ifdef _MSC_VER
#define strtoll _strtoi64
#endif

extern "C" {

struct json_val_elem {
    char *key;
    uint32_t key_length;
    struct json_val *val;
};

typedef struct json_val {
    int type;
    int length;
    union {
        char *str_val;
        double float_val;
        long long int_val;
        struct json_val **array;
        struct json_val_elem **object;
    } u;
} json_val_t;

static void *tree_create_structure(int nesting, int is_object)
{
    (void)nesting;
    json_val_t *v = (json_val_t *)malloc(sizeof(json_val_t));
    if (v) {
        /* instead of defining a new enum type, we abuse the
        * meaning of the json enum type for array and object */
        if (is_object) {
            v->type = JSON_OBJECT_BEGIN;
            v->u.object = NULL;
        }
        else {
            v->type = JSON_ARRAY_BEGIN;
            v->u.array = NULL;
        }
        v->length = 0;
    }
    return v;
}

static char *memalloc_copy_length(const char *src, uint32_t n)
{
    char *dest;

    dest = (char*)calloc(n + 1, sizeof(char));
    if (dest)
        memcpy(dest, src, n);
    return dest;
}

static void *tree_create_data(int type, const char *data, uint32_t length)
{
    json_val_t *v;

    v = (json_val_t *)malloc(sizeof(json_val_t));
    if (v) {
        v->type = type;
        v->length = length;
        switch (type) {
        case JSON_STRING:
            v->u.str_val = memalloc_copy_length(data, length);
            if (!v->u.str_val) {
                free(v);
                return NULL;
            }
            break;

        case JSON_INT:
            v->u.int_val = strtoll(data, NULL, 10);
            break;

        case JSON_FLOAT:
            v->u.float_val = atof(data);
            break;
        }
    }
    return v;
}

static int tree_append(void *structure, char *key, uint32_t key_length, void *obj)
{
    json_val_t *parent = (json_val_t *)structure;
    if (key) {
        struct json_val_elem *objelem;

        if (parent->length == 0) {
            parent->u.object = (json_val_elem **)calloc(1 + 1, sizeof(json_val_t *)); /* +1 for null */
            if (!parent->u.object)
                return 1;
        }
        else {
            uint32_t newsize = parent->length + 1 + 1; /* +1 for null */
            void *newptr;

            newptr = realloc(parent->u.object, newsize * sizeof(json_val_t *));
            if (!newptr)
                return -1;
            parent->u.object = (json_val_elem **)newptr;
        }

        objelem = (json_val_elem *)malloc(sizeof(struct json_val_elem));
        if (!objelem)
            return -1;

        objelem->key = memalloc_copy_length(key, key_length);
        objelem->key_length = key_length;
        objelem->val = (json_val_t *)obj;
        parent->u.object[parent->length++] = objelem;
        parent->u.object[parent->length] = NULL;
    }
    else {
        if (parent->length == 0) {
            parent->u.array = (json_val_t **)calloc(1 + 1, sizeof(json_val_t *)); /* +1 for null */
            if (!parent->u.array)
                return 1;
        }
        else {
            uint32_t newsize = parent->length + 1 + 1; /* +1 for null */
            void *newptr;

            newptr = realloc(parent->u.object, newsize * sizeof(json_val_t *));
            if (!newptr)
                return -1;
            parent->u.array = (json_val_t **)newptr;
        }
        parent->u.array[parent->length++] = (json_val_t *)obj;
        parent->u.array[parent->length] = NULL;
    }
    return 0;
}

static void tree_free(json_val_t * v) {
    switch (v->type) {
    case JSON_OBJECT_BEGIN:
        for (int i = 0; i < v->length; i++) {
            tree_free(v->u.object[i]->val);
            free(v->u.object[i]->key);
            free(v->u.object[i]);
        }
        free(v->u.object);
        break;

    case JSON_ARRAY_BEGIN:
        for (int i = 0; i < v->length; i++)
            tree_free(v->u.array[i]);
        free(v->u.array);
        break;

    case JSON_STRING:
        free(v->u.str_val);
        break;
    }
    free(v);
}

// print_func is either json_print_pretty or json_print_raw
typedef int (*json_print_function)(json_printer *, int, const char *, uint32_t);

static void tree_print(json_printer* printer, json_print_function print_func, const json_val_t * v) {
    switch (v->type) {
    case JSON_OBJECT_BEGIN:
        json_print_raw(printer, JSON_OBJECT_BEGIN, NULL, NULL);
        for (int i = 0; i < v->length; i++) {
            json_print_raw(printer, JSON_KEY, v->u.object[i]->key, v->u.object[i]->key_length);
            tree_print(printer, print_func, v->u.object[i]->val);
        }
        json_print_raw(printer, JSON_OBJECT_END, NULL, NULL);
        break;

    case JSON_ARRAY_BEGIN:
        json_print_raw(printer, JSON_ARRAY_BEGIN, NULL, NULL);
        for (int i = 0; i < v->length; i++)
            tree_print(printer, print_func, v->u.array[i]);
        json_print_raw(printer, JSON_ARRAY_END, NULL, NULL);
        break;

    case JSON_STRING:
        json_print_raw(printer, JSON_STRING, v->u.str_val, (uint32_t)strlen(v->u.str_val));
        break;

    case JSON_INT:
        {
            char buffer[32];
            int length = sprintf(buffer, "%lld", v->u.int_val);
            json_print_raw(printer, JSON_INT, buffer, (uint32_t)length);
        }        
        break;

    case JSON_FLOAT:
        {
            char buffer[32];
            int length = sprintf(buffer, "%.17g", v->u.float_val);
            json_print_raw(printer, JSON_FLOAT, buffer, (uint32_t)length);
        }        
        break;

    case JSON_TRUE:
        json_print_raw(printer, JSON_TRUE, "true", 4);
        break;

    case JSON_FALSE:
        json_print_raw(printer, JSON_STRING, "false", 5);
        break;

    case JSON_NULL:
        json_print_raw(printer, JSON_STRING, "null", 4);
        break;
    }
}

static void GenStat(Stat* s, const json_val_t * v) {
    switch (v->type) {
    case JSON_OBJECT_BEGIN:
        for (int i = 0; i < v->length; i++) {
            GenStat(s, v->u.object[i]->val);
            s->stringLength += v->u.object[i]->key_length;
        }
        s->objectCount++;
        s->memberCount += v->length;
        s->stringCount += v->length;
        break;

    case JSON_ARRAY_BEGIN:
        for (int i = 0; i < v->length; i++)
            GenStat(s, v->u.array[i]);
        s->arrayCount++;
        s->elementCount += v->length;
        break;

    case JSON_STRING:
        s->stringCount++;
        s->stringLength += strlen(v->u.str_val);
        break;

    case JSON_INT:
    case JSON_FLOAT:
        s->numberCount++;
        break;

    case JSON_TRUE:
        s->trueCount++;
        break;

    case JSON_FALSE:
        s->falseCount++;
        break;

    case JSON_NULL:
        s->nullCount++;
        break;
    }
}

struct string_buffer {
    char* buffer;
    size_t capacity;
    size_t length;
};

int string_buffer_append(void *userdata, const char *s, uint32_t length) {
    string_buffer* sb = (string_buffer*)userdata;
    if (sb->length + length > sb->capacity) {
        if (sb->capacity == 0)
            sb->capacity = 1024;

        do {
            sb->capacity *= 2;
        } while (sb->length + length > sb->capacity);

        sb->buffer = (char*)realloc(sb->buffer, sb->capacity);
    }
    memcpy(sb->buffer + sb->length, s, length);
    sb->length += length;
    return 0;
}

struct StatContext {
    Stat* stat;
    bool after_key;
};


#define HANDLE_ELEMENTCOUNT() \
    if (c->after_key) \
        c->after_key = false;\
    else \
        s->elementCount++

static int stat_callback(StatContext* c, int type, const char *data, uint32_t length) {
    (void)data;

    Stat* s = c->stat;
    switch (type) {
    case JSON_OBJECT_BEGIN:
        s->objectCount++;
        HANDLE_ELEMENTCOUNT();
        break;

    case JSON_KEY:
        s->memberCount++;
        s->stringCount++;
        s->stringLength += length;
        c->after_key = true;
        break;

    case JSON_ARRAY_BEGIN:
        s->arrayCount++;
        HANDLE_ELEMENTCOUNT();
        break;

    case JSON_STRING:
        s->stringCount++;
        s->stringLength += length;
        HANDLE_ELEMENTCOUNT();
        break;

    case JSON_INT:
    case JSON_FLOAT:
        s->numberCount++;
        HANDLE_ELEMENTCOUNT();
        break;

    case JSON_TRUE:
        s->trueCount++;
        HANDLE_ELEMENTCOUNT();
        break;

    case JSON_FALSE:
        s->falseCount++;
        HANDLE_ELEMENTCOUNT();
        break;

    case JSON_NULL:
        s->nullCount++;
        HANDLE_ELEMENTCOUNT();
        break;
    }

    return 0;
}

} // extern "C"

class VinenthzParseResult : public ParseResultBase {
public:
    VinenthzParseResult() : root() {}
    ~VinenthzParseResult() { if (root) tree_free(root); }

    json_val_t *root;
};

class VinenthzStringResult : public StringResultBase {
public:
    VinenthzStringResult() : sb() {}
    ~VinenthzStringResult() { free(sb.buffer); }

    virtual const char* c_str() const { return sb.buffer; }

    void AppendEnds() {
        if (sb.buffer) {
            //assert(sb.length < sb.capacity); // Must have avaliable space
            sb.buffer[sb.length] = '\0';
        }
    }

    string_buffer sb;
};

class VinenthzTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Vinenthz/libjson (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        VinenthzParseResult* pr = new VinenthzParseResult;

        json_config config;
        memset(&config, 0, sizeof(json_config));
        config.max_nesting = 0;
        config.max_data = 0;
        config.allow_c_comments = 0;
        config.allow_yaml_comments = 0;

        json_parser parser;
        json_parser_dom dom;
        json_parser_dom_init(&dom, tree_create_structure, tree_create_data, tree_append);
        json_parser_init(&parser, &config, json_parser_dom_callback, &dom);
        uint32_t processed;
        if (!json_parser_string(&parser, json, (uint32_t)length, &processed) && json_parser_is_done(&parser))
            pr->root = (json_val_t*)dom.root_structure;
        else {
            delete pr;
            pr = 0;
        }
        json_parser_free(&parser);
        json_parser_dom_free(&dom);
        return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const VinenthzParseResult* pr = static_cast<const VinenthzParseResult*>(parseResult);
        VinenthzStringResult* sr = new VinenthzStringResult();
        json_printer printer;
        json_print_init(&printer, string_buffer_append, &sr->sb);
        tree_print(&printer, json_print_raw, pr->root);
        json_print_free(&printer);
        sr->AppendEnds();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const VinenthzParseResult* pr = static_cast<const VinenthzParseResult*>(parseResult);
        VinenthzStringResult* sr = new VinenthzStringResult();
        json_printer printer;
        json_print_init(&printer, string_buffer_append, &sr->sb);
        tree_print(&printer, json_print_pretty, pr->root);
        json_print_free(&printer);
        sr->AppendEnds();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const VinenthzParseResult* pr = static_cast<const VinenthzParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
#endif

#if TEST_SAXROUNDTRIP
    virtual StringResultBase* SaxRoundtrip(const char* json, size_t length) const {
        VinenthzStringResult* sr = new VinenthzStringResult();
        json_printer printer;
        json_print_init(&printer, string_buffer_append, &sr->sb);

        json_config config;
        memset(&config, 0, sizeof(json_config));
        config.max_nesting = 0;
        config.max_data = 0;
        config.allow_c_comments = 0;
        config.allow_yaml_comments = 0;

        json_parser parser;
        json_parser_init(&parser, &config, (json_parser_callback)json_print_raw, &printer);

        uint32_t processed;
        if (json_parser_string(&parser, json, (uint32_t)length, &processed) == 0)
            sr->AppendEnds();
        else {
            delete sr;
            sr = 0;
        }

        json_parser_free(&parser);
        json_print_free(&printer);
        return sr;
    }
#endif

#if TEST_SAXSTATISTICS
    virtual bool SaxStatistics(const char* json, size_t length, Stat* stat) const {
        json_config config;
        memset(&config, 0, sizeof(json_config));
        config.max_nesting = 0;
        config.max_data = 0;
        config.allow_c_comments = 0;
        config.allow_yaml_comments = 0;

        memset(stat, 0, sizeof(Stat));
        StatContext context;
        context.stat = stat;
        context.after_key = true;

        json_parser parser;
        json_parser_init(&parser, &config, (json_parser_callback)stat_callback, &context);

        uint32_t processed;
        bool ret = (json_parser_string(&parser, json, (uint32_t)length, &processed) == 0);

        json_parser_free(&parser);
        return ret;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        VinenthzParseResult* pr = static_cast<VinenthzParseResult*>(Parse(json, strlen(json)));
        bool ret = false;
        if (pr && 
            pr->root->type == JSON_ARRAY_BEGIN &&
            pr->root->length == 1)
        {
            if (pr->root->u.array[0]->type == JSON_FLOAT) {
                *d = pr->root->u.array[0]->u.float_val;
                ret = true;
            }
            else if (pr->root->u.array[0]->type == JSON_INT) {
                *d = (double)pr->root->u.array[0]->u.int_val;
                ret = true;
            }
        }
        delete pr;
        return ret;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        VinenthzParseResult* pr = static_cast<VinenthzParseResult*>(Parse(json, strlen(json)));
        bool ret = false;
        if (pr && 
            pr->root->type == JSON_ARRAY_BEGIN &&
            pr->root->length == 1 &&
            pr->root->u.array[0]->type == JSON_STRING)
        {
            s = std::string(
                pr->root->u.array[0]->u.str_val, 
                pr->root->u.array[0]->length);
            ret = true;
        }
        delete pr;
        return ret;
    }
#endif
};

REGISTER_TEST(VinenthzTest);
