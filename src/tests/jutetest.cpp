#include "../test.h"

#include "jute/jute.cpp"

using namespace jute;

static void GenStat(Stat& stat, jValue v) {
    switch (v.get_type()) {
    case JARRAY:
        {
            int size = v.size();
            for (int i = 0; i < size; i++)
                GenStat(stat, v[i]);

            stat.arrayCount++;
            stat.elementCount += size;
        }
        break;

    case JOBJECT:
        {
            int size = v.size();
            for (int i = 0; i < size; i++) {
                GenStat(stat, v[i]);
                //stat.stringLength += strlen(itr.memberName());
            }
            
            stat.objectCount++;
            stat.memberCount += size;
            stat.stringCount += size;
        }
        break;

    case JSTRING: 
        stat.stringCount++;
        stat.stringLength += v.as_string().size();
        break;

    case JNUMBER:
        stat.numberCount++;
        break;

    case JBOOLEAN:
        if (v.as_bool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case JNULL:
        stat.nullCount++;
        break;
    }
}

class JuteParseResult : public ParseResultBase {
public:
    JuteParseResult() : root(JUNKNOWN) {}
    jValue root;
};

class JuteStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class JuteTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "Jute (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif
	
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        JuteParseResult* pr = new JuteParseResult;
        pr->root = parser::parse(json);
        if (pr->root.get_type() == JNULL) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const JuteParseResult* pr = static_cast<const JuteParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        jValue root = parser::parse(json);
        *d = root[0].as_double();
        return true;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        jValue root = parser::parse(json);
        s = root[0].as_string();
        return true;
    }
#endif
};

REGISTER_TEST(JuteTest);
