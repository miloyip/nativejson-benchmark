#include "../test.h"

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

//static void GenStat(Stat* s, const UnknownElement& v) {
//    switch (v.) {
//    case json_object:
//        for (size_t i = 0; i < v->u.object.length; i++) {
//            const json_object_entry* e = v->u.object.values + i;
//            GenStat(s, e->value);
//            s->stringLength += e->name_length;
//        }
//        s->stringCount += v->u.object.length;
//        s->memberCount += v->u.object.length;
//        s->objectCount++;
//        break;
//
//    case json_array:
//        for (size_t i = 0; i < v->u.array.length; i++)
//            GenStat(s, v->u.array.values[i]);
//        s->arrayCount++;
//        s->elementCount += v->u.array.length;
//        break;
//
//    case json_string:
//        s->stringCount++;
//        s->stringLength += v->u.string.length;
//        break;
//
//    case json_integer:
//    case json_double:
//        s->numberCount++; 
//        break;
//
//    case json_boolean:
//        if (v->u.boolean)
//            s->trueCount++;
//        else
//            s->falseCount++;
//        break;
//
//    case json_null:
//        s->nullCount++;
//        break;
//    }
//}

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
    CajunTest() : TestBase("Cajun") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        CajunParseResult* pr = new CajunParseResult;
        std::istringstream is(std::string(json, length));
        try
        {
            Reader::Read(pr->root, is);
        }
        catch (Reader::ParseException&) {
            delete pr;
            return 0;
        }
    	return pr;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const CajunParseResult* pr = static_cast<const CajunParseResult*>(parseResult);
        CajunStringResult* sr = new CajunStringResult;
        std::stringstream os;
        Writer::Write(pr->root, os);
        sr->s = os.str();
        return sr;
    }

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const CajunParseResult* pr = static_cast<const CajunParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        StatVisitor visitor(*stat);
        pr->root.Accept(visitor);
        return true;
    }
};

REGISTER_TEST(CajunTest);
