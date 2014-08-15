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

class RapidjsonTest : public TestBase {
public:
	RapidjsonTest() : TestBase("rapidjson") {
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

    virtual void Free(void* userdata) const {
    	delete reinterpret_cast<Document*>(userdata);
    }
};

REGISTER_TEST(RapidjsonTest);
