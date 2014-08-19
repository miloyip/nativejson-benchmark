#include "../test.h"

#include "ujson4c/src/ujdecode.h"

extern "C" {

static void GenStat(Stat* s, UJObject v) {
	switch (UJGetType(v)) {
	case UJT_Object:
		{
			void* iter = UJBeginObject(v);
			UJString key;
			UJObject value;
			while (UJIterObject(&iter, &key, &value)) {
				s->memberCount++;
				s->stringCount++;
				s->stringLength += key.cchLen;
				GenStat(s, value);
			}
			s->objectCount++;
		}
		break;

	case UJT_Array:
		{
			void* iter = UJBeginArray(v);
			UJObject value;
			while (UJIterArray(&iter, &value)) {
				GenStat(s, value);
				s->elementCount++;
			}
			s->arrayCount++;
		}
		break;

	case UJT_String:
		{
			size_t length;
			UJReadString(v, &length);
			s->stringCount++;  
			s->stringLength += length;
		}
		break;

	case UJT_Long: 
	case UJT_LongLong:
	case UJT_Double:
		s->numberCount++;  break;

	case UJT_True: s->trueCount++; break;
	case UJT_False: s->falseCount++; break;
	case UJT_Null: s->nullCount++; break;
    default:;
    }
}

} // extern "C"

class Ujson4cParseResult : public ParseResultBase {
public:
    Ujson4cParseResult() : root(), state() {}
    ~Ujson4cParseResult() { UJFree(state); }

    UJObject root;
	void* state;
};

class Ujson4c : public TestBase {
public:
	Ujson4c() : TestBase("ujson4c (C)") {
	}
	
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        Ujson4cParseResult* pr = new Ujson4cParseResult;
		pr->root = UJDecode(json, length, NULL, &pr->state);
    	return pr;
    }

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const Ujson4cParseResult* pr = static_cast<const Ujson4cParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
};

REGISTER_TEST(Ujson4c);
