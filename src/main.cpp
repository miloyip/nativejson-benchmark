#include <algorithm>
#include <cfloat>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#if defined(_MSC_VER) || defined(__CYGWIN__)
#include <process.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#endif
#include "test.h"
#include "timer.h"
#include "resultfilename.h"
#include "rapidjson/internal/ieee754.h"

static const unsigned cTrialCount = 10;
static const char* gProgramName;

struct TestJson {
    TestJson() : filename(), json(), length(), stat(), statUTF16() {}

    char* fullpath;
    char* filename;
    char* json;
    size_t length;
    Stat stat;           // Reference statistics
    Stat statUTF16;      // Reference statistics
};

typedef std::vector<TestJson> TestJsonList;

static void PrintStat(const Stat& stat) {
    printf("objectCount:  %10u\n", (unsigned)stat.objectCount);
    printf("arrayCount:   %10u\n", (unsigned)stat.arrayCount);
    printf("numberCount:  %10u\n", (unsigned)stat.numberCount);
    printf("stringCount:  %10u\n", (unsigned)stat.stringCount);
    printf("trueCount:    %10u\n", (unsigned)stat.trueCount);
    printf("falseCount:   %10u\n", (unsigned)stat.falseCount);
    printf("nullCount:    %10u\n", (unsigned)stat.nullCount);
    printf("memberCount:  %10u\n", (unsigned)stat.memberCount);
    printf("elementCount: %10u\n", (unsigned)stat.elementCount);
    printf("stringLength: %10u\n", (unsigned)stat.stringLength);
}

#if USE_MEMORYSTAT
static void PrintMemoryStat() {
    const MemoryStat& stat = Memory::Instance().GetStat();
    printf(
        "Memory stats:\n"
        " mallocCount = %u\n"
        "reallocCount = %u\n"
        "   freeCount = %u\n"
        " currentSize = %u\n"
        "    peakSize = %u\n",
        (unsigned)stat.mallocCount,
        (unsigned)stat.reallocCount,
        (unsigned)stat.freeCount,
        (unsigned)stat.currentSize,
        (unsigned)stat.peakSize);
}
#endif

static char* ReadJSON(FILE *fp, size_t* length) {
    fseek(fp, 0, SEEK_END);
    *length = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* json = (char*)malloc(*length + 1);
    fread(json, 1, *length, fp);
    json[*length] = '\0';
    fclose(fp);
    return json;
}

static char* ReadJSON(const char* filename, size_t* length) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        char buffer[FILENAME_MAX];
        sprintf(buffer, "../%s", filename);
        fp = fopen(buffer, "rb");
        if (!fp)
            return 0;
    }
    return ReadJSON(fp, length);
}

static void makeValidFilename(char *filename) {
    while (*filename) {
        switch (*filename) {
            case '/':
                *filename = '_';
                break;
        }
        filename++;
    }
}

static void EscapeString(FILE* fp, const char *s, size_t length) {
    fputc('"', fp);
    for (size_t j = 0; j < length; j++)
        if (s[j] != 0)
            fputc(s[j], fp);
        else
            fputs("\\0", fp);
    fputc('"', fp);
}

static bool ReadFiles(const char* path, TestJsonList& testJsons) {
    char fullpath[FILENAME_MAX];
    sprintf(fullpath, path, "data.txt");
    FILE* fp = fopen(fullpath, "r");
    if (!fp)
        return false;

    // Currently use RapidJSON to generate reference statistics
    TestList& tests = TestManager::Instance().GetTests();
    const TestBase* referenceTest = 0;
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr) {
        if (strcmp((*itr)->GetName(), "RapidJSON (C++)") == 0) {
            referenceTest = *itr;
            break;
        }
    }

    if (!referenceTest) {
        printf("Error: Cannot find RapidJSON as refernce test. Not reading any files.\n");
        fclose(fp);
        return false;
    }

    while (!feof(fp)) {
        char filename[FILENAME_MAX];
        if (fscanf(fp, "%s", filename) == 1) {
            sprintf(fullpath, path, filename);
            FILE *fp2 = fopen(fullpath, "rb");
            if (!fp2) {
                printf("Error: Cannot read '%s'\n", filename);
                continue;
            }

            TestJson t = TestJson();
            t.fullpath = StrDup(fullpath);
            t.filename = StrDup(filename);
            t.json = ReadJSON(fp2, &t.length);

            // Generate reference statistics
#if TEST_SAXSTATISTICS
            if (!referenceTest->SaxStatistics(t.json, t.length, &t.stat))
                printf("Failed to generate reference statistics\n");
#endif

#if TEST_SAXSTATISTICSUTF16
            if (!referenceTest->SaxStatisticsUTF16(t.json, t.length, &t.statUTF16))
                printf("Failed to generate reference UTF16 statistics\n");
#endif
            printf("Read '%s' (%u bytes)\n", t.filename, (unsigned)t.length);
            PrintStat(t.stat);
            printf("\n");

            testJsons.push_back(t);
        }
    }

    fclose(fp);
    printf("\n");
    return true;
}

