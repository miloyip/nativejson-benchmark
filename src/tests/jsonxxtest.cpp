#include "../test.h"
#ifdef _MSC_VER
#pragma warning (disable:4127) // conditional expression is constant
#pragma warning (disable:4244) // conversion from 'int' to 'char', possible loss of data
#endif
#include "jsonxx/jsonxx.cc"

using namespace jsonxx;

void GenObjectStat(Stat& s, const Object& o);

void GenStat(Stat& s, const Value& v) {
    switch (v.type_) {
    case Value::OBJECT_:
        GenObjectStat(s, v.get<Object>());
        break;

    case Value::ARRAY_:
        {
            const Array& a = v.get<Array>();
            Array::container::const_iterator itr = a.values().begin();
            Array::container::const_iterator end = a.values().end();
            for (; itr != end; ++itr)
                GenStat(s, **itr);
            s.arrayCount++;
            s.elementCount += a.size();
        }
        break;

    case Value::STRING_:
        s.stringCount++;
        s.stringLength += v.get<String>().size();
        break;

    case Value::NUMBER_:
        s.numberCount++;
        break;

    case Value::BOOL_:
        if (v.get<bool>())
            s.trueCount++;
        else
            s.falseCount++;
        break;

    case Value::NULL_:
        s.nullCount++;
        break;

    case Value::INVALID_:
        assert(false);
    }
}

void GenObjectStat(Stat& s, const Object& o) {
    Object::container::const_iterator itr = o.kv_map().begin();
    Object::container::const_iterator end = o.kv_map().end();
    for (; itr != end; ++itr) {
        s.stringLength += itr->first.size(); // key
        GenStat(s, *itr->second);
    }
    s.objectCount++;
    s.memberCount += o.size();
    s.stringCount += o.size();  // Key
}

class JsonxxTest : public TestBase {
public:
    JsonxxTest() : TestBase("JSON++") {
	}
	
    virtual void* Parse(const char* json) const {
#if 0
        // Not working
        Value* v = new Value;
        std::string err;
        if (!v->parse(json)) {
    		delete v;
    		return 0;
    	}
    	return v;
#else
        Object* o = new Object;
        if (!o->parse(json)) {
            delete o;
            return 0;
        }
        return o;
#endif
    }

    virtual char* Stringify(void* userdata) const {
#if 0
        // Not working
        Value* v = reinterpret_cast<Value*>(userdata);
        std::string s = v->is<Object>() ? v->get<Object>().json() : v->get<Array>().json();
        return strdup(s.c_str());
#else
        Object *o = reinterpret_cast<Object*>(userdata);
        return strdup(o->json().c_str());
#endif
    }

    virtual char* Prettify(void* userdata) const {
#if 0
        // Not working
        Value* v = reinterpret_cast<Value*>(userdata);
        std::string s = v->is<Object>() ? v->get<Object>().json() : v->get<Array>().json();
        return strdup(s.c_str());
#else
        Object *o = reinterpret_cast<Object*>(userdata);
        return strdup(o->json().c_str());
#endif
    }

    virtual Stat Statistics(void* userdata) const {
        Stat s;
        memset(&s, 0, sizeof(s));
#if 0
        // Not working
        Value* v = reinterpret_cast<Value*>(userdata);
        GenStat(s, *v);
        return s;
#else
        Object* o = reinterpret_cast<Object*>(userdata);
        GenObjectStat(s, *o);
        return s;
#endif
    }

    virtual void Free(void* userdata) const {
    	delete reinterpret_cast<Value*>(userdata);
    }
};

REGISTER_TEST(JsonxxTest);
