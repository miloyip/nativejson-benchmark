#include <sstream>
#include <string>
#include <string_view>

#include "../test.h"
#include "simdjson.cpp"
#include "simdjson.h"

using namespace simdjson;

static void GenStat(Stat &stat, const dom::element v) {
  switch (v.type()) {
    case dom::element_type::ARRAY:
      for (dom::element child : dom::array(v)) {
        stat.elementCount++;
        GenStat(stat, child);
      }
      stat.arrayCount++;
      break;
    case dom::element_type::OBJECT:
      for (dom::key_value_pair kv : dom::object(v)) {
        GenStat(stat, kv.value);
        stat.stringLength += kv.key.size();
        stat.memberCount++;
        stat.stringCount++;
      }
      stat.objectCount++;
      break;
    case dom::element_type::INT64:
    case dom::element_type::UINT64:
    case dom::element_type::DOUBLE:
      stat.numberCount++;
      break;
    case dom::element_type::STRING: {
      stat.stringCount++;
      std::string_view sv = v.get<std::string_view>();
      stat.stringLength += sv.size();
    } break;
    case dom::element_type::BOOL:
      if (v.get<bool>()) {
        stat.trueCount++;
      } else {
        stat.falseCount++;
      }
      break;
    case dom::element_type::NULL_VALUE:
      ++stat.nullCount;
      break;
  }
}

class SimdJsonParseResult : public ParseResultBase {
 public:
  dom::element root{};
  std::unique_ptr<dom::parser> parser = std::make_unique<dom::parser>();
};

class SimdStringResult : public StringResultBase {
 public:
  std::stringstream ss;
  const char *c_str() const override { return ss.str().c_str(); }
};
class SimdTest : public TestBase {
 public:
#if TEST_INFO
  const char *GetName() const override { return "simdjson"; }
  const char *GetFilename() const override { return __FILE__; }
#endif

#if TEST_PARSE
  ParseResultBase *Parse(const char *j, size_t length) const override {
    auto pr = std::make_unique<SimdJsonParseResult>();

    std::string_view s(j, length);
    simdjson::error_code error;
    pr->parser->parse(s).tie(pr->root, error);
    if (error) {
      return nullptr;
    }
    return pr.release();
  }
#endif

#if TEST_STRINGIFY
  StringResultBase *Stringify(
      const ParseResultBase *parseResult) const override {
    auto sr = std::make_unique<SimdStringResult>();

    auto pr = static_cast<const SimdJsonParseResult *>(parseResult);
    sr->ss << pr->root;
    return sr.release();
  }
#endif

#if TEST_PRETTIFY
  // Currently unsupported, simdjson v0.3
  StringResultBase *Prettify(
      const ParseResultBase *parseResult) const override {
    (void)parseResult;
    return nullptr;
  }
#endif

#if TEST_STATISTICS
  bool Statistics(const ParseResultBase *parseResult,
                  Stat *stat) const override {
    auto pr = static_cast<const SimdJsonParseResult *>(parseResult);
    memset(stat, 0, sizeof(Stat));
    GenStat(*stat, pr->root);
    return true;
  }
#endif

#if TEST_CONFORMANCE
  bool ParseDouble(const char *j, double *d) const override {
    simdjson::error_code error;
    simdjson::dom::parser parser;
    parser.parse(j, std::strlen(j)).at(0).get<double>().tie(*d, error);
    if (error) {
      return false;
    }
    return true;
  }

  bool ParseString(const char *j, std::string &s) const override {
    simdjson::error_code error;
    std::string_view answer;
    parser.parse(j,strlen(j))
        .at(0)
        .get<std::string_view>()
        .tie(answer, error);
    if (error) {
      return false;
    }
    s.assign(answer.data(), answer.size());
    return true;
  }
#endif
};

REGISTER_TEST(SimdTest);

