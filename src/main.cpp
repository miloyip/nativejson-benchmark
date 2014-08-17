#include <algorithm>
#include <cfloat>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "test.h"
#include "timer.h"
#include "resultfilename.h"

static const unsigned cTrialCount = 10;

struct TestJson {
    char* filename;
    char* json;
    size_t length;
    Stat stat;      // Reference statistics
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

#ifdef USE_MEMORYSTAT
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

static char* Strdup(const char* s) {
    size_t length = strlen(s);
    char* r = (char*)malloc(length + 1);
    memcpy(r, s, length + 1);
    return r;
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
        if (strcmp((*itr)->GetName(), "RapidJSON") == 0) {
            referenceTest = *itr;
            break;
        }
    }

    if (!referenceTest) {
        printf("Error: Cannot find RapidJSON as refernce test. Not reading any files.\n");
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
            
            TestJson t;
            t.filename = Strdup(filename);
            fseek(fp2, 0, SEEK_END);
            t.length = (size_t)ftell(fp2);
            fseek(fp2, 0, SEEK_SET);
            t.json = (char*)malloc(t.length + 1);
            fread(t.json, 1, t.length, fp2);
            t.json[t.length] = '\0';
            fclose(fp2);

            // Generate reference statistics
            ParseResultBase* dom = referenceTest->Parse(t.json, t.length);
            t.stat = referenceTest->Statistics(dom);

            printf("Read '%s' (%u bytes)\n", t.filename, (unsigned)t.length);
            PrintStat(t.stat);
            printf("\n");

            // Write out reference JSON
#if 0
            {
                char* json = referenceTest->Stringify(dom);
                char jsonfilename[FILENAME_MAX];
                sprintf(jsonfilename, "%s_%s", referenceTest->GetName(), filename);
                FILE* fp = fopen(jsonfilename, "wb");
                fwrite(json, strlen(json), 1, fp);
                fclose(fp);
                free(json);
            }
#endif

            delete dom;

            testJsons.push_back(t);
        }
    }

    fclose(fp);
    printf("\n");
    return true;
}

static void FreeFiles(TestJsonList& testJsons) {
    for (TestJsonList::iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        free(itr->filename);
        free(itr->json);
        itr->filename = 0;
        itr->json = 0;
    }
}

static void Verify(const TestBase& test, const TestJsonList& testJsons) {
    printf("Verifying %s ... ", test.GetName());
    fflush(stdout);

    bool failed = false;

    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        MEMORYSTAT_SCOPE();

        ParseResultBase* dom1 = test.Parse(itr->json, itr->length);
        if (!dom1) {
            printf("\nFailed to parse '%s'\n", itr->filename);
            failed = true;
            continue;
        }

        Stat stat1 = test.Statistics(dom1);

        StringResultBase* json1 = test.Stringify(dom1);
        delete dom1;

        if (!json1) {
            printf("\nFailed to strinify '%s'\n", itr->filename);
            failed = true;
            continue;
        }

        ParseResultBase* dom2 = test.Parse(json1->c_str(), strlen(json1->c_str()));
        if (!dom2) {
            printf("\nFailed to parse '%s' 2nd time\n", itr->filename);
            failed = true;
            continue;
        }

        Stat stat2 = test.Statistics(dom2);

        StringResultBase* json2 = test.Stringify(dom2);
        delete dom2;

        Stat* statProblem = 0;
        int statProblemWhich = 0;
        if (memcmp(&stat1, &itr->stat, sizeof(Stat)) != 0) {
            statProblem = &stat1;
            statProblemWhich = 1;
        }
        else if (memcmp(&stat1, &itr->stat, sizeof(Stat)) != 0) {
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
            FILE* fp = fopen(filename, "wb");
            fwrite(json1->c_str(), strlen(json1->c_str()), 1, fp);
            fclose(fp);

            failed = true;
        }

        delete json1;
        delete json2;

#ifdef USE_MEMORYSTAT
        const MemoryStat& stat = Memory::Instance().GetStat();
        if (stat.currentSize != 0) {
            printf("\nWarning: potential memory leak (%d allocations for %d bytes)\n",
                (int)stat.mallocCount + (int)stat.reallocCount - (int)stat.freeCount,
                (int)stat.currentSize);

            PrintMemoryStat();
            printf("\n");
        }
#endif
    }

    printf(failed ? "Failed\n" : "OK\n");
}

