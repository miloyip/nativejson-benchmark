#include "../test.h"

#include "Jzon/Jzon.cpp"

using namespace Jzon;

static void GenStat(Stat& stat, const Node& v) {
    switch (v.getType()) {
    case Node::T_ARRAY:
        for (Node::const_iterator itr = v.begin(); itr != v.end(); ++itr)
            GenStat(stat, (*itr).second);
        stat.arrayCount++;
        stat.elementCount += v.getCount();
        break;

    case Node::T_OBJECT:
        for (Node::const_iterator itr = v.begin(); itr != v.end(); ++itr) {
            GenStat(stat, (*itr).second);
            stat.stringLength += (*itr).first.size();
        }
        stat.objectCount++;
        stat.memberCount += v.getCount();
        stat.stringCount += v.getCount();
        break;

    case Node::T_STRING: 
        stat.stringCount++;
        stat.stringLength += v.toString().size();
        break;

    case Node::T_NUMBER:
        stat.numberCount++;
        break;

    case Node::T_BOOL:
        if (v.toBool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case Node::T_NULL:
        stat.nullCount++;
        break;
    }
}

class JzonParseResult : public ParseResultBase {
public:
    Node root;
};

class JzonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }
    std::string s;
};

class JzonTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Jzon (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JzonParseResult* pr = new JzonParseResult;
        Parser parser;
        pr->root = parser.parseString(json);
        if (!parser.getError().empty()) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const JzonParseResult* pr = static_cast<const JzonParseResult*>(parseResult);
        JzonStringResult* sr = new JzonStringResult;
        Writer writer;
        writer.writeString(pr->root, sr->s);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const JzonParseResult* pr = static_cast<const JzonParseResult*>(parseResult);
        JzonStringResult* sr = new JzonStringResult;
        const Format format = { true, true, true, 4 };
        Writer writer(format);
        writer.writeString(pr->root, sr->s);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JzonParseResult* pr = static_cast<const JzonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        Parser parser;
        Node root = parser.parseString(json);
        if (parser.getError().empty() && root.isArray() && root.getCount() == 1 && root.get(0).isNumber()) {
            *d = root.get(0).toDouble();
            return true;
        }
        else
            return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        Parser parser;
        Node root = parser.parseString(json);
        if (parser.getError().empty() && root.isArray() && root.getCount() == 1 && root.get(0).isString()) {
            s = root.get(0).toString();
            return true;
        }
        else
            return false;
    }
#endif
};

REGISTER_TEST(JzonTest);
