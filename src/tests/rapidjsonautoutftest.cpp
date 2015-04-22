#include "../test.h"

// __SSE2__ and __SSE4_2__ are recognized by gcc, clang, and the Intel compiler.
// We use -march=native with gmake to enable -msse2 and -msse4.2, if supported.
#if defined(__SSE4_2__)
#  define RAPIDJSON_SSE42      
#elif defined(__SSE2__)        
#  define RAPIDJSON_SSE2
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
        if (pr->document.ParseStream<kParseDefaultFlags, AutoUTF<unsigned> >(is).HasParseError()) {
            delete pr;
            return 0;
        }
        return pr;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        MemoryStream ms(json, strlen(json));
        RapidjsonAutoUTFParseResult* pr = new RapidjsonAutoUTFParseResult;
        AutoUTFInputStream<unsigned, MemoryStream> is(ms);
        Document doc;
        if (doc.ParseStream<kParseDefaultFlags, AutoUTF<unsigned> >(is).HasParseError() || !doc.IsArray() || doc.Size() != 1 || !doc[0].IsNumber())
            return false;
        *d = doc[0].GetDouble();
        return true;
    }

    virtual bool ParseString(const char* json, const char** s, size_t *length) const {
        MemoryStream ms(json, strlen(json));
        RapidjsonAutoUTFParseResult* pr = new RapidjsonAutoUTFParseResult;
        AutoUTFInputStream<unsigned, MemoryStream> is(ms);
        Document doc;
        if (doc.ParseStream<kParseDefaultFlags, AutoUTF<unsigned> >(is).HasParseError() || !doc.IsArray() || doc.Size() != 1 || !doc[0].IsString())
            return false;
        *s = doc[0].GetString();
        *length = doc[0].GetStringLength();
        return true;
    }
#endif
};

REGISTER_TEST(RapidjsonAutoUTFTest);
