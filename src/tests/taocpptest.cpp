#if (defined(__clang__) || defined(__GNUC__)) && defined(__x86_64__)

#include "../test.h"

#include "taocppjson/include/tao/json.hpp"

class StatHandler
{
public:
    StatHandler(Stat& stat) : stat_(stat) {}

    void null() { stat_.nullCount++; }

    void boolean(const bool v) { v ? stat_.trueCount++ : stat_.falseCount++; }

    void number(const std::int64_t) { stat_.numberCount++; }
    void number(const std::uint64_t) { stat_.numberCount++; }
    void number(const double) { stat_.numberCount++; }

    void string(const std::string & v) { stat_.stringCount++; stat_.stringLength += v.size(); }

    void begin_array() {}
    void element() { stat_.elementCount++; }
    void end_array() { stat_.arrayCount++; }

    void begin_object() {}
    void key(const std::string & v) { stat_.stringCount++; stat_.stringLength += v.size(); }
    void member() { stat_.memberCount++; }
    void end_object() { stat_.objectCount++; }

private:
    StatHandler& operator=(const StatHandler&);

    Stat& stat_;
};

static void GenStat(Stat& stat, const tao::json::value& v){
   StatHandler statHandler(stat);
   tao::json::sax::from_value(v, statHandler);
}

class TAOCPPJSONParseResult : public ParseResultBase {
public:
   tao::json::value root;
};

class TAOCPPJSONStringResult : public StringResultBase {
public:
   virtual const char* c_str() const { return s.c_str(); }

   std::string s;
};
class TAOCPPJSONTest : public TestBase {
public:
#if TEST_INFO
   virtual const char* GetName() const { return "taocpp/json (C++11)"; }
   virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
   virtual ParseResultBase* Parse(const char* json, size_t length) const {
      TAOCPPJSONParseResult* pr = new TAOCPPJSONParseResult;
      try {
         pr->root = tao::json::from_string(json, length);
      }
      catch (...) {
         delete pr;
         return nullptr;
      }
      return pr;
   }
#endif

#if TEST_STRINGIFY
   virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const {
      const TAOCPPJSONParseResult* pr = static_cast<const TAOCPPJSONParseResult*>(parseResult);
      TAOCPPJSONStringResult* sr = new TAOCPPJSONStringResult;
      sr->s = to_string(pr->root);
      return sr;
   }
#endif

#if TEST_PRETTIFY
   virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const {
      const TAOCPPJSONParseResult* pr = static_cast<const TAOCPPJSONParseResult*>(parseResult);
      TAOCPPJSONStringResult* sr = new TAOCPPJSONStringResult;
      sr->s = to_string(pr->root, 4);
      return sr;
   }
#endif

#if TEST_STATISTICS
   virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
      const TAOCPPJSONParseResult* pr = static_cast<const TAOCPPJSONParseResult*>(parseResult);
      memset(stat, 0, sizeof(Stat));
      GenStat(*stat, pr->root);
      return true;
   }
#endif

#if TEST_SAXROUNDTRIP
    virtual StringResultBase* SaxRoundtrip(const char* json, size_t length) const {
        TAOCPPJSONStringResult* sr = new TAOCPPJSONStringResult;
        std::ostringstream oss;
        tao::json::sax::to_stream handler( oss );
        tao::json::sax::from_string(json, length, handler);
        sr->s = oss.str();
        return sr;
    }
#endif

#if TEST_SAXSTATISTICS
    virtual bool SaxStatistics(const char* json, size_t length, Stat* stat) const {
        memset(stat, 0, sizeof(Stat));
        StatHandler handler(*stat);
        tao::json::sax::from_string(json, length, handler);
        return true;
    }
#endif

#if TEST_CONFORMANCE
   virtual bool ParseDouble(const char* json, double* d) const {
      try {
         auto root = tao::json::from_string(json);
         *d = root[0].get_double();
         return true;
      }
      catch (...) {
      }
      return false;
   }

   virtual bool ParseString(const char* j, std::string& s) const {
      try {
         auto root = tao::json::from_string(j);
         s = root[0].get_string();
         return true;
      }
      catch (...) {
      }
      return false;
   }
#endif
};

REGISTER_TEST(TAOCPPJSONTest);

#endif
