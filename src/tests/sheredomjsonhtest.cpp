#include "../test.h"
#include "../cjsonlibs/sheredom_jsonh_renaming.h"
#include "sheredom-jsonh/json.h"

static void GenStat(Stat* s, const json_value_s* v) {
    switch (v->type) {
    case json_type_object:
        {
            json_object_s* o = (json_object_s*)v->payload;
            for (json_object_element_s* child = o->start; child != 0; child = child->next) {
                GenStat(s, child->value);
                s->stringCount++;
                s->stringLength += child->name->string_size;
                s->memberCount++;
            }
            s->objectCount++;
        }
        break;

    case json_type_array:
        {
            json_array_s* a = (json_array_s*)v->payload;
            for (json_array_element_s* child = a->start; child != 0; child = child->next) {
                GenStat(s, child->value);
                s->elementCount++;
            }
            s->arrayCount++;
        }
        break;

    case json_type_string:
        s->stringCount++;
        s->stringLength += ((json_string_s*)v->payload)->string_size;
        break;

    case json_type_number:
        s->numberCount++; 
        break;

    case json_type_true:
        s->trueCount++;
        break;

    case json_type_false:
        s->falseCount++;
        break;

    case json_type_null:
        s->nullCount++;
        break;
    }
}

static void ParseNumbers(const json_value_s* v) {
    switch (v->type) {
    case json_type_array:
        {
            json_array_s* a = (json_array_s*)v->payload;
            for (json_array_element_s* child = a->start; child != 0; child = child->next)
                ParseNumbers(child->value);
        }
        break;

    case json_type_object:
        {
            json_object_s* o = (json_object_s*)v->payload;
            for (json_object_element_s* child = o->start; child != 0; child = child->next)
                ParseNumbers(child->value);
        }
        break;

    case json_type_number:
        {
            json_number_s* n = (json_number_s*)v->payload;
            atof(n->number);
        }
        break;

    default:;
    }
}

static void WriteNumbers(const json_value_s* v) {
    switch (v->type) {
    case json_type_array:
        {
            json_array_s* a = (json_array_s*)v->payload;
            for (json_array_element_s* child = a->start; child != 0; child = child->next)
                WriteNumbers(child->value);
        }
        break;

    case json_type_object:
        {
            json_object_s* o = (json_object_s*)v->payload;
            for (json_object_element_s* child = o->start; child != 0; child = child->next)
                WriteNumbers(child->value);
        }
        break;

    case json_type_number:
        {
            double d;
            memcpy(&d, v->payload, sizeof(double));
            char buffer[32] = { 0 };
            sprintf(buffer, "%17g", d);
        }
        break;

    default:;
    }
}

class SheredomParseResult : public ParseResultBase {
public:
    SheredomParseResult() : root() {}
    ~SheredomParseResult() { free(root); }

    json_value_s *root;
};

class SheredomStringResult : public StringResultBase {
public:
    SheredomStringResult() : s() {}
    ~SheredomStringResult() { free(s); }

    virtual const char* c_str() const { return s; }

    char* s;
};

class SheredomTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Sheredom json.h (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        SheredomParseResult* pr = new SheredomParseResult;
        pr->root = sheredom_json_parse(json, length);
        if (!pr->root) {
            delete pr;
            return 0;
        }
        // Since json.h does not parse numbers, emulate here in order to compare with other parsers.
        ParseNumbers(pr->root);
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const SheredomParseResult* pr = static_cast<const SheredomParseResult*>(parseResult);
        SheredomStringResult* sr = new SheredomStringResult;
        sr->s = static_cast<char*>(sheredom_json_write_minified(pr->root, 0));
        // Since json.h does not write numbers, emulate here in order to compare with other parsers.
        WriteNumbers(pr->root);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const SheredomParseResult* pr = static_cast<const SheredomParseResult*>(parseResult);
        SheredomStringResult* sr = new SheredomStringResult;
        sr->s = static_cast<char*>(sheredom_json_write_pretty(pr->root, "    ", "\n", 0));
        // Since json.h does not write numbers, emulate here in order to compare with other parsers.
        WriteNumbers(pr->root);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const SheredomParseResult* pr = static_cast<const SheredomParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        SheredomParseResult pr;
        pr.root = sheredom_json_parse(json, strlen(json));
        if (pr.root && pr.root->type == json_type_array) {
            json_array_s* a = (json_array_s*)pr.root->payload;
            if (a->length == 1 && a->start->value->type == json_type_number) {
                *d = atof(((json_number_s*)(a->start->value->payload))->number);
                return true;
            }
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        SheredomParseResult pr;
        pr.root = sheredom_json_parse(json, strlen(json));
        if (pr.root && pr.root->type == json_type_array) {
            json_array_s* a = (json_array_s*)pr.root->payload;
            if (a->length == 1 && a->start->value->type == json_type_string) {
                s = ((json_string_s*)(a->start->value->payload))->string;
                return true;
            }
        }
        return false;
    }
#endif
};

REGISTER_TEST(SheredomTest);
