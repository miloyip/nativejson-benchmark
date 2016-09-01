#include "../test.h"
#include "jsmn/jsmn.h"

static void GenStat(Stat* s, const char* json, const jsmntok_t* tokens, int count) {
    for (int i = 0; i < count; i++)
    switch (tokens[i].type) {
    case JSMN_OBJECT:
        s->memberCount += tokens[i].size;
        s->objectCount++;
        break;

    case JSMN_ARRAY:
        s->elementCount += tokens[i].size;
        s->arrayCount++;
        break;

    case JSMN_STRING:
        s->stringCount++;
        s->stringLength += tokens[i].end - tokens[i].start;
        break;

    case JSMN_PRIMITIVE:
        if (json[tokens[i].start] == 't')
            s->trueCount++;
        else if (json[tokens[i].start] == 'f')
            s->falseCount++;
        else if (json[tokens[i].start] == 'n')
            s->nullCount++;
        else
            s->numberCount++; 
        break;

    default:;
    }
}

static void ParseNumbers(const char* json, const jsmntok_t* tokens, int count) {
    for (int i = 0; i < count; i++)
        if (tokens[i].type == JSMN_PRIMITIVE) {
            const char* s = json + tokens[i].start;
            if (*s != 't' && *s != 'f' && *s != 'n')
                atof(s);
        }
}

class JsmnParseResult : public ParseResultBase {
public:
    JsmnParseResult() : json(), tokens() {}
    ~JsmnParseResult() { free(json); free(tokens); }

    char* json;
    jsmntok_t* tokens;
    int count;
};

class JsmnTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "jsmn (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        JsmnParseResult* pr = new JsmnParseResult;
        jsmn_parser parser;
        jsmn_init(&parser);
        pr->count = jsmn_parse(&parser, json, length, NULL, 0);
        if (pr->count < 0) {
            printf("Error %d\n", pr->count);
            delete pr;
            return 0;
        }

        jsmn_init(&parser);
        pr->tokens = (jsmntok_t*)malloc(pr->count * sizeof(jsmntok_t));
        int error = jsmn_parse(&parser, json, length, pr->tokens, pr->count);
        if (error < 0) {
            printf("Error %d\n", error);
            delete pr;
            return 0;
        }

        // need a copy of JSON in order to determine the types
        pr->json = strdup(json);

        // Since jsmn does not parse numbers, emulate here in order to compare with other parsers.
        ParseNumbers(json, pr->tokens, pr->count);
    	return pr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JsmnParseResult* pr = static_cast<const JsmnParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->json, pr->tokens, pr->count);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        jsmn_parser parser;
        jsmn_init(&parser);
        jsmntok_t tokens[2];
        int count = jsmn_parse(&parser, json, strlen(json), tokens, 2);
        if (count == 2 && tokens[0].type == JSMN_ARRAY && tokens[0].size == 1 && tokens[1].type == JSMN_PRIMITIVE) {
            *d = atof(json + tokens[1].start);
            return true;
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        jsmn_parser parser;
        jsmn_init(&parser);
        jsmntok_t tokens[2];
        int count = jsmn_parse(&parser, json, strlen(json), tokens, 2);
        if (count == 2 && tokens[0].type == JSMN_ARRAY && tokens[0].size == 1 && tokens[1].type == JSMN_STRING) {
            s = std::string(json + tokens[1].start, json + tokens[1].end);
            return true;
        }
        return false;
    }
#endif
};

REGISTER_TEST(JsmnTest);
