
#include "../test.h"

#include <boost/json/src.hpp>

#if TEST_STRINGIFY
#include <sstream>

boost::json::serializer serializer_;
char buf_[BOOST_JSON_STACK_BUFFER_SIZE];
std::ostringstream ss_;
#endif

static void GenStat(Stat& stat, const boost::json::value& v) {
    switch(v.kind()) {
        case boost::json::kind::array: {
            const boost::json::array &a = v.get_array();
            for (auto it = a.begin(); a.end() != it; ++it) {
                GenStat(stat, *it);
            }
            ++stat.arrayCount;
            stat.elementCount += a.size();
            break;
        }

        case boost::json::kind::object: {
            const boost::json::object &o = v.get_object();
            for (auto it = o.begin(); o.end() != it; ++it) {
                stat.stringLength += it->key().size();
                GenStat(stat, it->value());
            }
            ++stat.objectCount;
            stat.memberCount += o.size();
            stat.stringCount += o.size();
            break;
        }

        case boost::json::kind::string: {
                ++stat.stringCount;
                stat.stringLength += v.get_string().size();
                break;
        }

        case boost::json::kind::uint64:
        case boost::json::kind::int64:
        case boost::json::kind::double_:
            ++stat.numberCount;
            break;

        case boost::json::kind::bool_: {
            if (v.get_bool()) {
                ++stat.trueCount;
            } else {
                ++stat.falseCount;
            }
            break;
        }

        case boost::json::kind::null:
            ++stat.nullCount;
            break;
    }
}

class BoostjsonParseResult : public ParseResultBase {
public:
    boost::json::value root;
};

class BoostjsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const override { return s.c_str(); }

    std::string s;
};

class BoostjsonTest : public TestBase {
public:
#if TEST_STRINGIFY
    virtual void SetUp() const {
        ss_.str("");
        ss_.clear();
    }
#endif

#if TEST_INFO
    virtual const char* GetName() const override { return "Boost.JSON"; }
    virtual const char* GetFilename() const override { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const override {
        (void)length;
        boost::json::error_code ec;
        boost::json::value jv = boost::json::parse(json, ec);
        if(ec) {
            return NULL;
        }
        BoostjsonParseResult* pr = new BoostjsonParseResult;
        pr->root = jv;
        return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const override {
        const BoostjsonParseResult* pr = static_cast<const BoostjsonParseResult*>(parseResult);
        BoostjsonStringResult* sr = new BoostjsonStringResult;
        serializer_.reset(&pr->root);
        while(!serializer_.done()) {
            ss_ << serializer_.read(buf_);
        }
        sr->s = ss_.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const override {
        const BoostjsonParseResult* pr = static_cast<const BoostjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const override {
        boost::json::error_code ec;
        boost::json::value jv = boost::json::parse(json, ec);
        if(!ec && jv.is_array() && jv.get_array().size() == 1 && jv.get_array()[0].is_double()) {
            *d = jv.get_array()[0].get_double();
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const override {
        boost::json::error_code ec;
        boost::json::value jv = boost::json::parse(json, ec);
        if(!ec && jv.is_array() && jv.get_array().size() == 1 && jv.get_array()[0].is_string()) {
            const auto &as = jv.get_array()[0].get_string();
            // do not use `s = as.c_str()` here, it might not copy everything (e.g. if there is a \0 in the string)
            s = std::string(as.c_str(), as.size());
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(BoostjsonTest);
