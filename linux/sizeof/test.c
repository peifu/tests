#include<stdio.h>

#define TEST_MACRO 0

#ifdef TEST_MACRO
char *s_str1 = "#ifdef TEST_MACRO";
#else
char *s_str1 = "#else Defined TEST_MACO";
#endif

#if TEST_MACRO
static char *s_str2 = "#if TEST_MACRO";
#else
static char *s_str2 = "#else TEST_MACRO";
#endif

int main()
{
	int a = 0;
	int array[2] = {0};
	int array2[2][4] = {0};
	char osd_name[] = "/dev/graphics/fb0";

	printf("osd_name = %s\n", osd_name);
	printf("array = %x\n", array);
	printf("array_n = %x\n", sizeof(array));
	printf("array2_n = %x\n", sizeof(array2[2]));

	printf("TEST_MACRO = %d\n", TEST_MACRO);
	printf("TEST (1)=%s\n", s_str1);
	printf("TEST (2)=%s\n", s_str2);

	printf("sizeof(int)=%d\n", sizeof(int));
	printf("sizeof(char)=%d\n", sizeof(char));
	printf("sizeof(short)=%d\n", sizeof(short));
	printf("sizeof(long)=%d\n", sizeof(long));
	printf("sizeof(long long)=%d\n", sizeof(long long));
	
	return 0;
}
