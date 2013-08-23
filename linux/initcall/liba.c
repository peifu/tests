#include <stdio.h>

typedef int (*fn) (void);

int fn_a()
{
	printf("fn_a() called\n");
	return 0;
}
__attribute__((__section__(".init_array.2"))) static fn init_a = fn_a;

int fn_b()
{
	printf("fn_b() called\n");
	return 0;
} 

int fn_c()
{
	printf("fn_c() called\n");
	return 0;
}
__attribute__((__section__(".init_array.3"))) static fn init_c = fn_c;

int fn_d()
{
	printf("fn_d() called\n");
	return 0;
}
__attribute__((__section__(".peifu"))) static fn init_d = fn_d;


