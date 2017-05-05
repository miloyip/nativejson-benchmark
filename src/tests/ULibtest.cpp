#include "../test.h"

/**
 * NB: if ULib is configured and compiled (./configure --disable-shared && make) use this way:
 *
 * #define HAVE_CONFIG_H
 * #include <ulib/json/value.h>
 * #undef HAVE_CONFIG_H
 *
 * otherwise:
 *
 * #include <ULib/src/ulib/all_cpp.cpp>
 */

#define HAVE_CONFIG_H
#include <ulib/json/value.h>
#undef HAVE_CONFIG_H

static ULib ulib("167193,0,0,0,-30,-31,-30,-31,0");

static void GenStat(Stat& stat, const union UValue::jval val)
{
	U_TRACE(5, "::GenStat(%p,0x%x)", &stat, val.ival)

	switch (UValue::getTag(val.ival))
		{
		case UValue::REAL_VALUE:
		case UValue::INT_VALUE:
		case UValue::UINT_VALUE:  stat.numberCount++; break;
		case UValue::TRUE_VALUE:  stat.trueCount++;   break;
		case UValue::FALSE_VALUE: stat.falseCount++;  break;
		case UValue::NULL_VALUE:  stat.nullCount++;   break;

		case UValue::STRING_VALUE:
		case UValue::UTF_VALUE:
			{
			stat.stringCount++;

			stat.stringLength += UValue::getStringSize(val);
			}
		break;

		case UValue::ARRAY_VALUE:
			{
			stat.arrayCount++;

			for (auto const& i : val)
				{
				stat.elementCount++;

				GenStat(stat, i.getValue());
				}
			}
		break;

		case UValue::OBJECT_VALUE:
			{
			stat.objectCount++;

			for (auto const& i : val)
				{
				stat.memberCount++;
				stat.stringCount++; // Key
				stat.stringLength += UValue::getStringSize(i.getKey());

				GenStat(stat, i.getValue());
				}
			}
		break;
		}
}

class ULibParseResult : public ParseResultBase {
public:
	UString s;
	UValue json;
};

class ULibStringResult : public StringResultBase {
public:
	virtual const char* c_str() const { return s.data(); }

	UString s;
};

class ULibTest : public TestBase {
public:
#if TEST_INFO
	virtual const char* GetName() const { return "ULib (C++)"; }
	virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_PARSE
	virtual ParseResultBase* Parse(const char* json, size_t length) const
		{
		ULibParseResult* pr = new ULibParseResult;

		return (pr->json.parse((pr->s = UString(json, length))) ? pr : (delete pr, (ULibParseResult*)0));
		}
#endif

#if TEST_STRINGIFY
	virtual StringResultBase* Stringify(const ParseResultBase* parseResult) const
		{
		ULibStringResult* sr = new ULibStringResult;

		sr->s = ((const ULibParseResult*)parseResult)->json.output();

		return sr;
		}
#endif

#if TEST_PRETTIFY
	virtual StringResultBase* Prettify(const ParseResultBase* parseResult) const
		{
		ULibStringResult* sr = new ULibStringResult;

		sr->s = ((const ULibParseResult*)parseResult)->json.prettify();

		return sr;
		}
#endif

#if TEST_STATISTICS
	virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const
		{
		(void) memset(stat, 0, sizeof(Stat));

		const ULibParseResult* pr = static_cast<const ULibParseResult*>(parseResult);

		GenStat(*stat, pr->json.getValue());

		return true;
		}
#endif

#if TEST_CONFORMANCE
	virtual bool ParseDouble(const char* json, double* d) const
		{
		UValue v;

		if (v.parse(UString(json)))
			{
			*d = v.at(0)->getDouble();

			return true;
			}

		return false;
		}

	virtual bool ParseString(const char* json, std::string& s) const
		{
		UValue v;

		if (v.parse(UString(json)))
			{
			UString result = v.at(0)->getString();

			(void) s.assign(U_STRING_TO_PARAM(result));

			return true;
			}

		return false;
		}
#endif
};

REGISTER_TEST(ULibTest);
