#include "../test.h"

#include "fastjson/src/fastjson.cpp"
#include "fastjson/src/fastjson2.cpp"
#include "fastjson/src/fastjson_dom.cpp"

using namespace fastjson;

static void GenStat(Stat& stat, const Token& token) {
    switch (token.type) {
    case Token::ArrayToken:
        for (ArrayEntry* child = token.array.ptr; child; child = child->next) {
            GenStat(stat, child->tok);
            stat.elementCount++;
        }
        stat.arrayCount++;
        break;

    case Token::DictToken:
        for (DictEntry* child = token.dict.ptr; child; child = child->next) {
            GenStat(stat, child->value_tok);
            stat.memberCount++;
            stat.stringCount++;
            stat.stringLength += child->key_tok.value.size;
        }
        stat.objectCount++;
        break;

    case Token::ValueToken:
        if (token.value.type_hint == ValueType::StringHint) {
            stat.stringCount++;
            stat.stringLength += token.value.size;
        }
        else
            stat.numberCount++;
        break;

    case Token::LiteralTrueToken:
        stat.trueCount++;
        break;

    case Token::LiteralFalseToken:
        stat.falseCount++;
        break;

    case Token::LiteralNullToken:
        stat.nullCount++;
        break;
    }
}

static double ParseNumbers(const Token& token) {
    double sum = 0.0;
    switch (token.type) {
    case Token::ArrayToken:
        for (ArrayEntry* child = token.array.ptr; child; child = child->next)
            sum += ParseNumbers(child->tok);
        break;

    case Token::DictToken:
        for (DictEntry* child = token.dict.ptr; child; child = child->next)
            sum += ParseNumbers(child->value_tok);
        break;

    case Token::ValueToken:
        if (token.value.type_hint == ValueType::NumberHint)
            sum += atof(token.value.ptr);
        break;

    default:;
    }
    return sum;
}

class FastjsonParseResult : public ParseResultBase {
public:
    Token token;
    dom::Chunk chunk;
    double sum;
};

class FastjsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class FastjsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "FastJson (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        FastjsonParseResult* pr = new FastjsonParseResult;
        std::string error_message;
        if (!dom::parse_string(json, &pr->token, &pr->chunk, 0, 0, &error_message)) {
            delete pr;
            return 0;
        }
        // Since FastJson does not parse numbers, emulate here in order to compare with other parsers.
        pr->sum = ParseNumbers(pr->token);
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const FastjsonParseResult* pr = static_cast<const FastjsonParseResult*>(parseResult);
        FastjsonStringResult* sr = new FastjsonStringResult;
        // Since FastJson does not parse numbers, cannot write numbers. Here just using parsing to emulate. 
        // (parsing double is often faster than writing double)
        double d = ParseNumbers(pr->token);
        sr->s = as_string(&pr->token);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const FastjsonParseResult* pr = static_cast<const FastjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->token);
        return true;
    }
#endif
};

REGISTER_TEST(FastjsonTest);
