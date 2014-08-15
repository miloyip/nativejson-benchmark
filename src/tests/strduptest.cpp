#include "../test.h"

class StrdupTest : public TestBase {
public:
	StrdupTest() : TestBase("strdup") {
	}
	
    virtual void* Parse(const char* json) const {
    	return _strdup(json);
    }

    virtual char* Stringify(void* userdata) const {
    	return _strdup((char*)userdata);
    }

    virtual char* Prettify(void* userdata) const {
        return _strdup((char*)userdata);
    }

    virtual void Free(void* userdata) const {
        free(userdata);
    }
};

REGISTER_TEST(StrdupTest);
