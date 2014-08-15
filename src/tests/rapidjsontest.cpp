#include "../test.h"

// Turn on SSE2 for x86-64.
#if defined(_M_X64) || defined(__amd64)
#define RAPIDJSON_SSE2
#endif

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

class StatHandler {
public:
    typedef char Ch;

    StatHandler(Stat& stat) : stat_(stat) {}

    bool Null() { stat_.nullCount++; return true;  }
    bool Bool(bool b) { if (b) stat_.trueCount++; else stat_.falseCount++; return true; }
    bool Int(int) { stat_.numberCount++; return true; }
    bool Uint(unsigned) { stat_.numberCount++; return true; }
    bool Int64(int64_t) { stat_.numberCount++; return true; }
    bool Uint64(uint64_t) { stat_.numberCount++; return true; }
    bool Double(double) { stat_.numberCount++; return true; }
    bool String(const Ch*, SizeType length, bool) { stat_.stringCount++; stat_.stringLength += length; return true; }
    bool StartObject() { return true; }
    bool EndObject(SizeType memberCount) { stat_.objectCount++; stat_.memberCount += memberCount; return true; }
    bool StartArray() { return true; }
    bool EndArray(SizeType elementCount) { stat_.arrayCount++; stat_.elementCount += elementCount; return true; }

private:
    StatHandler& operator=(const StatHandler&);

    Stat& stat_;
};

class RapidjsonTest : public TestBase {
public:
	RapidjsonTest() : TestBase("RapidJSON") {
	}
	
    virtual void* Parse(const char* json) const {
    	Document *doc = new Document;
    	if (doc->Parse(json).HasParseError()) {
    		delete doc;
    		return 0;
    	}
    	return doc;
    }

    virtual char* Stringify(void* userdata) const {
    	Document* doc = reinterpret_cast<Document*>(userdata);
    	StringBuffer sb;
    	Writer<StringBuffer> writer(sb);
    	doc->Accept(writer);
    	return strdup(sb.GetString());
    }

    virtual char* Prettify(void* userdata) const {
    	Document* doc = reinterpret_cast<Document*>(userdata);
    	StringBuffer sb;
    	PrettyWriter<StringBuffer> writer(sb);
    	doc->Accept(writer);
    	return strdup(sb.GetString());
    }

    virtual Stat Statistics(void* userdata) const {
        Document* doc = reinterpret_cast<Document*>(userdata);
        Stat s;
        memset(&s, 0, sizeof(s));
        StatHandler h(s);
        doc->Accept(h);
        return s;
    }

    virtual void Free(void* userdata) const {
    	delete reinterpret_cast<Document*>(userdata);
    }
};

REGISTER_TEST(RapidjsonTest);
