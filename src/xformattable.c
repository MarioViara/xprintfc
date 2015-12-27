/**
 * @file        xformattable.c
 *
 * @brief       Table generator for xformatc.c
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
#include <stdio.h>

#include "xformatc.c"

static const unsigned states[] =
{
/* CHAR         NORMAL  PERCENT  FLAG   WIDTH     DOT  PRECIS    SIZE    TYPE*/
/* OTHER    */      0,      0,      0,      0,      0,      0,      0,      0,
/* PERCENT  */      1,      0,      0,      0,      0,      0,      0,      1,
/* DOT      */      0,      4,      4,      4,      4,      0,      0,      0,
/* STAR     */      0,      3,      3,      0,      5,      6,      0,      0,
/* ZERO     */      0,      2,      2,      3,      5,      5,      0,      0,
/* DIGIT    */      0,      3,      3,      3,      5,      5,      0,      0,
/* FLAG     */      0,      2,      2,      2,      2,      2,      2,      0,
/* SIZE     */      0,      6,      6,      6,      6,      6,      6,      0,
/* TYPE     */      0,      7,      7,      7,      7,      7,      7,      0,
};

static unsigned table['z' - ' ' + 1];
#define N (int)((sizeof(table)/sizeof(unsigned)))


void make(void)
{
    int c,i;
    int cl;

    for (i = 0;  i < N ; i++)
    {
        c = ' ' + i;
        switch (c)
        {
            case    '.':
                cl = CH_DOT;
                break;

            case    'l':
            case    'h':
            case	'z':
                cl = CH_SIZE;
                break;
            case    ' ':
            case    '-':
            case    '#':
            case    '+':
                cl = CH_FLAG;
                break;

            case    '0':
                cl = CH_ZERO;
                break;
            case    '1':
            case    '2':
            case    '3':
            case    '4':
            case    '5':
            case    '6':
            case    '7':
            case    '8':
            case    '9':
                cl = CH_DIGIT;
                break;
            case    '%':
                cl = CH_PERCENT;
                break;
            case    '*':
                cl = CH_STAR;
                break;

            case    'd':
            case    'i':
            case    'S':
            case    's':
            case    'b':
            case    'x':
            case    'X':
            case    'o':
            case    'u':
            case    'c':
            case    'C':
            case    'p':
            case    'P':
            case    'f':
            case    'B':
                cl = CH_TYPE;
                break;

            default:
                cl = CH_OTHER;
                break;
        }

        table[i] = cl;

        if (i < (int)(sizeof(states)/sizeof(unsigned)))
            table[i] = table[i] | (states[i] << 4);
    }

    printf("static const unsigned char formatStates[] =\n{\n");

    for (i = 0;  i < N ; i++)
    {
        if (i % 8 == 0)
            printf("\t");
        printf("0x%02X",table[i] & 0xff);
        if (i + 1 < N)
            printf(",");
        if ((i+1) % 8 == 0)
            printf("\n");
    }
    printf("\n};\n\n");
}


int main(int argc,char **argv)
{
    (void)argc;
    (void)argv;
    
    make();

    return 0;
}



