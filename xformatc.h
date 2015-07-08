/**
 * @file        xformatc.h
 *
 * @brief       Printf C declaration.
 * 
 * @author      Mario Viara
 * 
 * @version     1.01
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
#ifndef XFORMATC_H
#define XFORMATC_H
#include <stdarg.h>
#ifdef  __cplusplus
extern "C" {
#endif






unsigned xvformat(void (*outchar)(void *arg,char),void *arg,const char * fmt,va_list args);

unsigned xformat(void (*outchar)(void *arg,char),void *arg,const char * fmt,...);


#ifdef  __cplusplus
}
#endif

#endif
