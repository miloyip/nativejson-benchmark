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
static TestJsonList gTestJsons;

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

static bool ReadFiles(const char* path) {
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
            t.filename = strdup(filename);
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

            gTestJsons.push_back(t);
        }
    }

    fclose(fp);
    printf("\n");
    return true;
}

static void FreeFiles() {
    for (TestJsonList::iterator itr = gTestJsons.begin(); itr != gTestJsons.end(); ++itr) {
        free(itr->filename);
        free(itr->json);
        itr->filename = 0;
        itr->json = 0;
    }
}

static void Verify(const TestBase& test) {
    printf("Verifying %s ... ", test.GetName());
    bool failed = false;

    for (TestJsonList::iterator itr = gTestJsons.begin(); itr != gTestJsons.end(); ++itr) {
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

        ParseResultBase* dom2 = test.Parse(json1->c_str(), itr->length);
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
    }

    printf(failed ? "Failed\n" : "OK\n");
}

static void VerifyAll() {
    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr) {
        if (strcmp((*itr)->GetName(), "Strdup") != 0)   // skip Strdup
            Verify(**itr);
    }

    printf("\n");
}

static void BenchParse(const TestBase& test, FILE *fp) {
    for (TestJsonList::iterator itr = gTestJsons.begin(); itr != gTestJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Parse", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            timer.Start();
            ParseResultBase* dom = test.Parse(itr->json, itr->length);
            timer.Stop();

            delete dom;
            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }
        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);
        fprintf(fp, "Parse,%s,%s,%f\n", test.GetName(), itr->filename, minDuration);
    }
}

static void BenchStringify(const TestBase& test, FILE *fp) {
    for (TestJsonList::iterator itr = gTestJsons.begin(); itr != gTestJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Stringify", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length);

        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            timer.Start();
            StringResultBase* json = test.Stringify(dom);
            timer.Stop();

            delete json;
            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;

        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);
        fprintf(fp, "Stringify,%s,%s,%f\n", test.GetName(), itr->filename, minDuration);
    }
}

static void BenchPrettify(const TestBase& test, FILE *fp) {
    for (TestJsonList::iterator itr = gTestJsons.begin(); itr != gTestJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Prettify", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length);

        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            timer.Start();
            StringResultBase* json = test.Prettify(dom);
            timer.Stop();

            delete json;
            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;

        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);
        fprintf(fp, "Prettify,%s,%s,%f\n", test.GetName(), itr->filename, minDuration);
    }
}

static void BenchStatistics(const TestBase& test, FILE *fp) {
    for (TestJsonList::iterator itr = gTestJsons.begin(); itr != gTestJsons.end(); ++itr) {
        printf("%15s %-20s ... ", "Statistics", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        ParseResultBase* dom = test.Parse(itr->json, itr->length);

        for (unsigned trial = 0; trial < cTrialCount; trial++) {
            Timer timer;
            timer.Start();
            test.Statistics(dom);
            timer.Stop();

            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        delete dom;

        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);
        fprintf(fp, "Statistics,%s,%s,%f\n", test.GetName(), itr->filename, minDuration);
    }
}

static void Bench(const TestBase& test, FILE *fp) {
    printf("Benchmarking %s\n", test.GetName());
    BenchParse(test, fp);
    BenchStringify(test, fp);
    BenchPrettify(test, fp);
    BenchStatistics(test, fp);
    printf("\n");
}

static void BenchAll() {
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

    fprintf(fp, "Type,Library,Filename,Time(ms)\n");

    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr)
        Bench(**itr, fp);

    fclose(fp);

    printf("\n");
}

int main() {
    // Read files
    if (!ReadFiles("../data/%s"))
        ReadFiles("../../data/%s");

    // sort tests
    TestList& tests = TestManager::Instance().GetTests();
    std::sort(tests.begin(), tests.end());

    VerifyAll();
    BenchAll();

    FreeFiles();
}
