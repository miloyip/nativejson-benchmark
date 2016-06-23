#include "../test.h"
#include "pjson.h"
#include <array>
#include <vector>

class PJsonTest : public TestBase {
public:
#if TEST_INFO
	virtual const char* GetName() const { return "pjson (C)"; }
	virtual const char* GetFilename() const { return __FILE__; }
#endif

#if TEST_SAXSTATISTICS
	virtual bool SaxStatistics(const char* json, size_t length, Stat* stat) const {
		memset(stat, 0, sizeof(Stat));

		pj_parser parser;

		std::vector<char> buffer(256);
		pj_init(&parser, buffer.data(), buffer.size());

		pj_feed(&parser, json, length);

		bool after_key = false;
#define COUNT_ELEMENTS() (void)(after_key ? after_key = false : ++stat->elementCount)
		for (;;) {
			std::array<pj_token, 128> tokens;
			pj_poll(&parser, tokens.data(), tokens.size());
			for (size_t i = 0; i < tokens.size();) {
				auto &token = tokens[i++];
				switch (token.token_type) {
				/* technical events */
				case PJ_STARVING:
					pj_feed_end(&parser);
					pj_poll(&parser, tokens.data(), tokens.size());
					i = 0;
					continue;
				case PJ_OVERFLOW:
					buffer.resize(buffer.size() + token.len);
					pj_realloc(&parser, buffer.data(), buffer.size());
					pj_poll(&parser, tokens.data(), tokens.size());
					i = 0;
					continue;
				case PJ_END:
					return true;

				/* actual JSON events */
				case PJ_TOK_NULL:
					stat->nullCount++;
					COUNT_ELEMENTS();
					break;
				case PJ_TOK_TRUE:
					stat->trueCount++;
					COUNT_ELEMENTS();
					break;
				case PJ_TOK_STR:
					stat->stringCount++;
					stat->stringLength += token.len;
					COUNT_ELEMENTS();
					break;
				case PJ_TOK_NUM:
					stat->numberCount++;
					COUNT_ELEMENTS();
					break;
				case PJ_TOK_MAP:
					COUNT_ELEMENTS();
					break;
				case PJ_TOK_KEY:
					stat->memberCount++;
					stat->stringCount++;
					stat->stringLength += token.len;
					after_key = true;
					break;
				case PJ_TOK_MAP_E:
					stat->objectCount++;
					break;
				case PJ_TOK_ARR:
					COUNT_ELEMENTS();
					break;
				case PJ_TOK_ARR_E:
					stat->arrayCount++;
					break;

				case PJ_ERR:
				default:
					return false;
				}
			}
		}
	}
#endif

#if TEST_CONFORMANCE
	virtual bool ParseDouble(const char* json, double* d) const {
		pj_parser parser;

		std::vector<char> buffer(256);
		pj_init(&parser, buffer.data(), buffer.size());

		pj_feed(&parser, json, strlen(json));
		for (size_t step = 0;;) {
			std::array<pj_token, 3> tokens;
			pj_poll(&parser, tokens.data(), tokens.size());
			for (size_t i = 0;;) {
				auto &token = tokens[i++];

				switch (token.token_type) {
				/* technical events */
				case PJ_STARVING:
					pj_feed_end(&parser);
					pj_poll(&parser, tokens.data(), tokens.size());
					i = 0;
					continue;
				case PJ_OVERFLOW:
					buffer.resize(buffer.size() + token.len);
					pj_realloc(&parser, buffer.data(), buffer.size());
					pj_poll(&parser, tokens.data(), tokens.size());
					i = 0;
					continue;
				case PJ_END:
					return step == 3;

				case PJ_TOK_ARR:
					if (step++ != 0) return false;
					break;
				case PJ_TOK_NUM:
					if (step++ != 1) return false;
					*d = atof(std::string(token.str, token.len).c_str());
					break;
				case PJ_TOK_ARR_E:
					if (step++ != 2) return false;
					break;

				case PJ_ERR:
				default:
					return false;
				}
			}
		}
	}

	virtual bool ParseString(const char* json, std::string& s) const {
		pj_parser parser;

		std::vector<char> buffer(256);
		pj_init(&parser, buffer.data(), buffer.size());

		pj_feed(&parser, json, strlen(json));
		for (size_t step = 0;;) {
			std::array<pj_token, 3> tokens;
			pj_poll(&parser, tokens.data(), tokens.size());
			for (size_t i = 0;;) {
				auto &token = tokens[i++];

				switch (token.token_type) {
				/* technical events */
				case PJ_STARVING:
					pj_feed_end(&parser);
					pj_poll(&parser, tokens.data(), tokens.size());
					i = 0;
					continue;
				case PJ_OVERFLOW:
					buffer.resize(buffer.size() + token.len);
					pj_realloc(&parser, buffer.data(), buffer.size());
					pj_poll(&parser, tokens.data(), tokens.size());
					i = 0;
					continue;
				case PJ_END:
					return step == 3;

				case PJ_TOK_ARR:
					if (step++ != 0) return false;
					break;
				case PJ_TOK_STR:
					if (step++ != 1) return false;
					s.assign(token.str, token.len);
					break;
				case PJ_TOK_ARR_E:
					if (step++ != 2) return false;
					break;

				case PJ_ERR:
				default:
					return false;
				}
			}
		}
	}
#endif
};

REGISTER_TEST(PJsonTest);
