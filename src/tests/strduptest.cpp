#include "../test.h"
#include <cstdlib>

class StrdupTest : public TestBase {
public:
	StrdupTest() : TestBase("strdup") {
	}
	
    virtual void* Parse(const char* json, size_t length) const {
        void* r = malloc(length);
        memcpy(r, json, length + 1);
    	return r;
    }

    virtual char* Stringify(void* userdata) const {
    	return strdup((char*)userdata);
    }

    virtual char* Prettify(void* userdata) const {
        return strdup((char*)userdata);
    }

    virtual Stat Statistics(void* userdata) const {
        Stat s;
        memset(&s, 0, sizeof(s));
        s.stringCount = 1;
        s.stringLength = strlen((char*)userdata);
        return s;
    }

    virtual void Free(void* userdata) const {
        free(userdata);
    }
};

REGISTER_TEST(StrdupTest);
