#include "../test.h"
#include "cJSON/cJSON.h"

static void GenStat(Stat* s, const cJSON* v) {
    if (cJSON_IsObject(v)) {
        for (cJSON* child = v->child; child != 0; child = child->next) {
            GenStat(s, child);
            s->stringCount++;
            s->stringLength += strlen(child->string);
            s->memberCount++;
        }
        s->objectCount++;
    } else if (cJSON_IsArray(v)) {
        for (cJSON* child = v->child; child != 0; child = child->next) {
            GenStat(s, child);
            s->elementCount++;
        }
        s->arrayCount++;
    } else if (cJSON_IsString(v)) {
        s->stringCount++;
        s->stringLength += strlen(v->valuestring);
    } else if (cJSON_IsNumber(v)) {
        s->numberCount++;
    } else if (cJSON_IsTrue(v)) {
        s->trueCount++;
    } else if (cJSON_IsFalse(v)) {
        s->falseCount++;
    } else if (cJSON_IsNull(v)) {
        s->nullCount++;
    }
}

class CjsonParseResult : public ParseResultBase {
public:
    CjsonParseResult() : root() {}
    ~CjsonParseResult() { cJSON_Delete(root); }

    cJSON *root;
};

class CjsonStringResult : public StringResultBase {
public:
    CjsonStringResult() : s() {}
    ~CjsonStringResult() { free(s); }

    virtual const char* c_str() const { return s; }

    char* s;
};

class CjsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "cJSON (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        CjsonParseResult* pr = new CjsonParseResult;
        pr->root = cJSON_ParseWithOpts(json, nullptr, static_cast<cJSON_bool>(true));
        if (pr->root == nullptr) {
            delete pr;
            return nullptr;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const CjsonParseResult* pr = static_cast<const CjsonParseResult*>(parseResult);
        CjsonStringResult* sr = new CjsonStringResult;
        sr->s = cJSON_PrintBuffered(pr->root, 4096, static_cast<cJSON_bool>(false));
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const CjsonParseResult* pr = static_cast<const CjsonParseResult*>(parseResult);
        CjsonStringResult* sr = new CjsonStringResult;
        sr->s = cJSON_PrintBuffered(pr->root, 4096, static_cast<cJSON_bool>(true));
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const CjsonParseResult* pr = static_cast<const CjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        CjsonParseResult pr;
        pr.root = cJSON_Parse(json);
        if ((pr.root != nullptr) && cJSON_IsArray(pr.root) && cJSON_IsNumber(pr.root->child)) {
            *d = pr.root->child->valuedouble;
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        CjsonParseResult pr;
        pr.root = cJSON_Parse(json);
        if ((pr.root != nullptr) && cJSON_IsArray(pr.root) && cJSON_IsString(pr.root->child)) {
            s = pr.root->child->valuestring;
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(CjsonTest);
