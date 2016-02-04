#include "../test.h"
#include <cstdio>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Usage: jsonstat file.json\n");
		return 1;
	}

	FILE *fp = fopen(argv[1], "rb");
	if (!fp) {
		printf("File %s not found\n", argv[1]);
		return 2;
	}

    fseek(fp, 0, SEEK_END);
    size_t length = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* json = (char*)malloc(length + 1);
    fread(json, 1, length, fp);
    json[length] = '\0';
	fclose(fp);

	const TestBase& test = *TestManager::Instance().GetTests().front();
    test.SetUp();

    ParseResultBase* dom = test.Parse(json, length);
    free(json);

    Stat stat;
    if (!test.Statistics(dom, &stat)) {
        printf("Not support Statistics\n");
        delete dom;
        test.TearDown();
        return 3;
    }

    delete dom;

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

    test.TearDown();
    return 0;
}
