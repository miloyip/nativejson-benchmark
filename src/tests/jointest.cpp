#if defined(__GNUC__)

#include "../test.h"

#include "join/join/core/src/error.cpp"
#include "join/join/sax/include/join/json.hpp"

#include <sstream>

using join::sax::Value;
using join::sax::JsonReader;
using join::sax::JsonWriter;

static void GenStat (Stat& stat, const Value& v) 
{
    switch (v.index ()) {
        case Value::ArrayValue:
            for (auto& element : v.getArray ())
                GenStat (stat, element);
            stat.arrayCount++;
            stat.elementCount += v.size ();
            break;

        case Value::ObjectValue:
            for (auto& element : v.getObject ()) {
                GenStat (stat, element.second);
                stat.stringLength += element.first.size ();
            }
            stat.objectCount++;
            stat.memberCount += v.size ();
            stat.stringCount += v.size ();
            break;

        case Value::String:
            stat.stringCount++;
            stat.stringLength += v.size ();
            break;

        case Value::Integer:
        case Value::Unsigned:
        case Value::Integer64:
        case Value::Unsigned64:
        case Value::Real:
            stat.numberCount++;
            break;

        case Value::Boolean:
            if (v.isTrue ())
                stat.trueCount++;
            else
                stat.falseCount++;
            break;

        case Value::Null:
            stat.nullCount++;
            break;
    }
}

class JoinParseResult : public ParseResultBase {
public:
    Value root;
};

class JoinStringResult : public StringResultBase {
public:
    virtual const char* c_str () const { 
        return s.c_str ();
    }

    std::string s;
};

class JoinTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName () const { 
        return "Join (C++14)"; 
    }

    virtual const char* GetFilename () const { 
        return __FILE__;
    }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse (const char* j, size_t length) const {
        JoinParseResult* pr = new JoinParseResult;
        if (pr->root.deserialize <JsonReader> (j, length) == -1) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify (const ParseResultBase* parseResult) const {
        const JoinParseResult* pr = static_cast <const JoinParseResult*> (parseResult);
        JoinStringResult* sr = new JoinStringResult;
        std::stringstream ss;
        pr->root.serialize <JsonWriter> (ss);
        sr->s = ss.str ();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify (const ParseResultBase* parseResult) const {
        const JoinParseResult* pr = static_cast <const JoinParseResult*> (parseResult);
        JoinStringResult* sr = new JoinStringResult;
        std::stringstream ss;
        JsonWriter writer (ss, 2);
        writer.serialize (pr->root);
        sr->s = ss.str ();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics (const ParseResultBase* parseResult, Stat* stat) const {
        const JoinParseResult* pr = static_cast <const JoinParseResult*> (parseResult);
        memset (stat, 0, sizeof (Stat));
        GenStat (*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble (const char* j, double* d) const {
        Value root;
        if (root.deserialize <JsonReader> (j) != -1) {
            *d = root[0].getDouble ();
            return true;
        }
        return false;
    }

    virtual bool ParseString (const char* j, std::string& s) const {
        Value root;
        if (root.deserialize <JsonReader> (j) != -1) {
            s = root[0].getString ();
            return true;
        }
        return false;
    }
#endif
};

REGISTER_TEST(JoinTest);

#endif
