#include "../test.h"

#include "ThorsSerializer_StringNumber.h"
#include "ThorsSerializer_GetStats.h"
#include "ThorsSerializer_Twitter.h"
#include "ThorsSerializer_Canada.h"
#include "ThorsSerializer_citm_catalog.h"

#include "ThorSerialize/Traits.h"
#include "ThorSerialize/JsonThor.h"
#include "ThorSerialize/SerUtil.h"
#include <iostream>
#include <map>
#include <memory>
#include "ThorsSerializer_Common.h"

class ThorsSerializerTest: public TestBase
{
    TestAction                                   testNotImplemented;
    VectorDouble                                 testVectorDouble;
    VectorString                                 testVectorString;
    GetValue<std::map<std::string, std::string>> testGetValueMap2String;
    GetValue<std::map<std::string, bool>>        testGetValueMap2Bool;
    GetValue<std::map<std::string, int>>         testGetValueMap2Int;
    GetValue<std::map<std::string, int*>>        testGetValueMap2IntPointer;
    GetValue<std::map<std::string, M01>>         testGetValueMap2M01;
    GetValue<std::vector<std::string>>           testGetValueVec2String;
    //GetValue<std::vector<L18>>                   testGetValueVec2L18;
    GetValue<std::vector<int*>>                  testGetValueVec2IntPointer;
    GetValue<std::vector<bool>>                  testGetValueVec2Bool;
    GetValue<std::vector<int>>                   testGetValueVec2Int;
    GetValue<std::vector<long>>                  testGetValueVec2Long;
    GetValue<std::vector<double>>                testGetValueVec2Double;
    GetValue<std::vector<StringNumber>>          testGetValueVec2StringNumber;
    GetValue<Obj2>                               testGetValueObj2;
    GetValue<Obj3>                               testGetValueObj3;
    GetValue<Country>                            testGetValueCountry;
    GetValue<Perform>                            testGetValuePerform;
    GetValue<Twitter>                            testGetValueTwitter;

    ActionMap                   actionMap;
    mutable TestAction const*   currentRunner;

