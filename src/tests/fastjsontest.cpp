#include "../test.h"

#include "fastjson/src/fastjson.cpp"
#include "fastjson/src/fastjson2.cpp"
#include "fastjson/src/fastjson_dom.cpp"
#include <cstdio>

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

static void ParseNumbers(const Token& token) {
    switch (token.type) {
    case Token::ArrayToken:
        for (ArrayEntry* child = token.array.ptr; child; child = child->next)
            ParseNumbers(child->tok);
        break;

    case Token::DictToken:
        for (DictEntry* child = token.dict.ptr; child; child = child->next)
            ParseNumbers(child->value_tok);
        break;

    case Token::ValueToken:
        if (token.value.type_hint == ValueType::NumberHint)
            atof(token.value.ptr);
        break;

    default:;
    }
}

static void WriteNumbers(Token& token) {
    switch (token.type) {
    case Token::ArrayToken:
        for (ArrayEntry* child = token.array.ptr; child; child = child->next)
            WriteNumbers(child->tok);
        break;

    case Token::DictToken:
        for (DictEntry* child = token.dict.ptr; child; child = child->next)
            WriteNumbers(child->value_tok);
        break;

    case Token::ValueToken:
        if (token.value.type_hint == ValueType::NumberHint) {
            if (token.value.size >= sizeof(double)) {
                char buffer[32] = { 0 };
                double d = atof(token.value.ptr);
                sprintf(buffer, "%17g", d);
            }
        }
        break;

    default:;
    }
}

class FastjsonParseResult : public ParseResultBase {
public:
    Token token;
    dom::Chunk chunk;
};

class FastjsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class FastjsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "mikeando/FastJson (C++)"; }
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
        ParseNumbers(pr->token);
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const FastjsonParseResult* pr = static_cast<const FastjsonParseResult*>(parseResult);
        FastjsonStringResult* sr = new FastjsonStringResult;
        // Since FastJson does not write numbers, emulate here in order to compare with other parsers.
        WriteNumbers(const_cast<Token&>(pr->token));
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

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        FastjsonParseResult pr;
        std::string error_message;
        if (dom::parse_string(json, &pr.token, &pr.chunk, 0, 0, &error_message) &&
            pr.token.type == Token::ArrayToken && pr.token.array.ptr &&
            pr.token.array.ptr->tok.type == Token::ValueToken &&
            pr.token.array.ptr->tok.value.type_hint == ValueType::NumberHint)
        {
            *d = atof(pr.token.array.ptr->tok.value.ptr);
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        FastjsonParseResult pr;
        std::string error_message;
        if (dom::parse_string(json, &pr.token, &pr.chunk, 0, 0, &error_message) &&
            pr.token.type == Token::ArrayToken && pr.token.array.ptr &&
            pr.token.array.ptr->tok.type == Token::ValueToken &&
            pr.token.array.ptr->tok.value.type_hint == ValueType::StringHint)
        {
            s = std::string(pr.token.array.ptr->tok.value.ptr, pr.token.array.ptr->tok.value.size);
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(FastjsonTest);
