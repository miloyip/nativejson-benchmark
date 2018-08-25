#ifndef THORS_SERIALIZER_COMMON_H
#define THORS_SERIALIZER_COMMON_H

class TestAction
{
    public:
    virtual ParseResultBase* Parse(const char*, size_t) const {
        return nullptr;
    }
    virtual bool ParseDouble(const char*, double*) const {
        return false;
    }
    virtual bool ParseString(const char*, std::string&) const {
        return false;
    }
    virtual StringResultBase* SaxRoundtrip(const char*, size_t) const {
        return nullptr;
    }
    virtual StringResultBase* Stringify(const ParseResultBase*) const {
        return nullptr;
    }
    virtual StringResultBase* Prettify(const ParseResultBase*) const {
        return nullptr;
    }
    virtual bool Statistics(const ParseResultBase*, Stat*) const {
        return false;
    }
};

using ThorsAnvil::Serialize::jsonExport;
using ThorsAnvil::Serialize::jsonImport;
using ThorsAnvil::Serialize::ParserInterface;
using ThorsAnvil::Serialize::PrinterInterface;

class VectorDouble: public TestAction
{
    public:
    virtual bool ParseDouble(const char* json, double* d) const {
        std::stringstream stream(json);
        std::vector<double> result;
        stream >> jsonImport(result, ParserInterface::ParseType::Weak, true);
        if (stream && result.size() == 1) {
            *d = result[0];
            return true;
        }
        return false;
    }
};
class VectorString: public TestAction
{
    public:
    virtual bool ParseString(const char* json, std::string& output) const {
        std::stringstream stream(json);
        std::vector<std::string> result;
        stream >> jsonImport(result, ParserInterface::ParseType::Weak, true);
        if (stream && result.size() == 1) {
            output = result[0];
            return true;
        }
        return false;
    }
};


template<typename Value>
struct GetValueResult: public ParseResultBase
{
    Value       data;
    GetValueResult()
        : data{}
    {}
};
struct GetValueStream: public StringResultBase {
    std::stringstream   stream;
    virtual const char* c_str() const   {return stream.str().c_str();}
};

class CStringStream: public std::istream
{
    class CStringStreamBuf: public std::streambuf
    {
        public:
            CStringStreamBuf(char const* buffer, std::size_t len)
            {
                // This is safe as long as it is only used
                // as part of an istream (read only).
                // That is why I have wrapped this inside a class that only
                // inherits from std::istream.
                //
                // Any put back operations don't write to the buffer.
                // They validate they are putting back the same character
                // that is in the buffer and if not call pbackfail()
                // By default the pbackfail() fails returning EOF.
                // This is fine for our use case.
                char* b = const_cast<char*>(buffer);
                setg(b, b, b+ len);
            }
    };
    CStringStreamBuf    streamBuf;;
    public:
        CStringStream(char const* buffer, std::size_t len)
            // This is technically unsafe as the streamBuf has not been
            // initialized. But the istream constructor does not use
            // the buffer, only stores the pointer for later use.
            // So this will work as expected.
            : std::istream(&streamBuf)
            , streamBuf(buffer, len)
        {}
};

template<typename Value>
class GetValue: public TestAction
{
    public:
    virtual ParseResultBase* Parse(const char* json, size_t len) const {
        GetValueResult<Value>* result = new GetValueResult<Value>();
        CStringStream        stream(json, len);
        stream >> jsonImport(result->data, ParserInterface::ParseType::Weak, true);
        char bad;
        if (!stream || stream >> bad) {
            delete result;
            result = nullptr;
        }
        return result;
    }
    virtual StringResultBase* Stringify(ParseResultBase const* value) const {
        GetValueStream* result = new GetValueStream;
        GetValueResult<Value> const* inputValue = dynamic_cast<GetValueResult<Value> const*>(value);
        result->stream << jsonExport(inputValue->data, PrinterInterface::OutputType::Stream, true);
        return result;
    }
    virtual StringResultBase* Prettify(const ParseResultBase* value) const {
        GetValueStream* result = new GetValueStream;
        GetValueResult<Value> const* inputValue = dynamic_cast<GetValueResult<Value> const*>(value);
        result->stream << jsonExport(inputValue->data, PrinterInterface::OutputType::Config, true);
        return result;
    }
    virtual bool Statistics(const ParseResultBase* value, Stat* stat) const {
        GetValueResult<Value> const* inputValue = dynamic_cast<GetValueResult<Value> const*>(value);
        getStats(stat, inputValue->data);
        return true;
    }
};

using ActionMap = std::map<std::string, TestAction*>;
using   L01= std::vector<std::string>;
using   L02= std::vector<L01>;
using   L03= std::vector<L02>;
using   L04= std::vector<L03>;
using   L05= std::vector<L04>;
using   L06= std::vector<L05>;
using   L07= std::vector<L06>;
using   L08= std::vector<L07>;
using   L09= std::vector<L08>;
using   L10= std::vector<L09>;
using   L11= std::vector<L10>;
using   L12= std::vector<L11>;
using   L13= std::vector<L12>;
using   L14= std::vector<L13>;
using   L15= std::vector<L14>;
using   L16= std::vector<L15>;
using   L17= std::vector<L16>;
using   L18= std::vector<L17>;
using   M01= std::map<std::string, std::string>;
inline void getStats(Stat* stat, std::map<std::string, M01> const& value)
{
    stat->objectCount++;
    stat->memberCount += value.size();
    for(auto const& da: value) {
        stat->stringLength += da.first.size();
        getStats(stat, da.second);
    }
}

struct Obj2
{
    std::string     foo;
};
ThorsAnvil_MakeTrait(Obj2, foo);
inline void getStats(Stat* stat, Obj2 const& value)
{
    stat->objectCount++;
    stat->elementCount++;
    stat->stringCount += 2; /* Key + Value */
    stat->stringLength += (3 /*foo*/ + value.foo.size());
}

struct Obj3
{
    Opt<int>        a;
    std::string     foo;
};
ThorsAnvil_MakeTrait(Obj3, a, foo);
inline void getStats(Stat* stat, Obj3 const& value)
{
    stat->objectCount++;
    stat->elementCount += 2;
    ((value.a) ? stat->numberCount : stat->nullCount)++;
    stat->stringCount += 3; /*Key + Key + Value*/
    stat->stringLength += (3 /*foo*/ + 1 /*a*/ + value.foo.size());
}

#endif