static void FreeFiles(TestJsonList& testJsons) {
    for (TestJsonList::iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        free(itr->fullpath);
        free(itr->filename);
        free(itr->json);
        itr->filename = 0;
        itr->json = 0;
    }
}

// Normally use this at the end of MEMORYSTAT_SCOPE()
#if USE_MEMORYSTAT
void CheckMemoryLeak() {
    const MemoryStat& stat = Memory::Instance().GetStat();
    if (stat.currentSize != 0) {
        printf("\nWarning: potential memory leak (%d allocations for %d bytes)\n",
            (int)stat.mallocCount + (int)stat.reallocCount - (int)stat.freeCount,
            (int)stat.currentSize);

        PrintMemoryStat();
        printf("\n");
    }
}
#define MEMORYSTAT_CHECKMEMORYLEAK() CheckMemoryLeak()
#else
#define MEMORYSTAT_CHECKMEMORYLEAK()
#endif

static void Verify(const TestBase& test, const TestJsonList& testJsons) {
    (void)testJsons;

    printf("Verifying %s ... ", test.GetName());
    fflush(stdout);

    bool failed = false;

#if TEST_PARSE && TEST_STATISTICS
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        MEMORYSTAT_SCOPE();

        test.SetUp();
        ParseResultBase* dom1 = test.Parse(itr->json, itr->length);
        if (!dom1) {
            printf("\nFailed to parse '%s'\n", itr->filename);
            failed = true;
            test.TearDown();
            continue;
        }

        Stat stat1;
        if (!test.Statistics(dom1, &stat1)) {
            printf("Not support Statistics\n");
            delete dom1;
            test.TearDown();
            continue;
        }

        StringResultBase* json1 = test.Stringify(dom1);
        delete dom1;

        if (!json1) {
            // Some libraries may not support stringify, but still check statistics
            if (memcmp(&stat1, &itr->stat, sizeof(Stat)) != 0 &&
                memcmp(&stat1, &itr->statUTF16, sizeof(Stat)) != 0)
            {
                printf("\nStatistics of '%s' is different from reference.\n\n", itr->filename);
                printf("Reference\n---------\n");
                PrintStat(itr->stat);
                printf("\nStat 1\n--------\n");
                PrintStat(stat1);
                printf("\n");
                failed = true;
            }
            test.TearDown();
            continue;
        }

        ParseResultBase* dom2 = test.Parse(json1->c_str(), strlen(json1->c_str()));
        if (!dom2) {
            printf("\nFailed to parse '%s' 2nd time\n", itr->filename);
            failed = true;

            // Write out json1 for diagnosis
            char filename[FILENAME_MAX];
            sprintf(filename, "%s_%s", test.GetName(), itr->filename);
            makeValidFilename(filename);
            FILE* fp = fopen(filename, "wb");
            fwrite(json1->c_str(), strlen(json1->c_str()), 1, fp);
            fclose(fp);

            delete json1;
            test.TearDown();
            continue;
        }

        Stat stat2;
        test.Statistics(dom2, &stat2);

        StringResultBase* json2 = test.Stringify(dom2);
        delete dom2;

        Stat* statProblem = 0;
        int statProblemWhich = 0;
        if (memcmp(&stat1, &itr->stat,      sizeof(Stat)) != 0 &&
            memcmp(&stat1, &itr->statUTF16, sizeof(Stat)) != 0)
        {
            statProblem = &stat1;
            statProblemWhich = 1;
        }
        else if (memcmp(&stat2, &itr->stat,      sizeof(Stat)) != 0 &&
                 memcmp(&stat2, &itr->statUTF16, sizeof(Stat)) != 0)
        {
            statProblem = &stat2;
            statProblemWhich = 2;
        }

        if (statProblem != 0) {
            printf("\nStatistics of '%s' is different from reference.\n\n", itr->filename);
            printf("Reference\n---------\n");
            PrintStat(itr->stat);
            printf("\nStat #%d\n--------\n", statProblemWhich);
            PrintStat(*statProblem);
            printf("\n");

            // Write out json1 for diagnosis
            char filename[FILENAME_MAX];
            sprintf(filename, "%s_%s", test.GetName(), itr->filename);
            makeValidFilename(filename);
            FILE* fp = fopen(filename, "wb");
            fwrite(json1->c_str(), strlen(json1->c_str()), 1, fp);
            fclose(fp);

            failed = true;
        }

        delete json1;
        delete json2;
        test.TearDown();

        MEMORYSTAT_CHECKMEMORYLEAK();
    }
#endif

