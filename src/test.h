#pragma once

#include <vector>
#include <cstring>

#ifdef _MSC_VER
#define strdup _strdup
#endif

class TestBase;
typedef std::vector<const TestBase *> TestList;

class TestManager {
public:
    static TestManager& Instance() {
        static TestManager singleton;
        return singleton;
    }

    void AddTest(const TestBase* test) {
        mTests.push_back(test);
    }

    const TestList& GetTests() const {
        return mTests;
    }

    TestList& GetTests() {
        return mTests;
    }

private:
    TestList mTests;
};

struct Stat {
    size_t objectCount;
    size_t arrayCount;
    size_t numberCount;
    size_t stringCount;
    size_t trueCount;
    size_t falseCount;
    size_t nullCount;

    size_t memberCount;   // Number of members in all objects
    size_t elementCount;  // Number of elements in all arrays
    size_t stringLength;  // Number of code units in all strings
};

class TestBase {
public:
    TestBase(const char* name) : name_(name) {
        TestManager::Instance().AddTest(this);
    }

    const char* GetName() const {
        return name_;
    }

    bool operator<(const TestBase& rhs) const {
        return strcmp(name_, rhs.name_) < 0;
    }

    virtual void* Parse(const char* json) const {
        (void)json;  return 0;
    }

    virtual char* Stringify(void* userdata) const {
        (void)userdata;
        return 0; 
    }

    virtual char* Prettify(void* userdata) const {
        (void)userdata;
        return 0;
    }

    virtual Stat Statistics(void* userdata) const {
        (void)userdata;
        Stat s;
        memset(&s, 0, sizeof(s));
        return s;
    }

    virtual void Free(void* userdata) const {
        (void)userdata;
    }

protected:
    const char* name_;
};

#define STRINGIFY(x) #x
#define REGISTER_TEST(cls) static cls gRegister##cls
