#include "../test.h"
#ifdef _MSC_VER
#pragma warning (disable:4244) // conversion from 'int' to 'char', possible loss of data
#pragma warning (disable:4800) // 'uint64_t' : forcing value to bool 'true' or 'false' (performance warning)
#endif
#include "gason/gason.cpp"
#include <iomanip>
#include <sstream>

static void GenStat(Stat& stat, const JsonValue& v) {
    switch (v.getTag()) {
    case JSON_TAG_ARRAY:
        for (auto i : v) {
            GenStat(stat, i->value);
            stat.elementCount++;
        }
        stat.arrayCount++;
        break;

    case JSON_TAG_OBJECT:
        for (auto i : v) {
            GenStat(stat, i->value);
            stat.memberCount++;
            stat.stringLength += strlen(i->key);
            stat.stringCount++;   // Key
        }
        stat.objectCount++;
        break;

    case JSON_TAG_STRING: 
        stat.stringCount++;
        stat.stringLength += strlen(v.toString());
        break;

    case JSON_TAG_NUMBER:
        stat.numberCount++;
        break;

    case JSON_TAG_BOOL:
        if (v.toBool())
            stat.trueCount++;
        else
            stat.falseCount++;
        break;

    case JSON_TAG_NULL:
        stat.nullCount++;
        break;
    }
}

static void dumpString(std::ostringstream& os, const char *s) {
    os.put('"');
    while (*s) {
        char c = *s++;
        switch (c) {
        case '\b':
            os << "\\b";
            break;
        case '\f':
            os << "\\f";
            break;
        case '\n':
            os << "\\n";
            break;
        case '\r':
            os << "\\r";
            break;
        case '\t':
            os << "\\t";
            break;
        case '\\':
            os << "\\\\";
            break;
        case '"':
            os << "\\\"";
            break;
        default:
            os.put(c);
        }
    }
    os << s << "\"";
}

static void dumpValue(std::ostringstream& os, const JsonValue& o, int shiftWidth, int indent = 0) {
    switch (o.getTag()) {
    case JSON_TAG_NUMBER:
        char buffer[32];
        sprintf(buffer, "%f", o.toNumber());
        os << buffer;
        break;
    case JSON_TAG_BOOL:
        os << (o.toBool() ? "true" : "false");
        break;
    case JSON_TAG_STRING:
        dumpString(os, o.toString());
        break;
    case JSON_TAG_ARRAY:
        // It is not necessary to use o.toNode() to check if an array or object
        // is empty before iterating over its members, we do it here to allow
        // nicer pretty printing.
        if (!o.toNode()) {
            os << "[]";
            break;
        }
        os << "[\n";
        for (auto i : o) {
            if (shiftWidth > 0)
                os << std::setw(indent + shiftWidth) << " " << std::setw(0);
            dumpValue(os, i->value, shiftWidth, indent + shiftWidth);
            os << (i->next ? ",\n" : "\n");
        }
        if (indent > 0)
            os << std::setw(indent) << " " << std::setw(0);
        os.put(']');
        break;
    case JSON_TAG_OBJECT:
        if (!o.toNode()) {
            os << "{}";
            break;
        }
        os << "{\n";
        for (auto i : o) {
            if (shiftWidth > 0)
                os << std::setw(indent + shiftWidth) << " " << std::setw(0);
            dumpString(os, i->key);
            os << ": ";
            dumpValue(os, i->value, shiftWidth, indent + shiftWidth);
            os << (i->next ? ",\n" : "\n");
        }
        if (indent > 0)
            os << std::setw(indent) << " " << std::setw(0);
        os.put('}');
        break;
    case JSON_TAG_NULL:
        os << "null";
        break;
    }
}

struct GasonState {
    GasonState() : json() {}
    ~GasonState() { free(json); }

    JsonAllocator allocator;
    JsonValue value;
    char *json;
};

class GasonTest : public TestBase {
public:
	GasonTest() : TestBase("gason") {
	}
	
    virtual void* Parse(const char* json, size_t length) const {
        (void)length;
        GasonState* state = new GasonState;
        char* end = 0;
        state->json = strdup(json);
        if (jsonParse(state->json, &end, &state->value, state->allocator) != JSON_PARSE_OK) {
            delete state;
            return 0;
        }
    	return state;
    }

    virtual char* Stringify(void* userdata) const {
        GasonState* state = reinterpret_cast<GasonState*>(userdata);
        std::ostringstream os;
        dumpValue(os, state->value, 0);
        return strdup(os.str().c_str());
    }

    virtual char* Prettify(void* userdata) const {
        GasonState* state = reinterpret_cast<GasonState*>(userdata);
        std::ostringstream os;
        dumpValue(os, state->value, 4);
        return strdup(os.str().c_str());
    }

    virtual Stat Statistics(void* userdata) const {
        GasonState* state = reinterpret_cast<GasonState*>(userdata);
        Stat s;
        memset(&s, 0, sizeof(s));
        GenStat(s, state->value);
        return s;
    }

    virtual void Free(void* userdata) const {
        JsonValue* root = reinterpret_cast<JsonValue*>(userdata);
        delete root;
    }    
};

REGISTER_TEST(GasonTest);
