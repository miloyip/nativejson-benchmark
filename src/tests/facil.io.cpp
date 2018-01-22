#include "../test.h"
#include "facil.io/lib/facil/core/types/fiobj.h"

#include <stdio.h>
#include <stdlib.h>

static int GenStat_task(FIOBJ obj, void *arg) {
  Stat *s = (Stat *)arg;
  if (fiobj_hash_key_in_loop()) {
    s->stringCount++;
    s->stringLength += fiobj_obj2cstr(fiobj_hash_key_in_loop()).len;
  }
  switch (FIOBJ_TYPE(obj)) {
  case FIOBJ_T_NULL:
    s->nullCount++;
    break;
  case FIOBJ_T_STRING:
  case FIOBJ_T_DATA:
  case FIOBJ_T_UNKNOWN:
    s->stringCount++;
    s->stringLength += fiobj_obj2cstr(obj).len;
    break;
  case FIOBJ_T_FLOAT:
  case FIOBJ_T_NUMBER:
    s->numberCount++;
    break;
  case FIOBJ_T_ARRAY:
    s->elementCount += fiobj_ary_count(obj);
    s->arrayCount++;
    break;
  case FIOBJ_T_HASH:
    s->memberCount += fiobj_hash_count(obj);
    s->objectCount++;
    break;
  case FIOBJ_T_TRUE:
    s->trueCount++;
    break;
  case FIOBJ_T_FALSE:
    s->falseCount++;
    break;
  }
  return 0;
}

class FacilParseResult : public ParseResultBase {
public:
  FacilParseResult() : root() {}
  ~FacilParseResult() { fiobj_free(root); }

  FIOBJ root;
};

class FacilStringResult : public StringResultBase {
public:
  FacilStringResult() : str() {}
  ~FacilStringResult() { fiobj_free(str); }

  virtual const char *c_str() const { return fiobj_obj2cstr(str).data; }

  FIOBJ str;
};

class FacilTest : public TestBase {
public:
#if TEST_INFO
  virtual const char *GetName() const { return "facil.io (C)"; }
  virtual const char *GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
  virtual ParseResultBase *Parse(const char *json, size_t length) const {
    (void)length;
    FacilParseResult *pr = new FacilParseResult;
    size_t consumed = fiobj_json2obj(&pr->root, json, length);
    if (pr->root == 0) {
      delete pr;
      return nullptr;
    }
    if (consumed < length &&
        (json[consumed] != ' ' && json[consumed] != 0x09 &&
         json[consumed] != 0x0A && json[consumed] != 0x0D &&
         json[consumed] != 0x20 && json[consumed] != 0x2C)) {
      delete pr;
      return nullptr;
    }
    return pr;
  }
#endif

#if TEST_STRINGIFY
  virtual StringResultBase *
  Stringify(const ParseResultBase *parseResult) const {
    const FacilParseResult *pr =
        static_cast<const FacilParseResult *>(parseResult);
    FacilStringResult *sr = new FacilStringResult;
    sr->str = fiobj_obj2json(pr->root, 0);
    return sr;
  }
#endif

#if TEST_PRETTIFY
  /* Not supported */
  virtual StringResultBase *Prettify(const ParseResultBase *parseResult) const {
    const FacilParseResult *pr =
        static_cast<const FacilParseResult *>(parseResult);
    FacilStringResult *sr = new FacilStringResult;
    sr->str = fiobj_obj2json(pr->root, 1);
    return sr;
  }
#endif

#if TEST_STATISTICS
  virtual bool Statistics(const ParseResultBase *parseResult,
                          Stat *stat) const {
    const FacilParseResult *pr =
        static_cast<const FacilParseResult *>(parseResult);
    memset(stat, 0, sizeof(Stat));
    fiobj_each2(pr->root, GenStat_task, stat);
    return true;
  }
#endif

#if TEST_CONFORMANCE
  virtual bool ParseDouble(const char *json, double *d) const {
    FIOBJ tmp = 0;
    if (!fiobj_json2obj(&tmp, json, 999) || tmp == 0)
      return false;
    if (FIOBJ_TYPE_IS(tmp, FIOBJ_T_FLOAT)) {
      *d = fiobj_obj2float(tmp);
      fiobj_free(tmp);
      return true;
    }
    if (FIOBJ_TYPE_IS(tmp, FIOBJ_T_ARRAY) && fiobj_ary_entry(tmp, 0) &&
        FIOBJ_TYPE_IS(fiobj_ary_entry(tmp, 0), FIOBJ_T_FLOAT)) {
      *d = fiobj_obj2float(fiobj_ary_entry(tmp, 0));
      fiobj_free(tmp);
      return true;
    }
    fiobj_free(tmp);
    return false;
  }

  virtual bool ParseString(const char *json, std::string &s) const {
    FIOBJ tmp = 0;
    fiobj_json2obj(&tmp, json, 999);
    if (tmp == 0)
      return false;

    if (FIOBJ_TYPE_IS(tmp, FIOBJ_T_STRING)) {
      s = std::string(fiobj_obj2cstr(tmp).data, fiobj_obj2cstr(tmp).len + 1);
      fiobj_free(tmp);
      return true;
    }
    if (FIOBJ_TYPE_IS(tmp, FIOBJ_T_ARRAY) && fiobj_ary_entry(tmp, 0) &&
        FIOBJ_TYPE_IS(fiobj_ary_entry(tmp, 0), FIOBJ_T_STRING)) {
      s = std::string(fiobj_obj2cstr(fiobj_ary_entry(tmp, 0)).data,
                      fiobj_obj2cstr(fiobj_ary_entry(tmp, 0)).len);
      fiobj_free(tmp);
      tmp = 0;
      return true;
    }
    fiobj_free(tmp);
    tmp = 0;
    return false;
  }
#endif
};

REGISTER_TEST(FacilTest);
