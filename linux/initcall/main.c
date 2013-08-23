#include<stdio.h>

extern int fn_b();

int main()
{
	printf("main() called\n");
	fn_b();
	return 0;
}
