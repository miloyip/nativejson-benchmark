#include "../test.h"
#include "facil.io/lib/facil/core/types/fiobj.h"

#include <stdio.h>
#include <stdlib.h>

static int GenStat_task(fiobj_s *obj, void *arg) {
  Stat *s = (Stat *)arg;
  switch (obj->type) {
  case FIOBJ_T_NULL:
    s->nullCount++;
    break;
  case FIOBJ_T_TRUE:
    s->trueCount++;
    break;
  case FIOBJ_T_FALSE:
    s->falseCount++;
    break;
  case FIOBJ_T_FLOAT:
  case FIOBJ_T_NUMBER:
    s->numberCount++;
    break;
  case FIOBJ_T_SYMBOL:
  case FIOBJ_T_STRING:
    s->stringCount++;
    s->stringLength += fiobj_obj2cstr(obj).len;
    break;
  case FIOBJ_T_ARRAY:
    s->elementCount += fiobj_ary_count(obj);
    s->arrayCount++;
    break;
  case FIOBJ_T_HASH:
    s->memberCount += fiobj_hash_count(obj);
    s->objectCount++;
    break;
  case FIOBJ_T_COUPLET:
    GenStat_task(fiobj_couplet2key(obj), arg);
    GenStat_task(fiobj_couplet2obj(obj), arg);
    break;
  case FIOBJ_T_IO:
    /* invalid */
    return -1;
    break;
  }
  return 0;
}

class FacilParseResult : public ParseResultBase {
public:
  FacilParseResult() : root() {}
  ~FacilParseResult() { fiobj_free(root); }

  fiobj_s *root;
};

class FacilStringResult : public StringResultBase {
public:
  FacilStringResult() : str() {}
  ~FacilStringResult() { fiobj_free(str); }

  virtual const char *c_str() const { return fiobj_obj2cstr(str).data; }

  fiobj_s *str;
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
    fiobj_json2obj(&pr->root, json, length);
    if (pr->root == nullptr) {
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
    fiobj_s *tmp;
    fiobj_json2obj(&tmp, json, 999);
    if (tmp == NULL)
      return false;
    if (tmp->type == FIOBJ_T_FLOAT) {
      *d = fiobj_obj2float(tmp);
      fiobj_free(tmp);
      return true;
    }
    if (tmp->type == FIOBJ_T_ARRAY && fiobj_ary_entry(tmp, 0) &&
        fiobj_ary_entry(tmp, 0)->type == FIOBJ_T_FLOAT) {
      *d = fiobj_obj2float(fiobj_ary_entry(tmp, 0));
      fiobj_free(tmp);
      return true;
    }
    fiobj_free(tmp);
    return false;
  }

  virtual bool ParseString(const char *json, std::string &s) const {
    fiobj_s *tmp;
    fiobj_json2obj(&tmp, json, 999);
    if (tmp == NULL)
      return false;

    if (tmp->type == FIOBJ_T_STRING) {
      s = std::string(fiobj_obj2cstr(tmp).data, fiobj_obj2cstr(tmp).len + 1);
      fiobj_free(tmp);
      return true;
    }
    if (tmp->type == FIOBJ_T_ARRAY && fiobj_ary_entry(tmp, 0) &&
        fiobj_ary_entry(tmp, 0)->type == FIOBJ_T_STRING) {
      s = std::string(fiobj_obj2cstr(fiobj_ary_entry(tmp, 0)).data,
                      fiobj_obj2cstr(fiobj_ary_entry(tmp, 0)).len);
      fiobj_free(tmp);
      tmp = nullptr;
      return true;
    }
    fiobj_free(tmp);
    tmp = nullptr;
    return false;
  }
#endif
};

REGISTER_TEST(FacilTest);
