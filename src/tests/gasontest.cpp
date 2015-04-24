#if (defined(_MSC_VER) && _MSC_VER >= 1700) || (__cplusplus >= 201103L)

#include "../test.h"
#ifdef _MSC_VER
#pragma warning (disable:4244) // conversion from 'int' to 'char', possible loss of data
#pragma warning (disable:4800) // 'uint64_t' : forcing value to bool 'true' or 'false' (performance warning)
#endif
#include "gason/src/gason.cpp"
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <sstream>

static void GenStat(Stat& stat, const JsonValue& v) {
    switch (v.getTag()) {
    case JSON_ARRAY:
        for (auto const& i : v) {
            GenStat(stat, i->value);
            stat.elementCount++;
        }
        stat.arrayCount++;
        break;

    case JSON_OBJECT:
        for (auto const& i : v) {
            GenStat(stat, i->value);
            stat.memberCount++;
            stat.stringLength += strlen(i->key);
            stat.stringCount++;   // Key
        }
        stat.objectCount++;
        break;

    case JSON_STRING: 
        stat.stringCount++;
        stat.stringLength += strlen(v.toString());
        break;

    case JSON_NUMBER:
        stat.numberCount++;
        break;

    case JSON_TRUE:
        stat.trueCount++;
        break;

    case JSON_FALSE:
        stat.falseCount++;
        break;

    case JSON_NULL:
        stat.nullCount++;
        break;
    }
}

static void dumpString(std::ostringstream& os, const char *s) {
    os.put('"');
    while (*s) {
        char c = *s++;
        switch (c) {
        case '\b':
            os << "\\b";
            break;
        case '\f':
            os << "\\f";
            break;
        case '\n':
            os << "\\n";
            break;
        case '\r':
            os << "\\r";
            break;
        case '\t':
            os << "\\t";
            break;
        case '\\':
            os << "\\\\";
            break;
        case '"':
            os << "\\\"";
            break;
        default:
            os.put(c);
        }
    }
    os << s << "\"";
}

static void dumpValue(std::ostringstream& os, const JsonValue& o, int shiftWidth, const std::string& linefeed = "", int indent = 0) {
    switch (o.getTag()) {
    case JSON_NUMBER:
        char buffer[32];
        sprintf(buffer, "%f", o.toNumber());
        os << buffer;
        break;
    case JSON_TRUE:
        os << "true";
        break;        
    case JSON_FALSE:
        os << "false";
        break;
    case JSON_STRING:
        dumpString(os, o.toString());
        break;
    case JSON_ARRAY:
        // It is not necessary to use o.toNode() to check if an array or object
        // is empty before iterating over its members, we do it here to allow
        // nicer pretty printing.
        if (!o.toNode()) {
            os << "[]";
            break;
        }
        os << "[" << linefeed;
        for (auto i : o) {
            if (shiftWidth > 0)
                os << std::setw(indent + shiftWidth) << " " << std::setw(0);
            dumpValue(os, i->value, shiftWidth, linefeed, indent + shiftWidth);
            if (i->next)
                os << ",";
            os << linefeed;
        }
        if (indent > 0)
            os << std::setw(indent) << " " << std::setw(0);
        os.put(']');
        break;
    case JSON_OBJECT:
        if (!o.toNode()) {
            os << "{}";
            break;
        }
        os << "{" << linefeed;
        for (auto i : o) {
            if (shiftWidth > 0)
                os << std::setw(indent + shiftWidth) << " " << std::setw(0);
            dumpString(os, i->key);
            os << ":";
            dumpValue(os, i->value, shiftWidth, linefeed, indent + shiftWidth);
            if (i->next)
                os << ",";
            os << linefeed;
        }
        if (indent > 0)
            os << std::setw(indent) << " " << std::setw(0);
        os.put('}');
        break;
    case JSON_NULL:
        os << "null";
        break;
    }
}

class GasonParseResult : public ParseResultBase {
public:
    GasonParseResult() : json() {}
    ~GasonParseResult() { free(json); }

    JsonAllocator allocator;
    JsonValue value;
    char *json;
};

class GasonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const {
        return s.c_str();
    }

    std::string s;
};

class GasonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "gason (C++11)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        GasonParseResult* pr = new GasonParseResult;
        char* end = 0;
        // gason uses insitu parsing, the source json must make a copy.
        pr->json = (char*)malloc(length + 1);
        memcpy(pr->json, json, length + 1);
        if (jsonParse(pr->json, &end, &pr->value, pr->allocator) != JSON_OK) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const GasonParseResult* pr = static_cast<const GasonParseResult*>(parseResult);
        std::ostringstream os;
        dumpValue(os, pr->value, 0);
        GasonStringResult* result = new GasonStringResult;
        result->s = os.str();
        return result;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const GasonParseResult* pr = static_cast<const GasonParseResult*>(parseResult);
        std::ostringstream os;
        dumpValue(os, pr->value, 4, "\n");
        GasonStringResult* result = new GasonStringResult;
        result->s = os.str();
        return result;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const GasonParseResult* pr = static_cast<const GasonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->value);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        GasonParseResult pr;
        char* end = 0;
        pr.json = strdup(json);
        if (jsonParse(pr.json, &end, &pr.value, pr.allocator) == JSON_OK &&
            pr.value.getTag() == JSON_ARRAY && 
            pr.value.toNode() &&
            pr.value.toNode()->value.getTag() == JSON_NUMBER)
        {
            *d = pr.value.toNode()->value.toNumber();
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        GasonParseResult pr;
        char* end = 0;
        pr.json = strdup(json);
        if (jsonParse(pr.json, &end, &pr.value, pr.allocator) == JSON_OK &&
            pr.value.getTag() == JSON_ARRAY && 
            pr.value.toNode() &&
            pr.value.toNode()->value.getTag() == JSON_STRING)
        {
            s = pr.value.toNode()->value.toString();
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(GasonTest);

#endif