#if TEST_SAXSTATISTICS
    // Verify SaxStatistics()
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        MEMORYSTAT_SCOPE();
        Stat stat1;
        if (test.SaxStatistics(itr->json, itr->length, &stat1)) {
            if (memcmp(&stat1, &itr->stat, sizeof(Stat)) != 0 &&
                memcmp(&stat1, &itr->statUTF16, sizeof(Stat)) != 0)
            {
                printf("\nSaxStatistics of '%s' is different from reference.\n\n", itr->filename);
                printf("Reference\n---------\n");
                PrintStat(itr->stat);
                printf("\nStat #%d\n--------\n", 1);
                PrintStat(stat1);
                printf("\n");
            }
        }
    }
#endif

    printf(failed ? "Failed\n" : "OK\n");
}

static void VerifyAll(const TestJsonList& testJsons) {
    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr)
        Verify(**itr, testJsons);

    printf("\n");
}

#if USE_MEMORYSTAT
#define BENCH_MEMORYSTAT_INIT()             MemoryStat memoryStat = MemoryStat()
#define BENCH_MEMORYSTAT_ITERATION(trial)   if (trial == 0) memoryStat = Memory::Instance().GetStat()
#ifdef _MSC_VER
#define BENCH_MEMORYSTAT_OUTPUT(fp)         fprintf(fp, ",%Iu,%Iu,%Iu", memoryStat.currentSize, memoryStat.peakSize, memoryStat.mallocCount + memoryStat.reallocCount)
#else
#define BENCH_MEMORYSTAT_OUTPUT(fp)         fprintf(fp, ",%zu,%zu,%zu", memoryStat.currentSize, memoryStat.peakSize, memoryStat.mallocCount + memoryStat.reallocCount)
#endif
#else
#define BENCH_MEMORYSTAT_INIT()
#define BENCH_MEMORYSTAT_ITERATION(trial)
#define BENCH_MEMORYSTAT_OUTPUT(fp)
#endif

#if TEST_PARSE
static void BenchParse(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Parse", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            test.SetUp();
            Timer timer;
            ParseResultBase* dom;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                dom = test.Parse(itr->json, itr->length);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            delete dom;

            if (!dom) {
                supported = false;
                test.TearDown();
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
            test.TearDown();
        }

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "1. Parse,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchParse(const TestBase&, const TestJsonList&, FILE *) {
}
#endif

#if TEST_PARSE && TEST_STRINGIFY
static void BenchStringify(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Stringify", itr->filename);
        fflush(stdout);

        test.SetUp();
        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length);

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            StringResultBase* json;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                json = test.Stringify(dom);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            delete json;

            if (!json) {
                supported = false;
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;
        test.TearDown();

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "2. Stringify,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchStringify(const TestBase&, const TestJsonList&, FILE *) {
}
#endif

#if TEST_PARSE && TEST_PRETTIFY
static void BenchPrettify(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Prettify", itr->filename);
        fflush(stdout);

        test.SetUp();
        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length);

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            StringResultBase* json;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                json = test.Prettify(dom);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            delete json;

            if (!json) {
                supported = false;
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;
        test.TearDown();

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "3. Prettify,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchPrettify(const TestBase&, const TestJsonList&, FILE *) {
}
#endif

#if TEST_PARSE && TEST_STATISTICS
static void BenchStatistics(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Statistics", itr->filename);
        fflush(stdout);

        test.SetUp();
        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length);

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                Stat stat;
                supported = test.Statistics(dom, &stat);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            if (!supported)
                break;

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;
        test.TearDown();

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "4. Statistics,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchStatistics(const TestBase&, const TestJsonList&, FILE *) {
}
#endif

#if TEST_SAXROUNDTRIP
static void BenchSaxRoundtrip(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "SaxRoundtrip", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            test.SetUp();
            Timer timer;
            StringResultBase* json;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                json = test.SaxRoundtrip(itr->json, itr->length);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            delete json;

            if (!json) {
                supported = false;
                test.TearDown();
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
            test.TearDown();
        }

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "5. Sax Round-trip,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchSaxRoundtrip(const TestBase&, const TestJsonList&, FILE*) {
}
#endif

#if TEST_SAXSTATISTICS
static void BenchSaxStatistics(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    MEMORYSTAT_SCOPE();
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Sax Statistics", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;

        BENCH_MEMORYSTAT_INIT();
        bool supported = true;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            test.SetUp();
            Timer timer;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                Stat stat;
                supported = test.SaxStatistics(itr->json, itr->length, &stat);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            if (!supported) {
                test.TearDown();
                break;
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
            test.TearDown();
        }

        if (!supported)
            printf("Not support\n");
        else {
            double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
            printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

            fprintf(fp, "6. SaxStatistics,%s,%s,%f", test.GetName(), itr->filename, minDuration);
            BENCH_MEMORYSTAT_OUTPUT(fp);
            fprintf(fp, ",0");  // Code size
            fputc('\n', fp);
        }
    }
    MEMORYSTAT_CHECKMEMORYLEAK();
}
#else
static void BenchSaxStatistics(const TestBase&, const TestJsonList&, FILE*) {
}
#endif

