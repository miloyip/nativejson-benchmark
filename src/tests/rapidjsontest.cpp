#include "../test.h"

// __SSE2__ and __SSE4_2__ are recognized by gcc, clang, and the Intel compiler.
// We use -march=native with gmake to enable -msse2 and -msse4.2, if supported.
#if defined(__SSE4_2__)
#  define RAPIDJSON_SSE42      
#elif defined(__SSE2__)        
#  define RAPIDJSON_SSE2
#endif

#ifndef TEST_PARSE_FLAG
#define TEST_PARSE_FLAG kParseDefaultFlags
#endif

#ifndef TEST_NAME
#define TEST_NAME "RapidJSON (C++)"
#endif

#ifndef TEST_CLASS
#define TEST_CLASS RapidjsonTest
#endif

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace rapidjson;

template <typename Encoding = UTF8<> >
class StatHandler : public BaseReaderHandler<Encoding, StatHandler<Encoding> > {
public:
    typedef typename Encoding::Ch Ch;

    StatHandler(Stat& stat) : stat_(stat) {}

    bool Null() { stat_.nullCount++; return true; }
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

static void GenStat(Stat& stat, const Value& v) {
    switch (v.GetType()) {
    case kNullType:  stat.nullCount++; break;
    case kFalseType: stat.falseCount++; break;
    case kTrueType:  stat.trueCount++; break;

    case kObjectType:
        for (Value::ConstMemberIterator m = v.MemberBegin(); m != v.MemberEnd(); ++m) {
            stat.stringLength += m->name.GetStringLength();
            GenStat(stat, m->value);
        }
        stat.objectCount++;
        stat.memberCount += (v.MemberEnd() - v.MemberBegin());
        stat.stringCount += (v.MemberEnd() - v.MemberBegin()); // Key
        break;

    case kArrayType:
        for (Value::ConstValueIterator i = v.Begin(); i != v.End(); ++i)
            GenStat(stat, *i);
        stat.arrayCount++;
        stat.elementCount += v.Size();
        break;

    case kStringType:
        stat.stringCount++;
        stat.stringLength += v.GetStringLength();
        break;

    case kNumberType:
        stat.numberCount++;
        break;
    }
}

class RapidjsonParseResult : public ParseResultBase {
public:
    Document document;
};

class RapidjsonStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return sb.GetString(); }

    StringBuffer sb;
};

class TEST_CLASS : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return TEST_NAME; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        RapidjsonParseResult* pr = new RapidjsonParseResult;
        if (pr->document.Parse<TEST_PARSE_FLAG>(json).HasParseError()) {
            delete pr;
            return 0;
        }
        return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const RapidjsonParseResult* pr = static_cast<const RapidjsonParseResult*>(parseResult);
        RapidjsonStringResult* sr = new RapidjsonStringResult;
        Writer<StringBuffer> writer(sr->sb);
        pr->document.Accept(writer);
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const RapidjsonParseResult* pr = static_cast<const RapidjsonParseResult*>(parseResult);
        RapidjsonStringResult* sr = new RapidjsonStringResult;
        PrettyWriter<StringBuffer> writer(sr->sb);
        pr->document.Accept(writer);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const RapidjsonParseResult* pr = static_cast<const RapidjsonParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
#if SLOWER_STAT
        StatHandler<> h(*stat);
        doc->Accept(h);
#else
        GenStat(*stat, pr->document);
#endif
        return true;
    }
#endif

#if TEST_SAXROUNDTRIP
    virtual StringResultBase* SaxRoundtrip(const char* json, size_t length) const {
        (void)length;
        Reader reader;
        RapidjsonStringResult* sr = new RapidjsonStringResult;
        StringStream is(json);
        Writer<StringBuffer> writer(sr->sb);
        if (!reader.Parse<TEST_PARSE_FLAG>(is, writer)) {
            delete sr;
            return 0;
        }
        return sr;
    }
#endif

#if TEST_SAXSTATISTICS
    virtual bool SaxStatistics(const char* json, size_t length, Stat* stat) const {
        (void)length;
        memset(stat, 0, sizeof(Stat));
        Reader reader;
        StringStream is(json);
        StatHandler<> handler(*stat);
        return reader.Parse<TEST_PARSE_FLAG>(is, handler);
    }
#endif

#if TEST_SAXSTATISTICSUTF16
    virtual bool SaxStatisticsUTF16(const char* json, size_t length, Stat* stat) const {
        (void)length;
        memset(stat, 0, sizeof(Stat));
        GenericReader<UTF8<>, UTF16<> > reader;
        StringStream is(json);
        StatHandler<UTF16<> > handler(*stat);
        return reader.Parse(is, handler);
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        Document doc;
        if (doc.Parse<TEST_PARSE_FLAG>(json).HasParseError() || !doc.IsArray() || doc.Size() != 1 || !doc[0].IsNumber())
            return false;
        *d = doc[0].GetDouble();
        return true;
    }

    virtual bool ParseString(const char* json, const char** s, size_t *length) const {
        Document doc;
        if (doc.Parse<TEST_PARSE_FLAG>(json).HasParseError() || !doc.IsArray() || doc.Size() != 1 || !doc[0].IsString())
            return false;
        *s = doc[0].GetString();
        *length = doc[0].GetStringLength();
        return true;
    }
#endif
};

REGISTER_TEST(TEST_CLASS);
