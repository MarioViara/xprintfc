/**
 * @file        xformatc.h
 *
 * @brief       Printf C declaration.
 * 
 * @author      Mario Viara
 * 
 * @copyright   Copyright Mario Viara 2014  - License Open Source (LGPL)
 * This is a free software and is opened for education, research and commercial
 * developments under license policy of following terms:
 * - This is a free software and there is NO WARRANTY.
 * - No restriction on use. You can use, modify and redistribute it for personal,
 *   non-profit or commercial product UNDER YOUR RESPONSIBILITY.
 * - Redistributions of source code must retain the above copyright notice.
 *
 * To contact the author send an email to mario_at_viara.eu*
 */
#ifndef XFORMATC_H
#define XFORMATC_H
#include <stdarg.h>
#ifdef  __cplusplus
extern "C" {
#endif

/**
 * MSVC use in x64 model IL32P64 architecture so the largest integer
 * is not a standard C	integer.
 */
#if defined(_MSC_VER) && defined(_M_AMD64)
#define	LONG	long long
#define XCFG_FORMAT_LONG_ARE_LONGLONG
#endif


/**
 * SDCC support only float and for now do not support long long
 */
#ifdef __SDCC
#define DOUBLE	float
#ifndef XCFG_FORMAT_LONGLONG
#define XCFG_FORMAT_LONGLONG    0
#endif
#endif


/**
 * Define internal parameters as volatile for 8 bit cpu define
 * XCFG_FORMAT_STATIC=static to reduce stack usage.
 */
#ifndef XCFG_FORMAT_STATIC
#define XCFG_FORMAT_STATIC
#endif


/**
 * Define XCFG_FORMAT_FLOAT=0 to remove floating point support
 */
#ifndef XCFG_FORMAT_FLOAT
#define XCFG_FORMAT_FLOAT    1
#endif


/**
 * Detect support for va_copy this macro must be called for example
 * in x86_64 machine to adjust the stack frame when an argument of va_list
 * is passed over functions.
 */
#ifndef XCFG_FORMAT_VA_COPY
#if	defined(__GNUC__) && defined(__x86_64__)
#define XCFG_FORMAT_VA_COPY     1
#endif


#ifndef XCFG_FORMAT_VA_COPY
#define XCFG_FORMAT_VA_COPY	0
#endif

#endif


/**
 * Define to 0 to support long long type (prefix ll)
 */
#ifndef XCFG_FORMAT_LONGLONG
#define XCFG_FORMAT_LONGLONG    1
#endif






unsigned xformat(void (*outchar)(void *arg,char),void *arg,const char * fmt,...);

unsigned xvformat(void (*outchar)(void *arg,char),void *arg,const char * fmt,va_list args);



#ifdef  __cplusplus
}
#endif

#endif