static void BenchCodeSize(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    (void)testJsons;

#if _MSC_VER
    const char cSeparator = '\\';
#else
    const char cSeparator = '/';
#endif

    // Compute path of bin
    char path[FILENAME_MAX];
    strcpy(path, gProgramName);
    char* lastSep = strrchr(path, cSeparator);
    *lastSep = '\0';

    // Compute filename suffix (e.g. "_release_x32_vs2010.exe"
    const char* filename_suffix = strchr(lastSep + 1, '_');

    // Compute test base name (e.g. "rapidjsontest")
    char testFilename[FILENAME_MAX];
    strcpy(testFilename, test.GetFilename());
    *strrchr(testFilename, '.') = '\0';
    const char* testBaseName = strrchr(testFilename, cSeparator) + 1;

    // Assemble a full path
    char fullpath[FILENAME_MAX];
    sprintf(fullpath, "%s%cjsonstat%cjsonstat_%s%s", path, cSeparator, cSeparator, testBaseName, filename_suffix);

    char * const argv[] = { fullpath, testJsons.front().fullpath, NULL };
#ifdef _MSC_VER
    int ret = _spawnv(_P_WAIT, fullpath, argv);
#elif defined(__CYGWIN__)
    int ret = spawnv(_P_WAIT, fullpath, argv);
#else
    pid_t pid;
    int ret = posix_spawn(&pid, fullpath, NULL, NULL, argv, NULL);
    if (ret == 0) {
        int status;
        waitpid(pid, &status, 0);
    }
#endif

    if (ret != 0) {
        printf("Execute '%s' failed (ret=%d)\n", fullpath, ret);
        return;
    }

    // Get file size
    FILE *fp2 = fopen(fullpath, "rb");
    if (fp2) {
        fseek(fp2, 0, SEEK_END);
        unsigned fileSize = (unsigned)ftell(fp2);
        printf("jsonstat file size = %u\n", fileSize);
        fprintf(fp, "7. Code size,%s,jsonstat,0", test.GetName());
#if USE_MEMORYSTAT
        fprintf(fp, ",0,0,0");
#endif
        fprintf(fp, ",%u\n", fileSize);
        fclose(fp2);
    }
    else
        printf("File '%s' not found\n", fullpath);
}

static void BenchPerformance(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    printf("Benchmarking Performance of %s\n", test.GetName());

    BenchParse(test, testJsons, fp);
    BenchStringify(test, testJsons, fp);
    BenchPrettify(test, testJsons, fp);
    BenchStatistics(test, testJsons, fp);
    BenchSaxRoundtrip(test, testJsons, fp);
    BenchSaxStatistics(test, testJsons, fp);
    BenchCodeSize(test, testJsons, fp);
    
    printf("\n");
}

