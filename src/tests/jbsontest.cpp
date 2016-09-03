#include "../test.h"

#if HAS_BOOST && __cplusplus >= 201402L // C++14

#define thread_local // workaround since OS X not supporting this

#include "jbson/document.hpp"
#include "jbson/json_reader.hpp"
#include "jbson/json_writer.hpp"

using namespace jbson;

static void GenStat(Stat& stat, const element& v);

static void GenStat(Stat& stat, const document& d) {
    for (auto& i : d) {
        GenStat(stat, i);
        stat.stringLength += i.name().size();
        stat.memberCount++;
        stat.stringCount++;
    }
    stat.objectCount++;
}

static void GenStat(Stat& stat, const array& a) {
    for (auto &i : a) {
        GenStat(stat, i);
        stat.elementCount++;
    }
    stat.arrayCount++;
}

static void GenStat(Stat& stat, const element& v) {
    switch (v.type()) {
    case element_type::document_element:
        GenStat(stat, get<element_type::document_element>(v));
        break;

    case element_type::array_element:
        GenStat(stat, get<element_type::array_element>(v));
        break;

    case element_type::string_element:
        stat.stringCount++;
        stat.stringLength += v.value<std::string>().size();
        break;

    case element_type::double_element:
    case element_type::int32_element:
    case element_type::int64_element:
        stat.numberCount++;
        break;

    case element_type::boolean_element:
        if (v.value<bool>())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case element_type::null_element:
        stat.nullCount++;
        break;
    }
}

class JbsonParseResult : public ParseResultBase {
public:
    JbsonParseResult(bool isArray_) : isArray(isArray_) {}
    document d;
    array a;
    bool isArray;
};

class JbsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};
class JbsonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Jbson (C++14)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        // Determine object or array
        for (size_t i = 0; i < length; i++) {
            switch (json[i]) {
                case '{':
                    {
                        JbsonParseResult* pr = new JbsonParseResult(false);
                        try {
                            pr->d = read_json(json);
                        }
                        catch (const json_parse_error& ) {
                            delete pr;
                            return 0;
                        }
                        return pr;
                    }
                case '[':
                    {
                        JbsonParseResult* pr = new JbsonParseResult(true);
                        try {
                            pr->a = read_json_array(json);
                        }
                        catch (const json_parse_error& ) {
                            delete pr;
                            return 0;
                        }
                        return pr;
                    }
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    continue;
            }
            // Unknown first non-whitespace character
            break;
        }
        return 0;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JbsonParseResult* pr = static_cast<const JbsonParseResult*>(parseResult);
        JbsonStringResult* sr = new JbsonStringResult;
        if (pr->isArray)
            write_json(pr->a, std::back_inserter(sr->s));
        else
            write_json(pr->d, std::back_inserter(sr->s));
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JbsonParseResult* pr = static_cast<const JbsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        if (pr->isArray)
            GenStat(*stat, pr->a);
        else
            GenStat(*stat, pr->d);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        try {
            array a = read_json_array(json);
            *d = a.begin()->value<double>();
            return true;
        }
        catch (...) {
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        try {
            array a = read_json_array(json);
            s = a.begin()->value<std::string>();
            return true;
        }
        catch (...) {
        }
        return false;
    }
#endif
};

REGISTER_TEST(JbsonTest);

#endif
