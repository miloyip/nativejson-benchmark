#include "../test.h"
#include <cstdlib>

class StrdupParseResult : public ParseResultBase {
public:
    StrdupParseResult() : r() {}
    ~StrdupParseResult() { free(r); }
    
    char *r;
};

class StrdupStringResult : public StringResultBase {
public:
    StrdupStringResult() : s() {}
    ~StrdupStringResult() { free(s); }

    virtual const char* c_str() const { return s; }
    
    char *s;
};

class StrdupTest : public TestBase {
public:
	StrdupTest() : TestBase("Strdup") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        StrdupParseResult* pr = new StrdupParseResult;
        pr->r = (char*)malloc(length);
        memcpy(pr->r, json, length + 1);
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const StrdupParseResult* pr = static_cast<const StrdupParseResult*>(parseResult);
        StrdupStringResult* sr = new StrdupStringResult;
    	sr->s = strdup(pr->r);
        return sr;
    }

    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const StrdupParseResult* pr = static_cast<const StrdupParseResult*>(parseResult);
        StrdupStringResult* sr = new StrdupStringResult;
        sr->s = strdup(pr->r);
        return sr;
    }

    virtual Stat Statistics(const ParseResultBase* parseResult) const {
        const StrdupParseResult* pr = static_cast<const StrdupParseResult*>(parseResult);
        Stat s;
        memset(&s, 0, sizeof(s));
        s.stringCount = 1;
        s.stringLength = strlen(pr->r);
        return s;
    }
};

REGISTER_TEST(StrdupTest);