    public:
    ThorsSerializerTest()
    {
        actionMap["vector-double"]                      = &testVectorDouble;
        actionMap["vector-string"]                      = &testVectorString;

        actionMap["../data/canada.json"]                = &testGetValueCountry;
        actionMap["../data/citm_catalog.json"]          = &testGetValuePerform;
        actionMap["../data/twitter.json"]               = &testGetValueTwitter;

        actionMap["../data/jsonchecker/fail02.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail03.json"]    = &testGetValueMap2String;
        actionMap["../data/jsonchecker/fail04.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail05.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail06.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail07.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail08.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail09.json"]    = &testGetValueMap2Bool;
        actionMap["../data/jsonchecker/fail10.json"]    = &testGetValueMap2Bool;
        actionMap["../data/jsonchecker/fail11.json"]    = &testGetValueMap2Int;
        actionMap["../data/jsonchecker/fail12.json"]    = &testGetValueMap2String;
        actionMap["../data/jsonchecker/fail13.json"]    = &testGetValueMap2Int;
        actionMap["../data/jsonchecker/fail14.json"]    = &testGetValueMap2Int;
        actionMap["../data/jsonchecker/fail15.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail16.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail17.json"]    = &testGetValueVec2String;

        actionMap["../data/jsonchecker/fail19.json"]    = &testGetValueMap2IntPointer;
        actionMap["../data/jsonchecker/fail20.json"]    = &testGetValueMap2IntPointer;
        actionMap["../data/jsonchecker/fail21.json"]    = &testGetValueMap2IntPointer;
        actionMap["../data/jsonchecker/fail22.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail23.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail24.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail25.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail26.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail27.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail28.json"]    = &testGetValueVec2String;
        actionMap["../data/jsonchecker/fail29.json"]    = &testGetValueVec2Double;
        actionMap["../data/jsonchecker/fail30.json"]    = &testGetValueVec2Double;
        actionMap["../data/jsonchecker/fail31.json"]    = &testGetValueVec2Double;
        actionMap["../data/jsonchecker/fail32.json"]    = &testGetValueMap2Bool;
        actionMap["../data/jsonchecker/fail33.json"]    = &testGetValueVec2String;

        //  pass01.json     ThorsSerializer does not support polymorphic arrays
        //actionMap["../data/jsonchecker/pass02.json"]    = &testGetValueVec2L18;
        actionMap["../data/jsonchecker/pass03.json"]    = &testGetValueMap2M01;


        actionMap["../data/roundtrip/roundtrip01.json"] = &testGetValueVec2IntPointer;
        actionMap["../data/roundtrip/roundtrip02.json"] = &testGetValueVec2Bool;
        actionMap["../data/roundtrip/roundtrip03.json"] = &testGetValueVec2Bool;
        actionMap["../data/roundtrip/roundtrip04.json"] = &testGetValueVec2Int;
        actionMap["../data/roundtrip/roundtrip05.json"] = &testGetValueVec2String;
        actionMap["../data/roundtrip/roundtrip06.json"] = &testGetValueVec2Int;
        actionMap["../data/roundtrip/roundtrip07.json"] = &testGetValueMap2String;
        actionMap["../data/roundtrip/roundtrip08.json"] = &testGetValueVec2Int;
        actionMap["../data/roundtrip/roundtrip09.json"] = &testGetValueObj2;
        actionMap["../data/roundtrip/roundtrip10.json"] = &testGetValueObj3;
        actionMap["../data/roundtrip/roundtrip11.json"] = &testGetValueVec2Int;
        actionMap["../data/roundtrip/roundtrip12.json"] = &testGetValueVec2Int;
        actionMap["../data/roundtrip/roundtrip13.json"] = &testGetValueVec2Long;
        actionMap["../data/roundtrip/roundtrip14.json"] = &testGetValueVec2Long;
        actionMap["../data/roundtrip/roundtrip15.json"] = &testGetValueVec2Int;
        actionMap["../data/roundtrip/roundtrip16.json"] = &testGetValueVec2Int;
        actionMap["../data/roundtrip/roundtrip17.json"] = &testGetValueVec2Long;
        actionMap["../data/roundtrip/roundtrip18.json"] = &testGetValueVec2Long;
        actionMap["../data/roundtrip/roundtrip19.json"] = &testGetValueVec2Long;
        actionMap["../data/roundtrip/roundtrip20.json"] = &testGetValueVec2Double;
        actionMap["../data/roundtrip/roundtrip21.json"] = &testGetValueVec2StringNumber;
        actionMap["../data/roundtrip/roundtrip22.json"] = &testGetValueVec2StringNumber;
        actionMap["../data/roundtrip/roundtrip23.json"] = &testGetValueVec2StringNumber;
        actionMap["../data/roundtrip/roundtrip24.json"] = &testGetValueVec2StringNumber;
        actionMap["../data/roundtrip/roundtrip25.json"] = &testGetValueVec2StringNumber;
        actionMap["../data/roundtrip/roundtrip26.json"] = &testGetValueVec2StringNumber;
        actionMap["../data/roundtrip/roundtrip27.json"] = &testGetValueVec2StringNumber;

        currentRunner      = &testNotImplemented;
    }
    virtual void SetUp(char const* fullPath) const
    {
        // std::cout << "Test: " << fullPath << " : " << fileName << "\n";

        auto find = actionMap.find(fullPath);
        if (find != actionMap.end())
        {
            currentRunner   = find->second;
        }
    }
    virtual void TearDown(char const*) const
    {
        currentRunner = &testNotImplemented;
    }

#if TEST_INFO
    virtual const char* GetName() const /*override*/        { return "ThorsSerializer"; }
    virtual const char* GetFilename() const /*override*/    { return __FILE__; }
#endif
#if TEST_PARSE
    virtual ParseResultBase* Parse(const char* json, size_t length) const {
        return currentRunner->Parse(json, length);
    }
#endif
#if TEST_CONFORMANCE
    virtual bool ParseDouble(const char* json, double* d) const {
        return currentRunner->ParseDouble(json, d);
    }

    virtual bool ParseString(const char* json, std::string& s) const {
        return currentRunner->ParseString(json, s);
    }
#endif
#if TEST_SAXROUNDTRIP
    virtual StringResultBase* SaxRoundtrip(const char*, size_t) const {
        return nullptr;
    }
#endif


#if TEST_STRINGIFY
    virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
        return currentRunner->Stringify(parseResult);
    }
#endif

#if TEST_PRETTIFY
    virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
        return currentRunner->Prettify(parseResult);
    }
#endif

#if TEST_STATISTICS
    virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
        stat->objectCount   = 0;
        stat->arrayCount    = 0;
        stat->numberCount   = 0;
        stat->stringCount   = 0;
        stat->trueCount     = 0;
        stat->falseCount    = 0;
        stat->nullCount     = 0;

        stat->memberCount   = 0;
        stat->elementCount  = 0;
        stat->stringLength  = 0;

        return currentRunner->Statistics(parseResult, stat);
    }
#endif

#if TEST_SAXSTATISTICS
    virtual bool SaxStatistics(const char*, size_t, Stat*) const {
        return false;
    }
#endif

#if TEST_SAXSTATISTICSUTF16
    virtual bool SaxStatisticsUTF16(const char*, size_t, Stat*) const {
        return false;
    }
#endif
};

REGISTER_TEST(ThorsSerializerTest);

