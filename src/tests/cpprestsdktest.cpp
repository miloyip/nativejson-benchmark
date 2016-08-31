#include "../test.h"

#if HAS_CPPREST

#include "cpprest/json.h"

#include <strstream>
#include <sstream>

using namespace web::json;
using namespace utility::conversions;

static void GenStat(Stat& stat, const value& v) {
    switch (v.type()) {
	case value::value_type::Array:
        for (auto const& element : v.as_array())
            GenStat(stat, element);
        stat.arrayCount++;
        stat.elementCount += v.size();
        break;

	case value::value_type::Object:
		for (auto const& kv : v.as_object()) {
            GenStat(stat, kv.second);
            stat.stringLength += kv.first.size();
        }
        stat.objectCount++;
        stat.memberCount += v.size();
        stat.stringCount += v.size();   // member names
        break;

	case value::value_type::String: 
        stat.stringCount++;
        stat.stringLength += v.as_string().size();
        break;

	case value::value_type::Number:
        stat.numberCount++;
        break;

	case value::value_type::Boolean:
        if (v.as_bool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

	case value::value_type::Null:
        stat.nullCount++;
        break;
    }
}

class CpprestsdkParseResult : public ParseResultBase {
public:
    value root;
};

class CpprestsdkStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class CpprestsdkTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "C++ REST SDK (C++11)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        CpprestsdkParseResult* pr = new CpprestsdkParseResult;
		std::istrstream is (json);
        try {
            pr->root = value::parse(is);
        }
        catch (web::json::json_exception& e) {
            printf("Parse error '%s'\n", e.what());
            delete pr;
            pr = 0;
        }
        catch (...) {
            delete pr;
            pr = 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const CpprestsdkParseResult* pr = static_cast<const CpprestsdkParseResult*>(parseResult);
		CpprestsdkStringResult* sr = new CpprestsdkStringResult;
        std::ostringstream os;
        pr->root.serialize(os);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const CpprestsdkParseResult* pr = static_cast<const CpprestsdkParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        std::istrstream is(json);
        try {
            value root = value::parse(is);
            *d = root.at(0).as_double();
            return true;
        }
        catch (...) {
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        std::istrstream is(json);
        try {
            value root = value::parse(is);
            s = to_utf8string(root.at(0).as_string());
            return true;
        }
        catch (...) {
        }
        return false;
    }
#endif
};

REGISTER_TEST(CpprestsdkTest);

#endif