static void VerifyAll(const TestJsonList& testJsons) {
    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr) {
        if (strcmp((*itr)->GetName(), "Strdup") != 0)   // skip Strdup
            Verify(**itr, testJsons);
    }

    printf("\n");
}

#ifdef USE_MEMORYSTAT
#define BENCH_MEMORYSTAT_INIT()             MemoryStat stat = MemoryStat()
#define BENCH_MEMORYSTAT_ITERATION(trial)   if (trial == 0) stat = Memory::Instance().GetStat()
#ifdef _MSC_VER
#define BENCH_MEMORYSTAT_OUTPUT(fp)         fprintf(fp, ",%Iu,%Iu,%Iu", stat.currentSize, stat.peakSize, stat.mallocCount + stat.reallocCount)
#else
#define BENCH_MEMORYSTAT_OUTPUT(fp)         fprintf(fp, ",%zu,%zu,%zu", stat.currentSize, stat.peakSize, stat.mallocCount + stat.reallocCount)
#endif
#else
#define BENCH_MEMORYSTAT_INIT()
#define BENCH_MEMORYSTAT_ITERATION(trial)
#define BENCH_MEMORYSTAT_OUTPUT(fp)
#endif

static void BenchParse(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Parse", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;

        BENCH_MEMORYSTAT_INIT();
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
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
            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }
        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

        fprintf(fp, "2. Parse,%s,%s,%f", test.GetName(), itr->filename, minDuration);
        BENCH_MEMORYSTAT_OUTPUT(fp);
        fputc('\n', fp);
    }
}

static void BenchStringify(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Stringify", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length);

        BENCH_MEMORYSTAT_INIT();
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
            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;

        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

        fprintf(fp, "3. Stringify,%s,%s,%f", test.GetName(), itr->filename, minDuration);
        BENCH_MEMORYSTAT_OUTPUT(fp);
        fputc('\n', fp);
    }
}

static void BenchPrettify(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Prettify", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length);

        BENCH_MEMORYSTAT_INIT();
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
            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;

        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

        fprintf(fp, "4. Prettify,%s,%s,%f", test.GetName(), itr->filename, minDuration);
        BENCH_MEMORYSTAT_OUTPUT(fp);
        fputc('\n', fp);
    }
}

static void BenchStatistics(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    for (TestJsonList::const_iterator itr = testJsons.begin(); itr != testJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Statistics", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length);

        BENCH_MEMORYSTAT_INIT();
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            {
                MEMORYSTAT_SCOPE();

                timer.Start();
                test.Statistics(dom);
                timer.Stop();

                BENCH_MEMORYSTAT_ITERATION(trial);
            }

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;

        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);

        fprintf(fp, "5. Statistics,%s,%s,%f", test.GetName(), itr->filename, minDuration);
        BENCH_MEMORYSTAT_OUTPUT(fp);
        fputc('\n', fp);
    }
}

static void Bench(const TestBase& test, const TestJsonList& testJsons, FILE *fp) {
    printf("Benchmarking %s\n", test.GetName());
    BenchParse(test, testJsons, fp);
    BenchStringify(test, testJsons, fp);
    BenchPrettify(test, testJsons, fp);
    BenchStatistics(test, testJsons, fp);
    printf("\n");
}

static void BenchAll(const TestJsonList& testJsons) {
    // Try to write to /result path, where template.php exists
    FILE *fp;
    if ((fp = fopen("../../result/template.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../../result/" RESULT_FILENAME, "w");
    }
    else if ((fp = fopen("../result/template.php", "r")) != NULL) {
        fclose(fp);
        fp = fopen("../result/" RESULT_FILENAME, "w");
    }
    else
        fp = fopen(RESULT_FILENAME, "w");

    fprintf(fp, "Type,Library,Filename,Time (ms),Memory (byte),MemoryPeak (byte),AllocCount\n");

    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr)
        Bench(**itr, testJsons, fp);

    fclose(fp);

    printf("\n");
}

int main() {
#if _MSC_VER
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //void *testWhetherMemoryLeakDetectionWorks = malloc(1);
#endif
    {
        // Read files
        TestJsonList testJsons;
        if (!ReadFiles("../data/%s", testJsons))
            ReadFiles("../../data/%s", testJsons);

        // sort tests
        TestList& tests = TestManager::Instance().GetTests();
        std::sort(tests.begin(), tests.end());

        VerifyAll(testJsons);
        BenchAll(testJsons);

        FreeFiles(testJsons);
    }

    TestManager::DestroyInstance();

#ifdef USE_MEMORYSTAT
    PrintMemoryStat();
#endif
}
