#include "../test.h"

#if HAS_BOOST && (!defined(_MSC_VER) || _MSC_VER >= 1900) && !defined(__i386__) // VS2015, and works on 64-bit only

#include "json-voorhees/src/jsonv/algorithm_map.cpp"
#include "json-voorhees/src/jsonv/algorithm_traverse.cpp"
#include "json-voorhees/src/jsonv/array.cpp"
#include "json-voorhees/src/jsonv/char_convert.cpp"
#include "json-voorhees/src/jsonv/coerce.cpp"
#include "json-voorhees/src/jsonv/demangle.cpp"
#include "json-voorhees/src/jsonv/detail.cpp"
#include "json-voorhees/src/jsonv/encode.cpp"
#include "json-voorhees/src/jsonv/object.cpp"
#include "json-voorhees/src/jsonv/parse.cpp"
#include "json-voorhees/src/jsonv/path.cpp"
#include "json-voorhees/src/jsonv/serialization.cpp"
#include "json-voorhees/src/jsonv/serialization_builder.cpp"
#include "json-voorhees/src/jsonv/tokenizer.cpp"
#include "json-voorhees/src/jsonv/util.cpp"
#include "json-voorhees/src/jsonv/value.cpp"
#include "json-voorhees/src/jsonv/detail/token_patterns.cpp"

using namespace jsonv;

static void GenStat(Stat& stat, const value& v) {
    switch (v.kind()) {
    case kind::array:
        {
            for (value::const_array_iterator itr = v.begin_array(); itr != v.end_array(); ++itr)
                GenStat(stat, *itr);
            stat.arrayCount++;
            stat.elementCount += v.size();
        }
        break;

    case kind::object:
        {
            for (value::const_object_iterator itr = v.begin_object(); itr != v.end_object(); ++itr) {
                GenStat(stat, itr->second);
                stat.stringLength += itr->first.size();
            }
            stat.objectCount++;
            stat.memberCount += v.size();
            stat.stringCount += v.size();
        }
        break;

    case kind::string: 
        stat.stringCount++;
        stat.stringLength += v.size();
        break;

    case kind::integer:
    case kind::decimal:
        stat.numberCount++;
        break;

    case kind::boolean:
        if (v.as_boolean())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case kind::null:
        stat.nullCount++;
        break;
    }
}

class VoorheesParseResult : public ParseResultBase {
public:
    value root;
};

class VoorheesStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};
class VoorheesTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "JSON Voorhees (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        VoorheesParseResult* pr = new VoorheesParseResult;
        try {
            pr->root = parse(json);
        }
        catch (...) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const VoorheesParseResult* pr = static_cast<const VoorheesParseResult*>(parseResult);
        VoorheesStringResult* sr = new VoorheesStringResult;
        sr->s = to_string(pr->root);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const VoorheesParseResult* pr = static_cast<const VoorheesParseResult*>(parseResult);
        VoorheesStringResult* sr = new VoorheesStringResult;
        std::ostringstream os;
        ostream_pretty_encoder e(os, 4);
        e.encode(pr->root);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const VoorheesParseResult* pr = static_cast<const VoorheesParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        try {
            value v = parse(json);
            *d = v[0].as_decimal();
            return true;
        }
        catch (...) {
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        try {
            value v = parse(json);
            s = v[0].as_string();
            return true;
        }
        catch (...) {
        }
        return false;
    }
#endif
};

REGISTER_TEST(VoorheesTest);

#endif