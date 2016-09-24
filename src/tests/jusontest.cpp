#include "../test.h"
#include "juson/juson.h"

#include <cassert>


static void GenStat(Stat* s, const juson_value_t* v) {
    switch (v->t) {
    case JUSON_OBJECT:
        for (juson_value_t* child = v->head; child != 0; child = child->next) {
            GenStat(s, child);
            ++s->memberCount;
        }
        ++s->objectCount;
        break;

    case JUSON_ARRAY:
        for (int i = 0; i < v->size; ++i)
            GenStat(s, v->adata[i]);
        s->elementCount += v->size;
        ++s->arrayCount;
        break;

    case JUSON_STRING:
        ++s->stringCount;
        s->stringLength += v->len;
        break;

    case JUSON_INTEGER:
    case JUSON_FLOAT:
        ++s->numberCount; 
        break;

    case JUSON_BOOL:
        v->bval ? ++s->trueCount: ++s->falseCount;
        break;

    case JUSON_NULL:
        ++s->nullCount;
        break;
    
    case JUSON_PAIR:
        GenStat(s, v->val);
        ++s->stringCount;
        s->stringLength += v->key->len;
        break;
    
    default:
        assert(false);
    }
}

class JusonParseResult : public ParseResultBase {
public:
    JusonParseResult() : doc() {}
    ~JusonParseResult() { juson_destroy(&doc); }

    juson_doc_t doc;
};

class JusonStringResult : public StringResultBase {
public:
    JusonStringResult() : s() {}
    ~JusonStringResult() { free(s); }

    virtual const char* c_str() const { return s; }

    char* s;
};

class JusonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "juson (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JusonParseResult* pr = new JusonParseResult;
        if (!juson_parse(&pr->doc, json)) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif


#ifdef TEST_STRINGIFY
#   undef TEST_STRINGIFY
#endif

#define TEST_STRINGIFY 0
#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JusonParseResult* pr = static_cast<const JusonParseResult*>(parseResult);
        JusonStringResult* sr = new JusonStringResult;
        sr->s = cJSON_PrintUnformatted(pr->root);
        return sr;
    }
#endif

#ifdef TEST_PRETTIFY
#   undef TEST_PRETTIFY
#endif

#define TEST_PRETTIFY 0 
#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const JusonParseResult* pr = static_cast<const JusonParseResult*>(parseResult);
        JusonStringResult* sr = new JusonStringResult;
        sr->s = cJSON_Print(pr->root);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JusonParseResult* pr = static_cast<const JusonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->doc.val);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        JusonParseResult pr;
        juson_value_t* root = juson_parse(&pr.doc, json);
        if (root && root->t == JUSON_ARRAY && root->size &&
            (root->adata[0]->t == JUSON_FLOAT || root->adata[0]->t == JUSON_INTEGER)) {
            if (root->adata[0]->t == JUSON_FLOAT)
                *d = root->adata[0]->fval;
            else
                *d = root->adata[0]->ival;
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        JusonParseResult pr;
        juson_value_t* root = juson_parse(&pr.doc, json);
        if (root && root->t == JUSON_ARRAY && root->size && root->adata[0]->t == JUSON_STRING) {
            s = std::string(root->adata[0]->sdata, root->adata[0]->len);
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(JusonTest);