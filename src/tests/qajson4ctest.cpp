#include "../test.h"

#include "qajson4c/src/qajson4c/qajson4c.h"

static void GenStat(Stat* s, const QAJ4C_Value* v) {
	const QAJ4C_Member* member;
	size_t count;

	switch(QAJ4C_get_type(v))
	{
	case QAJ4C_TYPE_OBJECT:
        count = QAJ4C_object_size(v);
        for ( size_t index = 0; index < count; ++index ) {
        	member = QAJ4C_object_get_member(v, index);
    		s->stringLength += QAJ4C_get_string_length(QAJ4C_member_get_key(member));
        	GenStat(s, QAJ4C_member_get_value(member));
        }
        s->objectCount++;
        s->memberCount += count;
		s->stringCount += count;
        break;
	case QAJ4C_TYPE_NULL:
		s->nullCount++;
		break;
	case QAJ4C_TYPE_ARRAY:
        count = QAJ4C_array_size(v);
        for ( size_t index = 0; index < count; ++index ) {
        	GenStat(s, QAJ4C_array_get(v, index));
        }
        s->arrayCount++;
        s->elementCount += count;
        break;
	case QAJ4C_TYPE_STRING:
		s->stringCount++;
		s->stringLength += QAJ4C_get_string_length(v);
		break;
	case QAJ4C_TYPE_NUMBER:
		s->numberCount++;
		break;
	case QAJ4C_TYPE_BOOL:
		if (QAJ4C_get_bool(v)) {
			s->trueCount++;
		} else {
			s->falseCount++;
		}
		break;
	default:
		break;
	}
}

class Qajson4cParseResult : public ParseResultBase {
public:
	Qajson4cParseResult(const char* json, size_t json_size ) :
			root(QAJ4C_parse_opt_dynamic(json, json_size, QAJ4C_PARSE_OPTS_STRICT, realloc)),
			len(json_size){
	}

	Qajson4cParseResult(const char* json ) :
			root(QAJ4C_parse_opt_dynamic(json, SIZE_MAX, QAJ4C_PARSE_OPTS_STRICT, realloc)),
			len(0) {
	}


	~Qajson4cParseResult() {
		free((void*) root);
	}

    const QAJ4C_Value* root;
    size_t len;
};

class Qajson4cStringResult : public StringResultBase {
public:
    Qajson4cStringResult() : s() {}
    ~Qajson4cStringResult() { free(s); }
    virtual const char* c_str() const { return s; }
    
    char* s;
};

class Qajson4cTest : public TestBase {
public:
    Qajson4cTest() = default;
#if TEST_INFO
    virtual const char* GetName() const { return "Qajson4c (C)"; }
    virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)length;
        Qajson4cParseResult* pr = new Qajson4cParseResult(json, length);
        if (QAJ4C_is_error(pr->root)) {
            delete pr;
            return 0;
        }
    	return pr;
    }
#endif

#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        const Qajson4cParseResult* pr = static_cast<const Qajson4cParseResult*>(parseResult);
        Qajson4cStringResult* sr = new Qajson4cStringResult;
        size_t alloc_size = pr->len < 32 ? 32 : pr->len;
        sr->s = (char*)malloc( alloc_size );
		QAJ4C_sprint(pr->root, sr->s, alloc_size);
        return sr;
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        const Qajson4cParseResult* pr = static_cast<const Qajson4cParseResult*>(parseResult);
        memset(stat, 0, sizeof(Stat));
        GenStat(stat, pr->root);
        return true;
    }
#endif

#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        Qajson4cParseResult pr(json);
        if (QAJ4C_is_array(pr.root) &&
        	QAJ4C_array_size(pr.root) == 1 &&
            QAJ4C_is_double(QAJ4C_array_get(pr.root, 0)))
        {
            *d = QAJ4C_get_double(QAJ4C_array_get(pr.root, 0));
            return true;
        }
        else {
            return false;
		}
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        Qajson4cParseResult pr(json);
        if (QAJ4C_is_array(pr.root) &&
			QAJ4C_array_size(pr.root) == 1 &&
			QAJ4C_is_string(QAJ4C_array_get(pr.root, 0)))
        {
            const QAJ4C_Value* str_val = QAJ4C_array_get(pr.root, 0);
            s = {QAJ4C_get_string(str_val), QAJ4C_get_string_length(str_val)};
            return true;
        }
        else {
            return false;
        }
    }
#endif
};

REGISTER_TEST(Qajson4cTest);
