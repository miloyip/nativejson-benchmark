#include "../test.h"

#if HAS_BOOST

#include "json_spirit/json_spirit/json_spirit_reader_template.h"
#include "json_spirit/json_spirit/json_spirit_writer_template.h"

using namespace json_spirit;

static void GenStat(Stat& stat, const Value& v) {
    switch (v.type()) {
    case obj_type:
        {
            const Object& o = v.get_obj();
            for (Object::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
                stat.stringLength += itr->name_.size();
                GenStat(stat, itr->value_);
            }
            stat.objectCount++;
            stat.memberCount += o.size();
            stat.stringCount += o.size();
        }
        break;

    case array_type:
        {
            const Array& a = v.get_array();

            for (Array::const_iterator i = a.begin(); i != a.end(); ++i)
                GenStat(stat, *i);
            stat.arrayCount++;
            stat.elementCount += a.size();
        }
        break;

    case str_type:
        stat.stringCount++;
        stat.stringLength += v.get_str().size();
        break;

    case int_type:
    case real_type:
        stat.numberCount++;
        break;

    case bool_type:
        if (v.get_bool())
            stat.trueCount++;
        else
            stat.falseCount++; break;
        break;

    case null_type:
        stat.nullCount++; break;
    }
}

class JsonspiritParseResult : public ParseResultBase {
public:
    Value root;
};

class JsonspiritStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class JsonspiritTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "JSON Spirit (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        JsonspiritParseResult* pr = new JsonspiritParseResult;
        if (!read_string(std::string(json, length), pr->root))
        {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JsonspiritParseResult* pr = static_cast<const JsonspiritParseResult*>(parseResult);
        JsonspiritStringResult* sr = new JsonspiritStringResult;
        sr->s = write_string(pr->root);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const JsonspiritParseResult* pr = static_cast<const JsonspiritParseResult*>(parseResult);
        JsonspiritStringResult* sr = new JsonspiritStringResult;
        sr->s = write_string(pr->root, pretty_print);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JsonspiritParseResult* pr = static_cast<const JsonspiritParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        Value root;
        if (read_string(std::string(json), root)) {
            try {
                *d = root.get_array()[0].get_real();
                return true;
            }
            catch (...) {
            }
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        Value root;
        if (read_string(std::string(json), root)) {
            try {
                s = root.get_array()[0].get_str();
                return true;
            }
            catch (...) {
            }
        }
        return false;
    }
#endif
};

REGISTER_TEST(JsonspiritTest);

#endif