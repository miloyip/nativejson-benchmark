#if (defined(__clang__) || defined(__GNUC__)) && defined(__x86_64__)

#include "../test.h"
#include "taocppjson/include/tao/json.hh"

static void GenStat(Stat& stat, const tao::json::value& v){
   switch (v.type()) {
      case tao::json::type::ARRAY:
         for (auto& element : v.get_array()) {
            GenStat(stat, element);
         }
         stat.arrayCount++;
         stat.elementCount += v.get_array().size();
         break;

      case tao::json::type::OBJECT:
         for (auto& element : v.get_object()) {
            GenStat(stat, element.second);
            stat.stringLength += element.first.size();
         }
         stat.objectCount++;
         stat.memberCount += v.get_object().size();
         stat.stringCount += v.get_object().size();
         break;

      case tao::json::type::STRING:
         stat.stringCount++;
         stat.stringLength += v.get_string().size();
         break;

      case tao::json::type::SIGNED:
      case tao::json::type::UNSIGNED:
      case tao::json::type::DOUBLE:
         stat.numberCount++;
         break;

      case tao::json::type::BOOL:
         if (v.get_bool())
            stat.trueCount++;
         else
            stat.falseCount++;
         break;

      case tao::json::type::NULL_:
         stat.nullCount++;
         break;

      case tao::json::type::POINTER:
         // not applicable in this benchmark
         throw std::runtime_error( "code should be unreachable" );
   }
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
   virtual ParseResultBase* Parse(const char* j, size_t length) const {
      (void)length;
      TAOCPPJSONParseResult* pr = new TAOCPPJSONParseResult;
      try {
         pr->root = tao::json::from_string(j);
      }
      catch (...) {
         delete pr;
         return 0;
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

#if TEST_CONFORMANCE
   virtual bool ParseDouble(const char* j, double* d) const {
      try {
         auto root = tao::json::from_string(j);
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
