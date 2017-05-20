#if defined(__clang__) || defined(__GNUC__)

#include "../test.h"

#include "nlohmann/src/json.hpp"

#include "taocppjson/include/tao/json.hpp"

#include "taocppjson/contrib/nlohmann/to_value.hpp"
#include "taocppjson/contrib/nlohmann/from_value.hpp"

using namespace nlohmann;

static void GenStat(Stat& stat, const json& v) {
    switch (v.type()) {
    case json::value_t::array:
        for (auto& element : v)
            GenStat(stat, element);
        stat.arrayCount++;
        stat.elementCount += v.size();
        break;

    case json::value_t::object:
        for (json::const_iterator it = v.begin(); it != v.end(); ++it) {
            GenStat(stat, it.value());
            stat.stringLength += it.key().size();
        }
        stat.objectCount++;
        stat.memberCount += v.size();
        stat.stringCount += v.size();
        break;

    case json::value_t::string:
        stat.stringCount++;
        stat.stringLength += v.get<std::string>().size();
        break;

    case json::value_t::number_integer:
    case json::value_t::number_unsigned:
    case json::value_t::number_float:
        stat.numberCount++;
        break;

    case json::value_t::boolean:
        if (v)
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case json::value_t::null:
        stat.nullCount++;
        break;

    case json::value_t::discarded:
        throw std::logic_error( "code should be unreachable" );
    }
}

class NlohmannParseResult : public ParseResultBase {
public:
    json root;
};

class NlohmannStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class TaocppNlohmannTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "taocpp/json & Nlohmann (C++11)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* j, size_t length) const {
        NlohmannParseResult* pr = new NlohmannParseResult;
        try {
            tao::json::nlohmann::to_value<json> handler;
            tao::json::sax::from_string(j, length, handler);
            pr->root = std::move( handler.value );
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
        const NlohmannParseResult* pr = static_cast<const NlohmannParseResult*>(parseResult);
        NlohmannStringResult* sr = new NlohmannStringResult;
        std::ostringstream oss;
        tao::json::sax::to_stream oss_handler(oss);
        tao::json::nlohmann::from_value(pr->root, oss_handler);
        sr->s = oss.str();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const NlohmannParseResult* pr = static_cast<const NlohmannParseResult*>(parseResult);
        NlohmannStringResult* sr = new NlohmannStringResult;
        std::ostringstream oss;
        tao::json::sax::to_pretty_stream oss_handler(oss, 4);
        tao::json::nlohmann::from_value(pr->root, oss_handler);
        sr->s = oss.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const NlohmannParseResult* pr = static_cast<const NlohmannParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* j, double* d) const {
        try {
            json root = json::parse(j);
            *d = root[0].get<double>();
            return true;
        }
        catch (...) {
        }
        return false;
    }

    virtual bool ParseString(const char* j, std::string& s) const {
        try {
            json root = json::parse(j);
            s = root[0].get<std::string>();
            return true;
        }
        catch (...) {
        }
        return false;
    }
#endif
};

REGISTER_TEST(TaocppNlohmannTest);

#endif
