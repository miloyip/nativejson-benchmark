#include "../test.h"

#include "jsoncpp/src/lib_json/json_reader.cpp"
#include "jsoncpp/src/lib_json/json_value.cpp"
#include "jsoncpp/src/lib_json/json_writer.cpp"

using namespace Json;

void GenStat(Stat& stat, const Value& v) {
    switch (v.type()) {
    case arrayValue:
        for (ValueConstIterator itr = v.begin(); itr != v.end(); ++itr)
            GenStat(stat, *itr);
        stat.arrayCount++;
        stat.elementCount += v.size();
        break;

    case objectValue:
        for (ValueConstIterator itr = v.begin(); itr != v.end(); ++itr) {
            GenStat(stat, *itr);
            stat.stringLength += strlen(itr.memberName());
        }
        stat.objectCount++;
        stat.memberCount += v.size();
        stat.stringCount += v.size();   // member names
        break;

    case stringValue: 
        stat.stringCount++;
        stat.stringLength += v.asString().size();
        break;

    case intValue:
    case uintValue:
    case realValue:
        stat.numberCount++;
        break;

    case booleanValue:
        if (v.asBool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case nullValue:
        stat.nullCount++;
        break;
    }
}

class JsoncppTest : public TestBase {
public:
	JsoncppTest() : TestBase("JsonCpp") {
	}
	
    virtual void* Parse(const char* json) const {
        Value* root = new Value;
        Reader reader;
        reader.parse(json, *root);
    	return root;
    }

    virtual char* Stringify(void* userdata) const {
        Value* root = reinterpret_cast<Value*>(userdata);
        FastWriter writer;
        return strdup(writer.write(*root).c_str());
    }

    virtual char* Prettify(void* userdata) const {
        Value* root = reinterpret_cast<Value*>(userdata);
        StyledWriter writer;
        return strdup(writer.write(*root).c_str());
    }

    virtual Stat Statistics(void* userdata) const {
        Value* root = reinterpret_cast<Value*>(userdata);
        Stat s;
        memset(&s, 0, sizeof(s));
        GenStat(s, *root);
        return s;
    }

    virtual void Free(void* userdata) const {
        Value* root = reinterpret_cast<Value*>(userdata);
        delete root;
    }    
};

REGISTER_TEST(JsoncppTest);
