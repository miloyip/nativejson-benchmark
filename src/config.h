#pragma once

// Define this if installed boost
#ifndef HAS_BOOST
#define HAS_BOOST 0
#endif

// Define this if installed POCO
#ifndef HAS_POCO
#define HAS_POCO 0
#endif

// Define this to profile memory usage in tests
#ifndef USE_MEMORYSTAT
#define USE_MEMORYSTAT 1
#endif

// Define functions to be tested.
// These are also needed to reduce unncessary code in code size benchmark.
#ifndef TEST_PARSE
#define TEST_PARSE 1
#endif

#ifndef TEST_STRINGIFY
#define TEST_STRINGIFY 1
#endif

#ifndef TEST_PRETTIFY
#define TEST_PRETTIFY 1
#endif

#ifndef TEST_STATISTICS
#define TEST_STATISTICS 1
#endif

#ifndef TEST_SAXROUNDTRIP
#define TEST_SAXROUNDTRIP 1
#endif

#ifndef TEST_SAXSTATISTICS
#define TEST_SAXSTATISTICS 1
#endif

#ifndef TEST_SAXSTATISTICSUTF16
#define TEST_SAXSTATISTICSUTF16 1
#endif

#ifndef TEST_CONFORMANCE
#define TEST_CONFORMANCE 1
#endif

#ifndef TEST_INFO
#define TEST_INFO 1
#endif
