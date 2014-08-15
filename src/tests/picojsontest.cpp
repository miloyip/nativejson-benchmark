#include "../test.h"
#ifdef _MSC_VER
#pragma warning (disable:4189) // local variable is initialized but not referenced
#pragma warning (disable:4127) // conditional expression is constant
#endif
#include "picojson/picojson.h"

using namespace picojson;

void GenStat(Stat& s, const value& v) {
    // Note: No public API to get value::_type
    if (v.is<object>()) {
        const object& o = v.get<object>();
        for (object::const_iterator itr = o.begin(); itr != o.end(); ++itr) {
            s.stringLength += itr->first.size(); // key
            GenStat(s, itr->second);
        }
        s.objectCount++;
        s.memberCount += o.size();
        s.stringCount += o.size();  // Key
    }
    else if (v.is<array>()) {
        const array& a = v.get<array>();
        for (array::const_iterator itr = a.begin(); itr != a.end(); ++itr)
            GenStat(s, *itr);
        s.arrayCount++;
        s.elementCount += a.size();
    }
    else if (v.is<std::string>()) {
        s.stringCount++;
        s.stringLength += v.get<std::string>().size();
    }
    else if (v.is<double>())
        s.numberCount++;
    else if (v.is<bool>()) {
        if (v.get<bool>())
            s.trueCount++;
        else
            s.falseCount++;
    }
    else if (v.is<null>())
        s.nullCount++;
}

class PicojsonTest : public TestBase {
public:
	PicojsonTest() : TestBase("PicoJSON") {
	}
	
    virtual void* Parse(const char* json, size_t length) const {
        value* v = new value;
        std::string err;
        parse(*v, json, json + length, &err);
    	if (!err.empty()) {
    		delete v;
    		return 0;
    	}
    	return v;
    }

    virtual char* Stringify(void* userdata) const {
        value* v = reinterpret_cast<value*>(userdata);
        return strdup(v->serialize().c_str());
    }

    virtual char* Prettify(void* userdata) const {
        // Note: no prettify functionality
        value* v = reinterpret_cast<value*>(userdata);
        return strdup(v->serialize().c_str());
    }

    virtual Stat Statistics(void* userdata) const {
        value* v = reinterpret_cast<value*>(userdata);
        Stat s;
        memset(&s, 0, sizeof(s));
        GenStat(s, *v);
        return s;
    }

    virtual void Free(void* userdata) const {
    	delete reinterpret_cast<value*>(userdata);
    }
};

REGISTER_TEST(PicojsonTest);
