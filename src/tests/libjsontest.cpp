#include "../test.h"

#ifndef NDEBUG
#define JSON_DEBUG
#endif

#include "libjson/libjson.h"

static void GenStat(Stat& stat, const JSONNode& v) {
    switch (v.type()) {
    case JSON_ARRAY:
        for (JSONNode::const_iterator itr = v.begin(); itr != v.end(); ++itr)
            GenStat(stat, *itr);
        stat.arrayCount++;
        stat.elementCount += v.size();
        break;

    case JSON_NODE:
        for (JSONNode::const_iterator itr = v.begin(); itr != v.end(); ++itr) {
            GenStat(stat, *itr);
            stat.stringLength += itr->name().size();
        }
        stat.objectCount++;
        stat.memberCount += v.size();
        stat.stringCount += v.size();   // member names
        break;

    case JSON_STRING:
        stat.stringCount++;
        stat.stringLength += v.as_string().size();
        break;

    case JSON_NUMBER:
        stat.numberCount++;
        break;

    case JSON_BOOL:
        if (v.as_bool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case JSON_NULL:
        stat.nullCount++;
        break;
    }
}

class LibjsonParseResult : public ParseResultBase {
public:
    JSONNode root;
};

class LibjsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class LibjsonTest : public TestBase {
public:
	LibjsonTest() : TestBase("Libjson") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        LibjsonParseResult* pr = new LibjsonParseResult;
        pr->root = libjson::parse(json);
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const LibjsonParseResult* pr = static_cast<const LibjsonParseResult*>(parseResult);
        LibjsonStringResult* sr = new LibjsonStringResult;
        sr->s = pr->root.write();
        return sr;
    }

    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const LibjsonParseResult* pr = static_cast<const LibjsonParseResult*>(parseResult);
        LibjsonStringResult* sr = new LibjsonStringResult;
        sr->s = pr->root.write_formatted();
        return sr;
    }

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const LibjsonParseResult* pr = static_cast<const LibjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(*stat, pr->root);
        return true;
    }
};

REGISTER_TEST(LibjsonTest);
