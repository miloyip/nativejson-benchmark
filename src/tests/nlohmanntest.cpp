#if defined(__clang__) || defined(__GNUC__)

#include "../test.h"
#include "nlohmann/src/json.hpp"

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
class NlohmannTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Nlohmann (C++11)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* j, size_t length) const {
        (void)length;
        NlohmannParseResult* pr = new NlohmannParseResult;
        try {
            pr->root = json::parse(j);
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
        sr->s = pr->root.dump();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const NlohmannParseResult* pr = static_cast<const NlohmannParseResult*>(parseResult);
        NlohmannStringResult* sr = new NlohmannStringResult;
        sr->s = pr->root.dump(4);
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

REGISTER_TEST(NlohmannTest);

#endif