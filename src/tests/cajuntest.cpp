#include "../test.h"

#if !defined(_MSC_VER) // runtime crash in Visual Studio

#include "cajun-jsonapi/include/cajun/json/reader.h"
#include "cajun-jsonapi/include/cajun/json/writer.h"

using namespace json;

class StatVisitor : public ConstVisitor
{
public:
    StatVisitor(Stat& stat) : stat(stat) {}

    virtual void Visit(const Array& array) {
        for (Array::const_iterator itr = array.Begin(); itr != array.End(); ++itr)
            itr->Accept(*this);
        stat.arrayCount++;
        stat.elementCount += array.Size();
    }

    virtual void Visit(const Object& object) {
        for (Object::const_iterator itr = object.Begin(); itr != object.End(); ++itr) {
            stat.stringLength += itr->name.size();
            itr->element.Accept(*this);
        }
        stat.objectCount++;
        stat.memberCount += object.Size();
        stat.stringCount += object.Size(); // Keys
    }

    virtual void Visit(const Number& number) {
        (void)number;
        stat.numberCount++;
    }

    virtual void Visit(const String& string) {
        stat.stringCount++;
        stat.stringLength += static_cast<const std::string&>(string).size();
    }

    virtual void Visit(const Boolean& boolean) {
        if (boolean)
            stat.trueCount++;
        else
            stat.falseCount++;
    }

    virtual void Visit(const Null& null) {
        (void)null;
        stat.nullCount++;
    }

private:
    Stat& stat;
};

class CajunParseResult : public ParseResultBase {
public:
    UnknownElement root;
};

class CajunStringResult : public StringResultBase {
public:
    virtual const char* c_str() const { return s.c_str(); }

    std::string s;
};

class CajunTest : public TestBase {
public:
#if TEST_INFO
    virtual const char* GetName() const { return "CAJUN (C++)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        CajunParseResult* pr = new CajunParseResult;
        try
        {
            std::istringstream is(std::string(json, length));
            Reader::Read(pr->root, is);
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
        const CajunParseResult* pr = static_cast<const CajunParseResult*>(parseResult);
        CajunStringResult* sr = new CajunStringResult;
        std::stringstream os;
        Writer::Write(pr->root, os);
        sr->s = os.str();
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const CajunParseResult* pr = static_cast<const CajunParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        StatVisitor visitor(*stat);
        pr->root.Accept(visitor);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        try
        {
            std::istringstream is(json);
            UnknownElement root;
            Reader::Read(root, is);
            *d = (Number)root[0];
            return true;
        }
        catch (...) {
        }
        return false;
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        try
        {
            UnknownElement root;
            std::istringstream is(json);
            Reader::Read(root, is);
            s = (String&)root[0];
            return true;
        }
        catch (...) {
        }
        return false;
    }
#endif
};

REGISTER_TEST(CajunTest);

#endif