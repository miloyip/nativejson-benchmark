#include "../test.h"

#if HAS_FOLLY && HAS_BOOST && (defined(__clang__) || defined(__GNUC__))

#include "folly/json.h"

using namespace folly;
using namespace folly::json;

static void GenStat(Stat& stat, const dynamic& v) {
    switch (v.type()) {
    case dynamic::ARRAY:
        {
            for (auto &i : v)
                GenStat(stat, i);
            stat.arrayCount++;
            stat.elementCount += v.size();
        }
        break;

    case dynamic::OBJECT:
        {
            auto p = v.items();
            for (auto& i : p) {
                GenStat(stat, i.second);
                stat.stringLength += i.first.size();
            }
            stat.objectCount++;
            stat.memberCount += v.size();
            stat.stringCount += v.size();
        }
        break;

    case dynamic::STRING: 
        stat.stringCount++;
        stat.stringLength += v.size();
        break;

    case dynamic::INT64:
    case dynamic::DOUBLE:
        stat.numberCount++;
        break;

    case dynamic::BOOL:
        if (v.getBool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case dynamic::NULLT:
        stat.nullCount++;
        break;
    }
}

class FollyParseResult : public ParseResultBase {
public:
    FollyParseResult() : root(nullptr) {}
    dynamic root;
};

class FollyStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    fbstring s;
};
class FollyTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Folly (C++11)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        FollyParseResult* pr = new FollyParseResult;
        try {
            pr->root = parseJson(StringPiece(json, json + length));
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
        const FollyParseResult* pr = static_cast<const FollyParseResult*>(parseResult);
        FollyStringResult* sr = new FollyStringResult;
        sr->s = toJson(pr->root);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const FollyParseResult* pr = static_cast<const FollyParseResult*>(parseResult);
        FollyStringResult* sr = new FollyStringResult;
        sr->s = toPrettyJson(pr->root);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const FollyParseResult* pr = static_cast<const FollyParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        try {
            dynamic v = parseJson(StringPiece(json));
            *d = v[0].getDouble();
            return true;
        }
        catch (...) {
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        try {
            dynamic v = parseJson(StringPiece(json));
            s = v[0].getString();
            return true;
        }
        catch (...) {
        }
        return false;
    }
#endif
};

REGISTER_TEST(FollyTest);

#endif
