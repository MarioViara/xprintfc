/**
 * @file        xformattest.c
 *
 * @brief       Test speed  for xformatc.c
 *
 * 
 * @author      Mario Viara
 * 
 * @version     1.00
 * 
 * @copyright   Copyright Mario Viara 2018  - License Open Source (LGPL)
 * This is a free software and is opened for education, research and commercial
 * developments under license policy of following terms:
 * - This is a free software and there is NO WARRANTY.
 * - No restriction on use. You can use, modify and redistribute it for personal,
 *   non-profit or commercial product UNDER YOUR RESPONSIBILITY.
 * - Redistributions of source code must retain the above copyright notice.
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "xformatc.h"


static void myPutchar(void *arg,char c)
{
    char ** s = (char **)arg;
    *(*s)++ = c;
}

static int myVsprintf(char *buf,const char *fmt,va_list args)
{
    int result = xvformat(myPutchar,(void *)&buf,fmt,args);
    *buf = 0;
    return result;
}


static void testFormat(int (*format)(char *buffer,const char *format,va_list arg),const char * fmt,...)
{
    char buffer[1024];


    va_list list;
#if  XCFG_FORMAT_VA_COPY
    va_list args;

    va_start(args,fmt);
    va_copy(list,args);
    (*format)(buffer,fmt,list);
    va_end(list);
    va_end(args);
#else
    va_start(list,fmt);
    (*format)(buffer,fmt,list);
    va_end(list);
#endif


    
}

static void testspeed(const char * name,long count,int (*format)(char *buffer,const char * fmt,va_list args))
{
	long i;
	struct timeval start,now;
	double elapsed;
	
	printf("Starting test for %s ... ",name);
	fflush(stdout);
	gettimeofday(&start,0);
	
	for (i = 0 ; i < count ; i++)
	{
		testFormat(format,"Hello world {%u}",sizeof(unsigned long));
		testFormat(format,"Hello %s","World");
		testFormat(format,"String %4.4s","Large");
		testFormat(format,"String %*.*s",4,4,"Hello");
		testFormat(format,"integer %05d %+d %d %2d %5d",-7,7,-7,1234,1234);
		testFormat(format,"Integer %+05d %-5d % 5d %05d",1234,1234,1234,1234);
		testFormat(format,"Integer blank % d % d",1,-1);
		testFormat(format,"Unsigned %u %lu",123,123Lu);
		testFormat(format,"Hex with prefix %#x %#x %#X %#08x",0,1,2,12345678);
		testFormat(format,"Octal %o %lo",123,123456L);
		testFormat(format,"Octal with prefix %#o %#o",0,5);
		testFormat(format,"Hex %x %X %lX",0x1234,0xf0ad,0xf2345678L);
		testFormat(format,"Special char %%");
		testFormat(format,"Size    of void * %u(%u)",(size_t)sizeof(void *),(size_t)sizeof(void *));
		testFormat(format,"Sizeof char=%d short=%d int=%d long=%d void*=%u size_t=%u",
			   sizeof(char),sizeof(short),sizeof(int),sizeof(long),sizeof(void *),sizeof(size_t));

#if XCFG_FORMAT_FLOAT
		testFormat(format,"Floating %f",-0.6);
		testFormat(format,"Floating %6.2f",22.0/7.0);
		testFormat(format,"Floating %6.2f",-22.0/7.0);
		testFormat(format,"Floating %+6.1f %6.2f",3.999,-3.999);
		testFormat(format,"Floating %6.1f %6.0f",3.999,-3.999);
		testFormat(format,"Floating %5.0f",3.14);
		testFormat(format,"Floating %.0f",0.491);
		testFormat(format,"Floating %.0f",0.490);
		testFormat(format,"Floating %.0f",0.489);
		testFormat(format,"Floating %.0f",-0.491);
		testFormat(format,"Floating %.0f",-0.490);
		testFormat(format,"Floating %.0f",-0.489);
#endif


#if XCFG_FORMAT_LONG
		testFormat(format,"long %d %o %x",123456L,123456L,123456L);
#endif

#if XCFG_FORMAT_LONGLONG
		testFormat(format,"long long int %lld",(long long)123);
		testFormat(format,"long long int %lld",(long long)-123);
		testFormat(format,"long long hex %#llx",(long long)0x123456789abcdef);
		testFormat(format,"long long hex %#llX",(long long)0x123456789abcdef);
		testFormat(format,"Floating > 32 bit %f",pow(2.0,32.0)+1.0);
		testFormat(format,"Floating < 32 bit %f",-pow(2.0,32.0)-1.0);
#endif
	}

	gettimeofday(&now,0);
	elapsed = ((double)now.tv_sec * 1000000.0 + now.tv_usec) - ((double)start.tv_sec * 1000000.0 + start.tv_usec);
	elapsed /= 1000000.0;

	printf(" Elapsed %.3f second(s)\n",elapsed);
	fflush(stdout);
}

int main(int argc,char **argv)
{
	long count = 0;

	if (argc < 2)
	{
		printf("usage: xfprmatcspeed cycle\n");
		exit(1);
	}
	else
	{

		count = atol(argv[1]);
	
		printf("Test speed for xprintfc using %lu cycle\n",count);
		testspeed("System   ",count,vsprintf);
		testspeed("xformatc ",count,myVsprintf);
	}
	
	return 0;
}


