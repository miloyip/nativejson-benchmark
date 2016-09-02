#include "../test.h"

#if !defined(_MSC_VER)

#include "pjson.h"
#include <array>
#include <vector>
#include <memory>
#include <clocale>

namespace {
	struct PJsonParser {
		pj_parser parser;
		std::vector<char> buffer;

		PJsonParser(size_t buffer_size = 256) : buffer(buffer_size) {
			reset();
		}
		void reset() { pj_init(&parser, buffer.data(), buffer.size()); }
		void feed(const char* data, size_t size) { pj_feed(&parser, data, size); }
		void feed(const char* data) { feed(data, strlen(data)); }
		void feed_end() { pj_feed_end(&parser); }
		void poll(pj_token* tokens, size_t size) { pj_poll(&parser, tokens, size); }
		void realloc(size_t size) {
			buffer.resize(size);
			pj_realloc(&parser, buffer.data(), size);
		}

		template <pj_token_type TokenType, class F>
		bool array_with_one_elem(F handler) {
			for (size_t step = 0;;) {
				std::array<pj_token, 3> tokens;
				poll(tokens.data(), tokens.size());
				for (size_t i = 0; i < tokens.size();) {
					auto &token = tokens[i++];

					switch (token.token_type) {
					/* technical events */
					case PJ_STARVING:
						feed_end();
						poll(tokens.data(), tokens.size());
						i = 0;
						continue;
					case PJ_OVERFLOW:
						realloc(std::min(buffer.size(), token.len));
						poll(tokens.data(), tokens.size());
						i = 0;
						continue;
					case PJ_END:
						return step == 3;

					case PJ_TOK_ARR:
						if (step++ != 0) return false;
						break;
					case TokenType:
						if (step++ != 1) return false;
						handler(token);
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
	};

	struct PJsonParseResult : ParseResultBase {
		Stat stat;
	};
} // anonymous namespace

class PJsonTest : public TestBase {
public:
#if TEST_INFO
	virtual const char* GetName() const { return "pjson (C)"; }
	virtual const char* GetFilename() const { return __FILE__; }
#endif

// pjson does not support parsing to DOM, so Parse() and Statistics() are disabled.

// #if TEST_PARSE
//     virtual ParseResultBase* Parse(const char* json, size_t length) const {
// 	    std::unique_ptr<PJsonParseResult> pr(new PJsonParseResult);
// 		if (SaxStatistics(json, length, &pr->stat))
// 			return pr.release();
// 		else
// 			return nullptr;
// 	}
// #endif

// #if TEST_STATISTICS
//     virtual bool Statistics(const ParseResultBase* parseResult, Stat* stat) const {
// 	    const PJsonParseResult* pr = static_cast<const PJsonParseResult*>(parseResult);
//         *stat = pr->stat;
//         return true;
//     }
// #endif

#if /*TEST_PARSE ||*/ TEST_SAXSTATISTICS
	virtual bool SaxStatistics(const char* json, size_t length, Stat* stat) const {
		setlocale(LC_CTYPE, "en_US.utf8");
		memset(stat, 0, sizeof(Stat));

		PJsonParser parser;

		parser.feed(json, length);

		bool after_key = true;
#define COUNT_ELEMENTS() (void)(after_key ? after_key = false : ++stat->elementCount)
		for (;;) {
			std::array<pj_token, 128> tokens;
			parser.poll(tokens.data(), tokens.size());
			for (size_t i = 0; i < tokens.size();) {
				auto &token = tokens[i++];
				switch (token.token_type) {
				/* technical events */
				case PJ_STARVING:
					parser.feed_end();
					parser.poll(tokens.data(), tokens.size());
					i = 0;
					continue;
				case PJ_OVERFLOW:
					parser.realloc(std::max(parser.buffer.size(), token.len));
					parser.poll(tokens.data(), tokens.size());
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
				case PJ_TOK_FALSE:
					stat->falseCount++;
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
				case PJ_TOK_KEY: // token for ':' (always follwed after PJ_TOK_STR)
					stat->memberCount++;
					--stat->elementCount; // we did counted PJ_TOK_STR (key) as an element already
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
	bool ParseDouble(const char* json, double* d) const override {
		PJsonParser parser;
		parser.feed(json);
		return parser.array_with_one_elem<PJ_TOK_NUM>([d](pj_token &tok) {
			*d = atof(std::string(tok.str, tok.len).c_str());
		});
	}

	bool ParseString(const char* json, std::string& s) const override {
		setlocale(LC_CTYPE, "en_US.utf8");
		PJsonParser parser;
		parser.feed(json);
		return parser.array_with_one_elem<PJ_TOK_STR>([&s](pj_token &tok) {
			s.assign(tok.str, tok.len);
		});
	}
#endif
};

REGISTER_TEST(PJsonTest);

#endif
