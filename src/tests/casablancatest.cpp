#if (defined(_MSC_VER) && _MSC_VER >= 1700) || (__cplusplus >= 201103L)

#include "../test.h"

#define _NO_ASYNCRTIMP 1

#include "casablanca/Release/src/pch/stdafx.h"

#include "casablanca/Release/src/json/json.cpp"
#include "casablanca/Release/src/json/json_parsing.cpp"
#include "casablanca/Release/src/json/json_serialization.cpp"
#include "casablanca/Release/src/utilities/asyncrt_utils.cpp"
#include "casablanca/Release/src/http/common/http_msg.cpp"
#include "casablanca/Release/src/http/common/http_helpers.cpp"
#include "casablanca/Release/src/http/client/http_msg_client.cpp"
#include "casablanca/Release/src/uri/uri.cpp"
#include "casablanca/Release/src/uri/uri_builder.cpp"
#include "casablanca/Release/src/uri/uri_parser.cpp"
#include <strstream>

using namespace web::json;
using namespace utility::conversions;

void GenStat(Stat& stat, const value& v) {
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
	CasablancaTest() : TestBase("Casablanca") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        CasablancaParseResult* pr = new CasablancaParseResult;
		std::istrstream is (json);
        pr->root = value::parse(is);
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const CasablancaParseResult* pr = static_cast<const CasablancaParseResult*>(parseResult);
		CasablancaStringResult* sr = new CasablancaStringResult;
		sr->s = to_utf8string(pr->root.serialize());
        return sr;
    }

    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const CasablancaParseResult* pr = static_cast<const CasablancaParseResult*>(parseResult);
		CasablancaStringResult* sr = new CasablancaStringResult;
		sr->s = to_utf8string(pr->root.serialize());
        return sr;
    }

    virtual Stat Statistics(const ParseResultBase* parseResult) const {
        const CasablancaParseResult* pr = static_cast<const CasablancaParseResult*>(parseResult);
        Stat s;
        memset(&s, 0, sizeof(s));
        GenStat(s, pr->root);
        return s;
    }
};

REGISTER_TEST(CasablancaTest);

#endif