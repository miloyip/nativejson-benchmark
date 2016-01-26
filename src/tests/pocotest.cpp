#include "../test.h"

#if HAS_POCO

#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Array.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Stringifier.h"
#include "Poco/JSON/PrintHandler.h"
#include <stack>

using namespace Poco;
using namespace Poco::JSON;
using namespace Poco::Dynamic;

// template <typename Encoding = UTF8<> >
// class StatHandler : public BaseReaderHandler<Encoding, StatHandler<Encoding> > {
// public:
//     typedef typename Encoding::Ch Ch;

//     StatHandler(Stat& stat) : stat_(stat) {}

//     bool Null() { stat_.nullCount++; return true; }
//     bool Bool(bool b) { if (b) stat_.trueCount++; else stat_.falseCount++; return true; }
//     bool Int(int) { stat_.numberCount++; return true; }
//     bool Uint(unsigned) { stat_.numberCount++; return true; }
//     bool Int64(int64_t) { stat_.numberCount++; return true; }
//     bool Uint64(uint64_t) { stat_.numberCount++; return true; }
//     bool Double(double) { stat_.numberCount++; return true; }
//     bool String(const Ch*, SizeType length, bool) { stat_.stringCount++; stat_.stringLength += length; return true; }
//     bool StartObject() { return true; }
//     bool EndObject(SizeType memberCount) { stat_.objectCount++; stat_.memberCount += memberCount; return true; }
//     bool StartArray() { return true; }
//     bool EndArray(SizeType elementCount) { stat_.arrayCount++; stat_.elementCount += elementCount; return true; }

// private:
//     StatHandler& operator=(const StatHandler&);

//     Stat& stat_;
// };

static void GenStat(Stat& stat, const Var& v) {
    const std::type_info& type = v.type();
    if (type == typeid(Object::Ptr)) {
        Object::Ptr o = v.extract<Object::Ptr>();
        for (Object::ConstIterator itr = o->begin(); itr != o->end(); ++itr) {
            stat.stringLength += itr->first.size();
            GenStat(stat, itr->second);
        }
        stat.memberCount += o->size();
        stat.stringCount += o->size();
        stat.objectCount++;
    }
    else if (type == typeid(Poco::JSON::Array::Ptr)) {
        Poco::JSON::Array::Ptr a = v.extract<Poco::JSON::Array::Ptr>();
        for (Poco::JSON::Array::ConstIterator itr = a->begin(); itr != a->end(); ++itr)
            GenStat(stat, *itr);
        stat.elementCount += a->size();
        stat.arrayCount++;
    }
    else if (type == typeid(std::string)) {
        stat.stringCount++;
        stat.stringLength += v.toString().size();
    }
    else if (type == typeid(bool)) {
        if (v.extract<bool>())
            stat.trueCount++;
        else
            stat.falseCount++;
    }
    else if (v.isNumeric())
        stat.numberCount++;
    else if (v.isEmpty())
        stat.nullCount++;
}

class PocoParseResult : public ParseResultBase {
public:
    Var root;
};

class PocoStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class PocoTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "POCO (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        PocoParseResult* pr = new PocoParseResult;
        Parser parser;
        try {
            pr->root = parser.parse(std::string(json, length));
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
        const PocoParseResult* pr = static_cast<const PocoParseResult*>(parseResult);
        PocoStringResult* sr = new PocoStringResult;
        std::ostringstream os;
        Stringifier::condense(pr->root, os);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        const PocoParseResult* pr = static_cast<const PocoParseResult*>(parseResult);
        PocoStringResult* sr = new PocoStringResult;
        std::ostringstream os;
        Stringifier::stringify(pr->root, true, os, 4);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const PocoParseResult* pr = static_cast<const PocoParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
#if SLOWER_STAT
        StatHandler<> h(*stat);
        doc->Accept(h);
#else
        GenStat(*stat, pr->root);
#endif
        return true;
    }
#endif

#if TEST_SAXROUNDTRIP
    virtual StringResultBase* SaxRoundtrip(const char* json, size_t length) const {
        std::ostringstream os;
        PrintHandler::Ptr handler = new PrintHandler(os, 0);
        Parser parser(handler);
        try {
            parser.parse(std::string(json, length));
            PocoStringResult* sr = new PocoStringResult;
            sr->s = os.str();
            return sr;
        }
        catch (...) {
            return 0;
        }
    }
#endif

#if TEST_SAXSTATISTICS
    class StatHandler : public Handler {
    public:
        StatHandler(Stat* stat) : stat_(stat) {}

        virtual void reset() {}

        virtual void startObject() {
            handleValue();
            stat_->objectCount++;
            inArray_.push(false);
        }

        virtual void endObject() {
            inArray_.pop();
        }

        virtual void startArray() {
            handleValue();
            stat_->arrayCount++;
            inArray_.push(true);
        }

        virtual void endArray() {
            inArray_.pop();
        }

        virtual void key(const std::string& k) {
            stat_->memberCount++;
            stat_->stringCount++;
            stat_->stringLength += k.size();
        }

        virtual void null() {
            handleValue();
            stat_->nullCount++;
        }

        virtual void value(int v) {
            handleValue();
            stat_->numberCount++;
        }

        virtual void value(unsigned v) {
            handleValue();
            stat_->numberCount++;
        }

    #if defined(POCO_HAVE_INT64)
        virtual void value(Int64 v) {
            handleValue();
            stat_->numberCount++;
        }

        virtual void value(UInt64 v) {
            handleValue();
            stat_->numberCount++;
        }
    #endif

        virtual void value(const std::string& value) {
            handleValue();
            stat_->stringCount++;
            stat_->stringLength += value.size();
        }

        virtual void value(double d) {
            handleValue();
            stat_->numberCount++;
        }

        virtual void value(bool b) {
            handleValue();
            if (b)
                stat_->trueCount++;
            else
                stat_->falseCount++;
        }

    private:
        void handleValue() {
            if (!inArray_.empty() && inArray_.top())
                stat_->elementCount++;
        }

        Stat* stat_;
        std::stack<bool> inArray_;
    };

    virtual bool SaxStatistics(const char* json, size_t length, Stat* stat) const {
        memset(stat, 0, sizeof(Stat));
        Parser parser(new StatHandler(stat));
        try {
            parser.parse(std::string(json, length));
            return true;
        }
        catch (...) {
            return false;
        }
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        try {
            Parser parser;
            Var root = parser.parse(json);
            *d = root.extract<Poco::JSON::Array::Ptr>()->get(0).extract<double>();
            return true;
        }
        catch (...) {
            return false;
        }
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        try {
            Parser parser;
            Var root = parser.parse(json);
            s = root.extract<Poco::JSON::Array::Ptr>()->get(0).toString();
            return true;
        }
        catch (...) {
            return false;
        }
    }
#endif
};

REGISTER_TEST(PocoTest);

#endif // HAS_POCO
