#include "../test.h"
#include "workflow-json-parser/json_parser.h"

static void GenStat(Stat *s, const json_value_t *val)
{
	const json_object_t *obj;
	const json_array_t *arr;
	const char *name;

	switch (json_value_type(val))
	{
	case JSON_VALUE_OBJECT:
		obj = json_value_object(val);
		json_object_for_each(name, val, obj)
		{
			GenStat(s, val);
			s->stringCount++;
			s->stringLength += strlen(name);
			s->memberCount++;
		}

		s->objectCount++;
		break;

	case JSON_VALUE_ARRAY:
		arr = json_value_array(val);
		json_array_for_each(val, arr)
		{
			GenStat(s, val);
			s->elementCount++;
		}

		s->arrayCount++;
		break;

	case JSON_VALUE_STRING:
		s->stringLength += strlen(json_value_string(val));
		s->stringCount++;
		break;

	case JSON_VALUE_TRUE:
		s->trueCount++;
		break;

	case JSON_VALUE_FALSE:
		s->falseCount++;
		break;

	case JSON_VALUE_NULL:
		s->nullCount++;
		break;
	}
}

class workflowjsonParseResult : public ParseResultBase
{
public:
	workflowjsonParseResult() : root(NULL) { }

	~workflowjsonParseResult()
	{
		if (root)
			json_value_destroy(root);
	}

	json_value_t *root;
};

class workflowjsonTest : public TestBase
{
public:
#if TEST_INFO
	virtual const char *GetName() const { return "C++ Workflow JSON (C)"; }
	virtual const char *GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
	virtual ParseResultBase *Parse(const char *json, size_t) const
	{
		json_value_t *root = json_value_parse(json);
		workflowjsonParseResult *res;

		if (root)
		{
			res = new workflowjsonParseResult;
			res->root = root;
		}
		else
			res = NULL;

		return res;
	}
#endif

#if TEST_STATISTICS
	virtual bool Statistics(const ParseResultBase *parseResult, Stat *stat) const
	{
		const workflowjsonParseResult *res = (const workflowjsonParseResult *)(parseResult);
		memset(stat, 0, sizeof (Stat));
		GenStat(stat, res->root);
		return true;
	}
#endif

#if TEST_CONFORMANCE
	virtual bool ParseDouble(const char *json, double *d) const
	{
		workflowjsonParseResult res;

		res.root = json_value_parse(json);
		if (res.root && json_value_type(res.root) == JSON_VALUE_ARRAY)
		{
			const json_array_t *arr = json_value_array(res.root);
			const json_value_t *val = json_array_next_value(NULL, arr);

			if (val && json_value_type(val) == JSON_VALUE_NUMBER)
			{
				*d = json_value_number(val);
				return true;
			}
		}

		return false;
	}

	virtual bool ParseString(const char *json, std::string& s) const
	{
		workflowjsonParseResult res;

		res.root = json_value_parse(json);
		if (res.root && json_value_type(res.root) == JSON_VALUE_ARRAY)
		{
			const json_array_t *arr = json_value_array(res.root);
			const json_value_t *val = json_array_next_value(NULL, arr);

			if (val && json_value_type(val) == JSON_VALUE_STRING)
			{
				s = json_value_string(val);
				return true;
			}
		}

		return false;
	}
#endif
};

REGISTER_TEST(workflowjsonTest);
