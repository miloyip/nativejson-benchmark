#pragma once
#include "config.h"
#include "memorystat.h"

#include <vector>
#include <cstring>
#include <cstdlib>

class TestBase;
typedef std::vector<const TestBase *> TestList;

class TestManager {
public:
    static TestManager& Instance() {
        static TestManager* singleton = new TestManager;
        return *singleton;
    }

    static void DestroyInstance() {
        delete &Instance();
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

// Each test can customize what to be stored in parse result, 
// which will be passed to Stringify()/Prettify()/Statistics()
class ParseResultBase {
public:
    virtual ~ParseResultBase() {}
};

// Stringify()/Prettify() returns object derived from this class.
// So that it can prevents unncessary strdup().
class StringResultBase {
public:
    virtual ~StringResultBase() {}

    // The test framework call this function to get a null-terminated string.
    virtual const char* c_str() const = 0;
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

    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        (void)json;
        (void)length;
        return 0;
    }

    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        (void)parseResult;
        return 0;
    }

    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        (void)parseResult;
        return 0;
    }

    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        (void)parseResult;
        (void)stat;
        return false;
    }

    virtual StringResultBase* SaxRoundtrip(const char* json, size_t length) const {
        (void)json;
        (void)length;
        return 0;
    }

    virtual bool SaxStatistics(const char* json, size_t length, Stat* stat) const {
        (void)json;
        (void)length;
        (void)stat;
        return false;
    }

    virtual bool SaxStatisticsUTF16(const char* json, size_t length, Stat* stat) const {
        (void)json;
        (void)length;
        (void)stat;
        return false;
    }

protected:
    const char* name_;
};

#define REGISTER_TEST(cls) static cls gRegister##cls
