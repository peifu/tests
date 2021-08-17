#include <stdio.h>
#include <stdlib.h>

#define err(f_, ...) {printf("[\033[31;1m!\033[0m] "); printf(f_, ##__VA_ARGS__);}
#define ok(f_, ...) {printf("[\033[32;1m+\033[0m] "); printf(f_, ##__VA_ARGS__);}
#define info(f_, ...) {printf("[\033[34;1m-\033[0m] "); printf(f_, ##__VA_ARGS__);}
#define warn(f_, ...) {printf("[\033[33;1mw\033[0m] "); printf(f_, ##__VA_ARGS__);}

int main()
{
	ok("Hello everyone!\n");
	err("I am error!\n");
	info("I am info!\n");
	warn("I am warn!\n");

	return 0;
}
