#include "../test.h"
#include <sstream>

#if defined(_MSC_VER)
#define YY_NO_UNISTD_H

#include <io.h>
#define isatty _isatty
#define not !
#endif

#include "../tunnuz/lex.yy.cc"

#include "../tunnuz/json.tab.cc"
#include "../tunnuz/json_st.cc"

using namespace JSON;

static void GenStat(Stat& stat, const Value& v) {
    switch (v.type()) {
    case ARRAY:
        {
            Array a(v);
            for (std::vector<Value>::const_iterator itr = a.begin(); itr != a.end(); ++itr)
                GenStat(stat, *itr);
            stat.arrayCount++;
            stat.elementCount += a.size();
        }
        break;

    case OBJECT:
        {
            Object o(v);
            for (std::map<std::string, Value>::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
                GenStat(stat, itr->second);
                stat.stringLength += itr->first.size();
            }
            stat.objectCount++;
            stat.memberCount += o.size();
            stat.stringCount += o.size();   // member names
        }
        break;

    case STRING: 
        stat.stringCount++;
        stat.stringLength += v.as_string().size();
        break;

    case INT:
    case FLOAT:
        stat.numberCount++;
        break;

    case BOOL:
        if (v.as_bool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case NIL:
        stat.nullCount++;
        break;
    }
}

class TunnuzParseResult : public ParseResultBase {
public:
    Value root;
};

class TunnuzStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};
class TunnuzTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "tunnuz/JSON++ (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        TunnuzParseResult* pr = new TunnuzParseResult;
        try {
            pr->root = parse_string(json);
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
        const TunnuzParseResult* pr = static_cast<const TunnuzParseResult*>(parseResult);        
        TunnuzStringResult* sr = new TunnuzStringResult;
        std::ostringstream os;
        os << pr->root;
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const TunnuzParseResult* pr = static_cast<const TunnuzParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        Value root;
        try {
            root = parse_string(json);
            if (root.type() == ARRAY &&
                Array(root).size() == 1 &&
                Array(root)[0].type() == FLOAT)
            {
                *d = Array(root)[0].as_float();
                return true;
            }
        }
        catch (...) {
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        Value root;
        try {
            root = parse_string(json);
        
            if (root.type() == ARRAY &&
                Array(root).size() == 1 &&
                Array(root)[0].type() == STRING)
            {
                s = Array(root)[0].as_string();
                return true;
            }
        }
        catch (...) {
        }
        return false;
    }
#endif
};

REGISTER_TEST(TunnuzTest);
