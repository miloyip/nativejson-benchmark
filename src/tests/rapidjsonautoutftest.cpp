#include "../test.h"

// Turn on SSE2 for x86-64.
#if defined(_M_X64) || defined(__amd64)
#define RAPIDJSON_SSE2
#endif

#include "rapidjson/document.h"
#include "rapidjson/encodedstream.h"
#include "rapidjson/memorystream.h"

using namespace rapidjson;

class RapidjsonAutoUTFParseResult : public ParseResultBase {
public:
    Document document;
};

class RapidjsonAutoUTFTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "RapidJSON_AutoUTF (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        MemoryStream ms(json, length);
        RapidjsonAutoUTFParseResult* pr = new RapidjsonAutoUTFParseResult;
        AutoUTFInputStream<unsigned, MemoryStream> is(ms);
        if (pr->document.ParseStream<0, AutoUTF<unsigned> >(is).HasParseError()) {
            delete pr;
            return 0;
        }
        return pr;
    }
#endif

};

REGISTER_TEST(RapidjsonAutoUTFTest);
