#include "../test.h"

#if ((defined(_MSC_VER) && _MSC_VER >= 1700) || (__cplusplus >= 201103L && HAS_BOOST)) && !defined(__CYGWIN__)

#define _NO_ASYNCRTIMP 1
#define CPPREST_EXCLUDE_WEBSOCKETS

#ifdef _WIN32
#include "casablanca/Release/src/utilities/web_utilities.cpp" // Must compile this first
#include "casablanca/Release/src/http/common/http_msg.cpp"
#include "casablanca/Release/src/http/common/http_helpers.cpp"
#include "casablanca/Release/src/http/client/http_client_msg.cpp"
#include "casablanca/Release/src/http/client/http_client_winhttp.cpp"
#include "casablanca/Release/src/http/oauth/oauth1.cpp"
#include "casablanca/Release/src/websockets/client/ws_client.cpp"
#include "casablanca/Release/src/uri/uri.cpp"
#include "casablanca/Release/src/uri/uri_builder.cpp"
#include "casablanca/Release/src/uri/uri_parser.cpp"
#include "casablanca/Release/src/utilities/base64.cpp"
#pragma comment (lib, "Winhttp.lib")
#pragma comment (lib, "Bcrypt.lib")
#pragma comment (lib, "Crypt32.lib")
#endif

#include "casablanca/Release/src/pch/stdafx.h"
#include "casablanca/Release/src/json/json.cpp"
#include "casablanca/Release/src/json/json_parsing.cpp"
#include "casablanca/Release/src/json/json_serialization.cpp"
#include "casablanca/Release/src/utilities/asyncrt_utils.cpp"

#include <strstream>
#include <sstream>

using namespace web::json;

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

class CasablancaParseResult : public ParseResultBase {
public:
    value root;
};

class CasablancaStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class CasablancaTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "C++ REST SDK (C++11)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        CasablancaParseResult* pr = new CasablancaParseResult;
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
        const CasablancaParseResult* pr = static_cast<const CasablancaParseResult*>(parseResult);
		CasablancaStringResult* sr = new CasablancaStringResult;
        std::ostringstream os;
        pr->root.serialize(os);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const CasablancaParseResult* pr = static_cast<const CasablancaParseResult*>(parseResult);
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

REGISTER_TEST(CasablancaTest);

#endif