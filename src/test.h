#pragma once

#include <vector>
#include <string.h>

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

    virtual void Free(void* userdata) const {
        (void)userdata;
    }

protected:
    const char* name_;
};

#define STRINGIFY(x) #x
#define REGISTER_TEST(cls) static cls gRegister##cls
