#include "../test.h"

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

static void GenStat(Stat& stat, const Value& v) {
    switch (v.getType()) {
    case Value::ARRAY:
        {
            const Array& a = v.getArray();
            for (Array::const_iterator itr = a.begin(); itr != a.end(); ++itr)
                GenStat(stat, *itr);
            stat.arrayCount++;
            stat.elementCount += a.size();
        }
        break;

    case Value::OBJECT:
        {
            const Object& o = v.getObject();
            for (Object::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
                GenStat(stat, itr->second);
                stat.stringLength += itr->first.size();
            }
            stat.objectCount++;
            stat.memberCount += o.size();
            stat.stringCount += o.size();
        }
        break;

    case Value::STRING: 
        stat.stringCount++;
        stat.stringLength += v.getString().size();
        break;

    case Value::INTEGER:
    case Value::DOUBLE:
        stat.numberCount++;
        break;

    case Value::BOOLEAN:
        if (v.getBoolean())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case Value::NULL_VALUE:
        stat.nullCount++;
        break;
    }
}

class VoorheesParseResult : public ParseResultBase {
public:
    Value root;
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
        pr->root.loadFromString(json);
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const VoorheesParseResult* pr = static_cast<const VoorheesParseResult*>(parseResult);
        VoorheesStringResult* sr = new VoorheesStringResult;
        std::ostringstream os;
        pr->root.writeToStream(os, false, false);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const VoorheesParseResult* pr = static_cast<const VoorheesParseResult*>(parseResult);
        VoorheesStringResult* sr = new VoorheesStringResult;
        std::ostringstream os;
        pr->root.writeToStream(os, true, false);
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
};

REGISTER_TEST(VoorheesTest);
