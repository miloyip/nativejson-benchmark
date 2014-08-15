#include <algorithm>
#include "test.h"
#include "timer.h"
#include "resultfilename.h"

struct TestJson {
    char* filename;
    char* json;
    size_t length;
};

typedef std::vector<TestJson> TestJsonList;
static TestJsonList gTestJsons;

static bool ReadFiles(const char* path) {
    char fullpath[FILENAME_MAX];
    sprintf(fullpath, path, "data.txt");
    FILE* fp = fopen(fullpath, "r");
    if (!fp)
        return false;

    while (!feof(fp)) {
        char filename[FILENAME_MAX];
        if (fscanf(fp, "%s", filename) == 1) {
            sprintf(fullpath, path, filename);
            FILE *fp2 = fopen(fullpath, "rb");
            if (!fp2) {
                printf("Cannot read '%s'\n", filename);
                continue;
            }
            
            TestJson t;
            t.filename = _strdup(filename);
            fseek(fp2, 0, SEEK_END);
            t.length = (size_t)ftell(fp2);
            fseek(fp2, 0, SEEK_SET);
            t.json = (char*)malloc(t.length + 1);
            fread(t.json, 1, t.length, fp2);
            t.json[t.length] = '\0';
            fclose(fp2);

            printf("Read '%s' (%d bytes)\n", t.filename, t.length);

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
        void* dom1 = test.Parse(itr->json);
        if (!dom1) {
            printf("\nFailed to parse '%s'\n", itr->filename);
            failed = true;
            continue;
        }
            
        char* json1 = test.Stringify(dom1);
        test.Free(dom1);
        void* dom2 = test.Parse(json1);
        char* json2 = test.Stringify(dom2);
        test.Free(dom2);

        free(json1);
        free(json2);
    }

    printf(failed ? "Failed\n" : "OK\n");
}

static void VerifyAll() {
    TestList& tests = TestManager::Instance().GetTests();
    for (TestList::iterator itr = tests.begin(); itr != tests.end(); ++itr)
        Verify(**itr);    

    printf("\n");
}

static void BenchParse(const TestBase& test, FILE *fp) {
    for (TestJsonList::iterator itr = gTestJsons.begin(); itr != gTestJsons.end(); ++itr) {
        printf("Parse     %-20s ... ", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        for (int trial = 0; trial < 10; trial++) {
            Timer timer;
            timer.Start();
            void* dom = test.Parse(itr->json);
            timer.Stop();

            test.Free(dom);
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
        printf("Stringify %-20s ... ", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        void* dom = test.Parse(itr->json);

        for (int trial = 0; trial < 10; trial++) {
            Timer timer;
            timer.Start();
            char* json = test.Stringify(dom);
            timer.Stop();

            free(json);
            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        test.Free(dom);

        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);
        fprintf(fp, "Stringify,%s,%s,%f\n", test.GetName(), itr->filename, minDuration);
    }
}

static void BenchPrettify(const TestBase& test, FILE *fp) {
    for (TestJsonList::iterator itr = gTestJsons.begin(); itr != gTestJsons.end(); ++itr) {
        printf("Prettify  %-20s ... ", itr->filename);
        fflush(stdout);

        double minDuration = DBL_MAX;
        void* dom = test.Parse(itr->json);

        for (int trial = 0; trial < 10; trial++) {
            Timer timer;
            timer.Start();
            char* json = test.Prettify(dom);
            timer.Stop();

            free(json);
            double duration = timer.GetElapsedMilliseconds();
            minDuration = std::min(minDuration, duration);
        }

        test.Free(dom);

        double throughput = itr->length / (1024.0 * 1024.0) / (minDuration * 0.001);
        printf("%6.3f ms  %3.3f MB/s\n", minDuration, throughput);
        fprintf(fp, "Stringify,%s,%s,%f\n", test.GetName(), itr->filename, minDuration);
    }
}

static void Bench(const TestBase& test, FILE *fp) {
    printf("Benchmarking %s\n", test.GetName());
    BenchParse(test, fp);
    BenchStringify(test, fp);
    BenchPrettify(test, fp);
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
