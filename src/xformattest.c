/**
 * @file        xformattest.c
 *
 * @brief       Test pattern for xformatc.c
 *
 * 
 * @author      Mario Viara
 * 
 * @version     1.00
 * 
 * @copyright   Copyright Mario Viara 2014  - License Open Source (LGPL)
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

#include "xformatc.h"


static void myPutchar(void *arg,char c)
{
    char ** s = (char **)arg;
    *(*s)++ = c;
}

static void myPrintf(char *buf,const char *fmt,va_list args)
{
    xvformat(myPutchar,(void *)&buf,fmt,args);
    *buf = 0;
}


static void testFormat(const char * fmt,...)
{
    char buf1[1024];
    char buf2[1024];

    va_list list;
#if  XCFG_FORMAT_VA_COPY
    va_list args;

    va_start(args,fmt);
#else
    va_start(list,fmt);
#endif

#if  XCFG_FORMAT_VA_COPY
    va_copy(list,args);
#endif

    myPrintf(buf1,fmt,list);

#if  XCFG_FORMAT_VA_COPY
    va_end(list);
#endif

#if  XCFG_FORMAT_VA_COPY
    va_copy(list,args);
#endif

    vsprintf(buf2,fmt,list);

#if  XCFG_FORMAT_VA_COPY
    va_end(list);
#endif


    if (*fmt != '*' && strcmp(buf1,buf2))
    {
        printf("XFormat : '%s'\nvsprintf: '%s'\nFormat  : '%s' failed\n",
               buf1,buf2,fmt);
        exit(1);
    }
    else
    {
        printf("'%s'\n'%s'\n",buf1,buf2);

    }

#if  XCFG_FORMAT_VA_COPY
    va_end(args);
    (void)args;
#else
    va_end(list);
#endif

}

int main(void)
{
    static int value;
    static void * ptr = &value;
    int stackValue;
    void * stackPtr = &stackValue;


    printf("XFORMATC test\n\n");
    testFormat("Hello world {%u}",sizeof(unsigned long));
    testFormat("Hello %s","World");
    testFormat("String %4.4s","Large");
    testFormat("String %*.*s",4,4,"Hello");
    testFormat("integer %05d %+d %d %2d %5d",-7,7,-7,1234,1234);
    testFormat("Integer %+05d %-5d % 5d %05d",1234,1234,1234,1234);
    testFormat("Integer blank % d % d",1,-1);
    testFormat("Unsigned %u %lu",123,123Lu);
    testFormat("Hex with prefix %#x %#x %#X %#08x",0,1,2,12345678);
    testFormat("Octal %o %lo",123,123456L);
    testFormat("Octal with prefix %#o %#o",0,5);
    testFormat("Hex %x %X %lX",0x1234,0xf0ad,0xf2345678L);
    testFormat("Special char %%");
    testFormat("Size    of void * %u(%u)",(size_t)sizeof(void *),(size_t)sizeof(void *));
	testFormat("Sizeof char=%d short=%d int=%d long=%d void*=%u size_t=%u",
			   sizeof(char),sizeof(short),sizeof(int),sizeof(long),sizeof(void *),sizeof(size_t));
	
#if XCFG_FORMAT_FLOAT
	testFormat("Floating %f",-0.6);
    testFormat("Floating %6.2f",22.0/7.0);
    testFormat("Floating %6.2f",-22.0/7.0);
    testFormat("Floating %+6.1f %6.2f",3.999,-3.999);
    testFormat("Floating %6.1f %6.0f",3.999,-3.999);
    testFormat("Floating %5.0f",3.14);
    testFormat("Floating %.0f",0.491);
    testFormat("Floating %.0f",0.490);
    testFormat("Floating %.0f",0.489);
    testFormat("Floating %.0f",-0.491);
    testFormat("Floating %.0f",-0.490);
    testFormat("Floating %.0f",-0.489);
#endif

    testFormat("*Sizeof of void * %zu",sizeof(void *));
    testFormat("*Binary number %b %#b",5,6);
    testFormat("*Stack  ptr %p %P",stackPtr,stackPtr);
    testFormat("*Static ptr %p %P",ptr,ptr);
    testFormat("*Text   ptr %p %P",xvformat,xvformat);
    testFormat("*boolean %B %B",1,0);
    testFormat("*Text pointer as sizeof %zX",xvformat);

#if XCFG_FORMAT_LONGLONG
    testFormat("long long int %lld",(long long)123);
    testFormat("long long int %lld",(long long)-123);
	testFormat("long long hex %#llx",(long long)0x123456789abcdef);
    testFormat("long long hex %#llX",(long long)0x123456789abcdef);
	testFormat("Floating > 32 bit %f",pow(2.0,32.0)+1.0);
	testFormat("Floating < 32 bit %f",-pow(2.0,32.0)-1.0);
#endif
    printf("\nTest completed succesfuylly\n"); 

    return 0;
}