static void BenchAllPerformance(const TestJsonList& testJsons) {
    // Try to write to /result path, where template.php exists
    FILE *fp;
    if ((fp = fopen("../../result/performance.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../../result/performance_" RESULT_FILENAME, "w");
    }
    else if ((fp = fopen("../result/performance.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../result/performance_" RESULT_FILENAME, "w");
    }
    else
        fp = fopen("performance_" RESULT_FILENAME, "w");

    fputs("Type,Library,Filename,Time (ms)", fp);
#if USE_MEMORYSTAT
    fputs(",Memory (byte),MemoryPeak (byte),AllocCount", fp);
#endif
    fputs(",FileSize (byte)\n", fp);

    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr)
        BenchPerformance(**itr, testJsons, fp);

    fclose(fp);
}

#if TEST_CONFORMANCE

static void BenchConformance(const TestBase& test, FILE* fp) {
    printf("Benchmarking Conformance of %s\n", test.GetName());
    
    // Output markdown
    FILE* md;
    char testname[FILENAME_MAX];
    strcpy(testname, test.GetName());
    makeValidFilename(testname);

    char mdFilename[FILENAME_MAX];
    sprintf(mdFilename, "../../result/conformance_%s.md", testname);
    if (!(md = fopen(mdFilename, "w"))) {
        sprintf(mdFilename, "../result/conformance_%s.md", testname);
        md = fopen(mdFilename, "w");
    }

    if (md)
        fprintf(md, "# Conformance of %s\n\n", test.GetName());

#if TEST_PARSE
    // Parse Validation, JsonChecker pass

    if (md)
        fprintf(md, "## 1. Parse Validation\n\n");

    int parseValidationCorrect = 0, parseValidationTotal = 0;

    for (int i = 1; i <= 3; i++) {
        MEMORYSTAT_SCOPE();

        char path[FILENAME_MAX];
        sprintf(path, "../data/jsonchecker/pass%02d.json", i);
        size_t length;
        char* json = ReadJSON(path, &length);
        if (!json)
            continue;

        test.SetUp();
        ParseResultBase* pr = test.Parse(json, length);
        bool result = pr != 0;
        fprintf(fp, "1. Parse Validation,%s,pass%02d,%s\n", test.GetName(), i, result ? "true" : "false");
        // printf("pass%02d: %s\n", i, result ? "true" : "false");
        delete pr;
        test.TearDown();

        if (!result) {
            if (md)
                fprintf(md, "* `%s` is valid but was mistakenly deemed invalid.\n~~~js\n%s\n~~~\n\n", path, json);
        }
        else
            parseValidationCorrect++;
        parseValidationTotal++;

        free(json);

        MEMORYSTAT_CHECKMEMORYLEAK();
    }

    // Parse Validation, JsonChecker fail
    for (int i = 1; i <= 33; i++) {
        MEMORYSTAT_SCOPE();

        char path[FILENAME_MAX];
        sprintf(path, "../data/jsonchecker/fail%02d.json", i);
        size_t length;
        char* json = ReadJSON(path, &length);
        if (!json)
            continue;

        test.SetUp();
        ParseResultBase* pr = test.Parse(json, length);
        bool result = pr == 0;
        fprintf(fp, "1. Parse Validation,%s,fail%02d,%s\n", test.GetName(), i, result ? "true" : "false");
        // printf("fail%02d: %s\n", i, result ? "true" : "false");
        delete pr;
        test.TearDown();

        if (!result) {
            if (md)
                fprintf(md, "* `%s` is invalid but was mistakenly deemed valid.\n~~~js\n%s\n~~~\n\n", path, json);
        }
        else
            parseValidationCorrect++;
        parseValidationTotal++;

        free(json);

        MEMORYSTAT_CHECKMEMORYLEAK();
    }

    if (md)
        fprintf(md, "\nSummary: %d of %d are correct.\n\n", parseValidationCorrect, parseValidationTotal);

#endif // TEST_PARSE

    parseValidationTotal = 0;
    parseValidationCorrect = 0;

    if (md)
        fprintf(md, "## 2. Parse Double\n\n");

    // Parse Double
    {
        using rapidjson::internal::Double;
        int i = 1;
        #define TEST_DOUBLE(json, expect)\
        {\
            bool result = false;\
            double actual = 0.0;\
            test.SetUp();\
            if (test.ParseDouble(json, &actual)) \
                result = Double(expect).Uint64Value() == Double(actual).Uint64Value();\
            if (!result) {\
                if (md)\
                    fprintf(md, "* `%s`\n  * expect: `%.17g (0x016%" PRIX64 ")`\n  * actual: `%.17g (0x016%" PRIX64 ")`\n\n",\
                        json, expect, Double(expect).Uint64Value(), actual, Double(actual).Uint64Value());\
            }\
            else\
                parseValidationCorrect++;\
            parseValidationTotal++;\
            /*printf("double%02d: %s\n", i, result ? "true" : "false");*/\
            /*if (!result)*/\
            /*    printf("JSON: %s\nExpect: %17g\nActual: %17g\n\n", json, expect, actual);*/\
            fprintf(fp, "2. Parse Double,%s,double%02d,%s\n", test.GetName(), i, result ? "true" : "false");\
            test.TearDown();\
            i++;\
        }
        TEST_DOUBLE("[0.0]", 0.0);
        TEST_DOUBLE("[-0.0]", -0.0);
        TEST_DOUBLE("[1.0]", 1.0);
        TEST_DOUBLE("[-1.0]", -1.0);
        TEST_DOUBLE("[1.5]", 1.5);
        TEST_DOUBLE("[-1.5]", -1.5);
        TEST_DOUBLE("[3.1416]", 3.1416);
        TEST_DOUBLE("[1E10]", 1E10);
        TEST_DOUBLE("[1e10]", 1e10);
        TEST_DOUBLE("[1E+10]", 1E+10);
        TEST_DOUBLE("[1E-10]", 1E-10);
        TEST_DOUBLE("[-1E10]", -1E10);
        TEST_DOUBLE("[-1e10]", -1e10);
        TEST_DOUBLE("[-1E+10]", -1E+10);
        TEST_DOUBLE("[-1E-10]", -1E-10);
        TEST_DOUBLE("[1.234E+10]", 1.234E+10);
        TEST_DOUBLE("[1.234E-10]", 1.234E-10);
        TEST_DOUBLE("[1.79769e+308]", 1.79769e+308);
        TEST_DOUBLE("[2.22507e-308]", 2.22507e-308);
        TEST_DOUBLE("[-1.79769e+308]", -1.79769e+308);
        TEST_DOUBLE("[-2.22507e-308]", -2.22507e-308);
        TEST_DOUBLE("[4.9406564584124654e-324]", 4.9406564584124654e-324); // minimum denormal
        TEST_DOUBLE("[2.2250738585072009e-308]", 2.2250738585072009e-308); // Max subnormal double
        TEST_DOUBLE("[2.2250738585072014e-308]", 2.2250738585072014e-308); // Min normal positive double
        TEST_DOUBLE("[1.7976931348623157e+308]", 1.7976931348623157e+308); // Max double
        TEST_DOUBLE("[1e-10000]", 0.0);                                   // must underflow
        TEST_DOUBLE("[18446744073709551616]", 18446744073709551616.0);    // 2^64 (max of uint64_t + 1, force to use double)
        TEST_DOUBLE("[-9223372036854775809]", -9223372036854775809.0);    // -2^63 - 1(min of int64_t + 1, force to use double)
        TEST_DOUBLE("[0.9868011474609375]", 0.9868011474609375);          // https://github.com/miloyip/rapidjson/issues/120
        TEST_DOUBLE("[123e34]", 123e34);                                  // Fast Path Cases In Disguise
        TEST_DOUBLE("[45913141877270640000.0]", 45913141877270640000.0);
        TEST_DOUBLE("[2.2250738585072011e-308]", 2.2250738585072011e-308); // http://www.exploringbinary.com/php-hangs-on-numeric-value-2-2250738585072011e-308/
        //TEST_DOUBLE("[1e-00011111111111]", 0.0);
        //TEST_DOUBLE("[-1e-00011111111111]", -0.0);
        TEST_DOUBLE("[1e-214748363]", 0.0);
        TEST_DOUBLE("[1e-214748364]", 0.0);
        //TEST_DOUBLE("[1e-21474836311]", 0.0);
        TEST_DOUBLE("[0.017976931348623157e+310]", 1.7976931348623157e+308); // Max double in another form

        // Since
        // abs((2^-1022 - 2^-1074) - 2.2250738585072012e-308) = 3.109754131239141401123495768877590405345064751974375599... ¡Á 10^-324
        // abs((2^-1022) - 2.2250738585072012e-308) = 1.830902327173324040642192159804623318305533274168872044... ¡Á 10 ^ -324
        // So 2.2250738585072012e-308 should round to 2^-1022 = 2.2250738585072014e-308
        TEST_DOUBLE("[2.2250738585072012e-308]", 2.2250738585072014e-308); // http://www.exploringbinary.com/java-hangs-when-converting-2-2250738585072012e-308/

        // More closer to normal/subnormal boundary
        // boundary = 2^-1022 - 2^-1075 = 2.225073858507201136057409796709131975934819546351645648... ¡Á 10^-308
        TEST_DOUBLE("[2.22507385850720113605740979670913197593481954635164564e-308]", 2.2250738585072009e-308);
        TEST_DOUBLE("[2.22507385850720113605740979670913197593481954635164565e-308]", 2.2250738585072014e-308);

        // 1.0 is in (1.0 - 2^-54, 1.0 + 2^-53)
        // 1.0 - 2^-54 = 0.999999999999999944488848768742172978818416595458984375
        TEST_DOUBLE("[0.999999999999999944488848768742172978818416595458984375]", 1.0); // round to even
        TEST_DOUBLE("[0.999999999999999944488848768742172978818416595458984374]", 0.99999999999999989); // previous double
        TEST_DOUBLE("[0.999999999999999944488848768742172978818416595458984376]", 1.0); // next double
        // 1.0 + 2^-53 = 1.00000000000000011102230246251565404236316680908203125
        TEST_DOUBLE("[1.00000000000000011102230246251565404236316680908203125]", 1.0); // round to even
        TEST_DOUBLE("[1.00000000000000011102230246251565404236316680908203124]", 1.0); // previous double
        TEST_DOUBLE("[1.00000000000000011102230246251565404236316680908203126]", 1.00000000000000022); // next double

        // Numbers from https://github.com/floitsch/double-conversion/blob/master/test/cctest/test-strtod.cc

        TEST_DOUBLE("[72057594037927928.0]", 72057594037927928.0);
        TEST_DOUBLE("[72057594037927936.0]", 72057594037927936.0);
        TEST_DOUBLE("[72057594037927932.0]", 72057594037927936.0);
        TEST_DOUBLE("[7205759403792793199999e-5]", 72057594037927928.0);
        TEST_DOUBLE("[7205759403792793200001e-5]", 72057594037927936.0);

        TEST_DOUBLE("[9223372036854774784.0]", 9223372036854774784.0);
        TEST_DOUBLE("[9223372036854775808.0]", 9223372036854775808.0);
        TEST_DOUBLE("[9223372036854775296.0]", 9223372036854775808.0);
        TEST_DOUBLE("[922337203685477529599999e-5]", 9223372036854774784.0);
        TEST_DOUBLE("[922337203685477529600001e-5]", 9223372036854775808.0);

        TEST_DOUBLE("[10141204801825834086073718800384]", 10141204801825834086073718800384.0);
        TEST_DOUBLE("[10141204801825835211973625643008]", 10141204801825835211973625643008.0);
        TEST_DOUBLE("[10141204801825834649023672221696]", 10141204801825835211973625643008.0);
        TEST_DOUBLE("[1014120480182583464902367222169599999e-5]", 10141204801825834086073718800384.0);
        TEST_DOUBLE("[1014120480182583464902367222169600001e-5]", 10141204801825835211973625643008.0);

        TEST_DOUBLE("[5708990770823838890407843763683279797179383808]", 5708990770823838890407843763683279797179383808.0);
        TEST_DOUBLE("[5708990770823839524233143877797980545530986496]", 5708990770823839524233143877797980545530986496.0);
        TEST_DOUBLE("[5708990770823839207320493820740630171355185152]", 5708990770823839524233143877797980545530986496.0);
        TEST_DOUBLE("[5708990770823839207320493820740630171355185151999e-3]", 5708990770823838890407843763683279797179383808.0);
        TEST_DOUBLE("[5708990770823839207320493820740630171355185152001e-3]", 5708990770823839524233143877797980545530986496.0);

        {
            char n1e308[312];   // '1' followed by 308 '0'
            n1e308[0] = '[';
            n1e308[1] = '1';
            for (int j = 2; j < 310; j++)
                n1e308[j] = '0';
            n1e308[310] = ']';
            n1e308[311] = '\0';
            TEST_DOUBLE(n1e308, 1E308);
        }

        // Cover trimming
        TEST_DOUBLE(
            "[2.22507385850720113605740979670913197593481954635164564802342610972482222202107694551652952390813508"
            "7914149158913039621106870086438694594645527657207407820621743379988141063267329253552286881372149012"
            "9811224514518898490572223072852551331557550159143974763979834118019993239625482890171070818506906306"
            "6665599493827577257201576306269066333264756530000924588831643303777979186961204949739037782970490505"
            "1080609940730262937128958950003583799967207254304360284078895771796150945516748243471030702609144621"
            "5722898802581825451803257070188608721131280795122334262883686223215037756666225039825343359745688844"
            "2390026549819838548794829220689472168983109969836584681402285424333066033985088644580400103493397042"
            "7567186443383770486037861622771738545623065874679014086723327636718751234567890123456789012345678901"
            "e-308]", 
            2.2250738585072014e-308);
    }

    if (md)
        fprintf(md, "\nSummary: %d of %d are correct.\n\n", parseValidationCorrect, parseValidationTotal);

    // Parse String

    parseValidationTotal = 0;
    parseValidationCorrect = 0;

    if (md)
        fprintf(md, "## 3. Parse String\n\n");

    {
        int i = 1;
        #define TEST_STRING(json, expect)\
        {\
            bool result = false;\
            size_t expectLength = sizeof(expect) - 1;\
            std::string actual;\
            test.SetUp();\
            if (test.ParseString(json, actual)) \
                result = (expectLength == actual.size()) && (memcmp(expect, actual.c_str(), expectLength) == 0);\
            if (!result) {\
                if (md) {\
                    fprintf(md, "* `%s`\n  * expect: `", json);\
                    EscapeString(md, expect, expectLength);\
                    fprintf(md, "` (length: %d)\n  * actual: `", (int)expectLength);\
                    EscapeString(md, actual.c_str(), actual.size());\
                    fprintf(md, "` (length: %d)\n\n", (int)actual.size());\
                }\
            }\
            else\
                parseValidationCorrect++;\
            parseValidationTotal++;\
            /*printf("string%02d: %s\n", i, result ? "true" : "false");*/\
            /*if (!result)*/\
            /*    printf("JSON: %s\nExpect: %s (%u) \nActual: %s (%u)\n\n", json, expect, (unsigned)expectLength, actual.c_str(), (unsigned)actual.size());*/\
            fprintf(fp, "3. Parse String,%s,string%02d,%s\n", test.GetName(), i, result ? "true" : "false");\
            test.TearDown();\
            i++;\
        }

        TEST_STRING("[\"\"]", "");
        TEST_STRING("[\"Hello\"]", "Hello");
        TEST_STRING("[\"Hello\\nWorld\"]", "Hello\nWorld");
        TEST_STRING("[\"Hello\\u0000World\"]", "Hello\0World");
        TEST_STRING("[\"\\\"\\\\/\\b\\f\\n\\r\\t\"]", "\"\\/\b\f\n\r\t");
        TEST_STRING("[\"\\u0024\"]", "\x24");         // Dollar sign U+0024
        TEST_STRING("[\"\\u00A2\"]", "\xC2\xA2");     // Cents sign U+00A2
        TEST_STRING("[\"\\u20AC\"]", "\xE2\x82\xAC"); // Euro sign U+20AC
        TEST_STRING("[\"\\uD834\\uDD1E\"]", "\xF0\x9D\x84\x9E");  // G clef sign U+1D11E
    }

    if (md)
        fprintf(md, "\nSummary: %d of %d are correct.\n\n", parseValidationCorrect, parseValidationTotal);

#if TEST_PARSE && TEST_STRINGIFY
    parseValidationTotal = 0;
    parseValidationCorrect = 0;

    if (md)
        fprintf(md, "## 4. Roundtrip\n\n");

    // Roundtrip
    for (int i = 1; i <= 27; i++) {
        MEMORYSTAT_SCOPE();
        
        char path[FILENAME_MAX];
        sprintf(path, "../data/roundtrip/roundtrip%02d.json", i);
        size_t length;
        char* json = ReadJSON(path, &length);
        if (!json)
            continue;

        test.SetUp();
        ParseResultBase* pr = test.Parse(json, length);
        bool result = false;
        bool terminate = false;
        if (pr) {
            StringResultBase* sr = test.Stringify(pr);
            delete pr;

            if (sr) {
                // Some libraries must produce newlines/tabs, skip them in comparison.
                const char* s = sr->c_str();
                result = true;
                for (size_t j = 0; j < length; ++j, ++s) {
                    while (*s == '\n' || *s == '\t')
                        ++s;
                    if (json[j] != *s) {
                        result = false;
                        break;
                    }
                }

                // printf("roundtrip%02d: %s\n", i, result ? "true" : "false");

                // if (!result)
                //     printf("Expect: %s\nActual: %s\n\n", json, sr->c_str());
            if (!result) {
                if (md)
                    fprintf(md, "* Fail:\n~~~js\n%s\n~~~\n\n~~~js\n%s\n~~~\n\n", json, sr ? sr->c_str() : "N/A");\
            }
            else
                parseValidationCorrect++;
            parseValidationTotal++;

                delete sr;
            }
            else
                terminate = true; // This library does not support stringify, terminate this test
        }

        test.TearDown();
        free(json);

        if (terminate)
            break;

        fprintf(fp, "4. Roundtrip,%s,roundtrip%02d,%s\n", test.GetName(), i, result ? "true" : "false");

        MEMORYSTAT_CHECKMEMORYLEAK();
    }

    if (md)
        fprintf(md, "\nSummary: %d of %d are correct.\n\n", parseValidationCorrect, parseValidationTotal);

#endif // TEST_PARSE && TEST_STRINGIFY

    printf("\n");
}

static void BenchAllConformance() {
    FILE *fp;
    if ((fp = fopen("../../result/conformance.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../../result/conformance.csv", "w");
    }
    else if ((fp = fopen("../result/conformance.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../result/conformance.csv", "w");
    }
    else
        fp = fopen("conformance.csv", "w");

    fputs("Type,Library,Test,Result\n", fp);

    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr) {
        if (strcmp((*itr)->GetName(), "strdup (C)") == 0 ||
            strcmp((*itr)->GetName(), "pjson (C)") == 0)
            continue;
        BenchConformance(**itr, fp);
    }

    fclose(fp);
}

#endif // TEST_CONFORMANCE

int main(int argc, char* argv[]) {
#if _MSC_VER
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //void *testWhetherMemoryLeakDetectionWorks = malloc(1);
#endif

    bool doVerify = true;
    bool doPerformance = true;
    bool doConformance = true;

    if (argc == 2) {
        if (strcmp(argv[1], "--verify-only") == 0) {
            doVerify = true;
            doPerformance = doConformance = false;
        }
        else if (strcmp(argv[1], "--performance-only") == 0) {
            doPerformance = true;
            doVerify = doConformance = false;
        }
        else if (strcmp(argv[1], "--conformance-only") == 0) {
            doConformance = true;
            doVerify = doPerformance = false;
        }
        else {
            fprintf(stderr, "Invalid option\n");
            exit(1);
        }
    }

    gProgramName = argv[0];

    MEMORYSTAT_SCOPE();

    {
        // Read files
        TestJsonList testJsons;
        if (!ReadFiles("../data/%s", testJsons))
            ReadFiles("../../data/%s", testJsons);

        // sort tests
        TestList& tests = TestManager::Instance().GetTests();
        std::sort(tests.begin(), tests.end());

        if (doVerify)
            VerifyAll(testJsons);

        if (doPerformance)
            BenchAllPerformance(testJsons);

#if TEST_CONFORMANCE
        if (doConformance)
            BenchAllConformance();
#endif

        printf("\n");

        FreeFiles(testJsons);
    }

    MEMORYSTAT_CHECKMEMORYLEAK();
}
