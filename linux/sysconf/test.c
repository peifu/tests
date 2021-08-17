#include <stdio.h>
#include <unistd.h>
#define ONE_MB (1024 * 1024)

int main (void)
{
	long num_procs;
	long page_size;
	long num_pages;
	long free_pages;
	long long mem;
	long long free_mem;

    num_procs = sysconf (_SC_NPROCESSORS_CONF);
	printf ("CPU: %ld ��\n", num_procs);
	page_size = sysconf (_SC_PAGESIZE);
	printf ("PAGESIZE: %ld K\n", page_size / 1024 );
	num_pages = sysconf (_SC_PHYS_PAGES);
	printf ("PHYS_PAGES: %ld ��\n", num_pages);
	free_pages = sysconf (_SC_AVPHYS_PAGES);
	printf ("FREE_PAGES: %ld ��\n", free_pages);
	mem = (long long) ((long long)num_pages * (long long)page_size);
	mem /= ONE_MB;
	free_mem = (long long)free_pages * (long long)page_size;
	free_mem /= ONE_MB;
	printf ("�ܹ��� %lld MB �������ڴ�, ���е������ڴ���: %lld MB\n", mem, free_mem);

    return (0);
}
