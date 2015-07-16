/**
 * @file	xformatc.c
 *
 * @brief	Printf C implementation.
 *
 * Tested wuth the following operating systems / compilers :
 *
 * - Visual studio 6
 * - Visual studio 2008 / Windows CE
 * - MinGw 32
 * - Linux i686
 * - Linux x86_64
 * - GCC wiith embedded ARM.
 * - Linux armel
 * - HCS08 with Freescale compiler.
 * - SDCC (Z80 and 8051)
 * 
 * @author	Mario Viara
 * 
 * @version	1.07
 * 
 * @copyright	Copyright Mario Viara 2014	- License Open Source (LGPL)
 * This is a free software and is opened for education, research and commercial
 * developments under license policy of following terms:
 * 
 * - This is a free software and there is NO WARRANTY.
 * - No restriction on use. You can use, modify and redistribute it for personal,
 *	 non-profit or commercial product UNDER YOUR RESPONSIBILITY.
 * - Redistributions of source code must retain the above copyright notice.
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include  "xformatc.h"



/**
 * Default largest int to long
 */
#ifndef	LONG
#define	LONG	long
#endif

/**
 * Define the double type if not defined
 */
#ifndef DOUBLE
#define DOUBLE	double
#endif


/**
 * Default long long type
 */
#ifndef LONGLONG
#define LONGLONG   long long
#endif

/**
 * Structure with all parameter used
 */
struct param_s
{
	/**
	 * Buffer for current intger value and for temporary
	 * double value defined as union to save stack.
	 */
	union
	{
		unsigned LONG		lvalue;
#if XCFG_FORMAT_LONGLONG
		unsigned LONGLONG	llvalue;
#endif
#if XCFG_FORMAT_FLOAT
		DOUBLE				dvalue;
#endif
	} values;

	/**
	 * Pointer to the output buffer
	 */
	char*		out;

#if XCFG_FORMAT_FLOAT
	/**
	 * Floating point argument
	 */
	DOUBLE		dbl;

	/**
	 * Fractional part of floating point
	 */
	unsigned LONG	fPart;

#endif

	
	/**
	 * Current lenght of the output buffer
	 */
	int		length;

	/**
	 * Field precision
	 */
	int		prec;

	/**
	 * Field width
	 */
	int		width;


	/**
	 * Count the number of char emitted
	 */
	unsigned	count;


	/**
	 * Union with all flags
	 */
	union
	{

		unsigned flags;

		struct
		{
			/* Type of integer field */
			unsigned	fl_type:2;

			
#define FLTYPE_INT			0	/* Argument is integer	*/
#define FLTYPE_LONG			1	/* Argument is long		*/
#define FLTYPE_SIZEOF		2	/* Argument is size_t	*/
#define FLTYPE_LONGLONG		3	/* Argument is long long*/
			
			/* Precision set */
			unsigned	fl_prec:1;


			/* Left alignment */
			unsigned	fl_left:1;

			/* Blank before positive integer number */
			unsigned	fl_blank:1;

			/* Prefix required */
			unsigned	fl_prefix:1;

			/* force a + before positive number */
			unsigned	fl_plus:1;

			/* Output in upper case letter */
			unsigned	fl_upper:1;

			/* Decimal field */
			unsigned	fl_decimal:1;

			/* Integer field */
			unsigned	fl_integer:1;

			/* Field is negative */
			unsigned	fl_minus:1;


			/* Value set */
			unsigned	fl_value:1;

			/* Buffer set */
			unsigned	fl_buffer:1;

		} flag;
	} flags;


	/**
	 * Length of the prefix
	 */
	int		prefixlen;

	/* Buffer to store the field  prefix */
	char prefix[2];


	/** Buffer to store the biggest integer number in binary */
#if XCFG_FORMAT_LONGLONG
	char		buffer[sizeof(LONGLONG)*8+1];
#else
	char		buffer[sizeof(LONG)*8+1];
#endif

	/* Radix for ASCII integer conversion */
	unsigned char	radix;

	/* char used for padding */
	char		pad;


	/**
	 * Current state
	 */
	char		state;

};


/**
 * Enum for the internal state machine
 */ 
enum State
{
	/* Normal state outputting literal */
	ST_NORMAL = 0,

	/* Just read % */
	ST_PERCENT = 1,

	/* Just read flag */
	ST_FLAG = 2,

	/* Just read the width */
	ST_WIDTH = 3,

	/* Just read '.' */
	ST_DOT= 4,

	/* Just read the precision */
	ST_PRECIS = 5,

	/* Just	 read the size */
	ST_SIZE = 6,

	/* Just read the type specifier */
	ST_TYPE = 7 
};

/**
 * Enum for char class
 */ 
enum CharClass
{
	/* Other char */
	CH_OTHER = 0,
	/* The % char */
	CH_PERCENT = 1,
	/* The . char */
	CH_DOT = 2,
	/* The * char */
	CH_STAR = 3,
	/* The 0 char */
	CH_ZERO = 4,
	/* Digit 0-9 */
	CH_DIGIT = 5,
	/* Flag chars */
	CH_FLAG = 6,
	/* Size chars */
	CH_SIZE = 7,
	/* Type chars */
	CH_TYPE = 8
};



/**
 * String used when %s is a null parameter
 */
static const char  ms_null[] = "(null)";
/*
 * String for true value
 */
static const char  ms_true[] = "True";

/**
 * String for false value
 */
static const char  ms_false[]= "False";


/*
 * Digit values
 */
static const char ms_digits[] = "0123456789abcdef";
   
   
/*
 * This table contains the next state for all char and it will be
 * generate using xformattable.c
 */

static const unsigned char formatStates[] =
{
	0x06,0x00,0x00,0x06,0x00,0x01,0x00,0x00,
	0x10,0x00,0x03,0x06,0x00,0x06,0x02,0x10,
	0x04,0x45,0x45,0x45,0x45,0x05,0x05,0x05,
	0x05,0x35,0x30,0x00,0x50,0x60,0x00,0x00,
	0x00,0x20,0x28,0x38,0x50,0x50,0x00,0x00,
	0x00,0x30,0x30,0x30,0x50,0x50,0x00,0x00,
	0x08,0x20,0x20,0x28,0x20,0x20,0x20,0x00,
	0x08,0x60,0x60,0x60,0x60,0x60,0x60,0x00,
	0x00,0x70,0x78,0x78,0x78,0x70,0x78,0x00,
	0x07,0x08,0x00,0x00,0x07,0x00,0x00,0x08,
	0x08,0x00,0x00,0x08,0x00,0x08,0x00,0x00,
	0x08,0x00,0x07
};




/**
 * Convert an unsigned value in one string
 *
 * All parameter are in the passed structure
 *
 * @param prec		- Minimum precision
 * @param lvalue	- Unsigned value
 * @param radix		- Radix (Supported values 2/8/10/16)
 *
 * @param out		- Buffer with the converted value.
 */
static void ulong2a(struct param_s * param)
{
	unsigned char digit;
	
	while (param->prec -- > 0 ||  param->values.lvalue)
	{
		switch (param->radix)
		{
			case 2:
				digit = param->values.lvalue & 0x01;
				param->values.lvalue >>= 1;
				break;

			case 8:
				digit = param->values.lvalue & 0x07;
				param->values.lvalue >>= 3;
				break;

			case 16:
				digit = param->values.lvalue & 0x0F;
				param->values.lvalue >>= 4;
				break;
				
			default:
			case 10:
				digit = param->values.lvalue % 10;
				param->values.lvalue /= 10;
				break;
		}


		*param->out -- = ms_digits[digit];
		param->length ++;

	}

}


#if XCFG_FORMAT_LONGLONG
#ifdef XCFG_FORMAT_LONG_ARE_LONGLONG
#define	ullong2a	ulong2a
#else
static void ullong2a(struct param_s * param)
{
	unsigned char digit;

	while (param->prec -- > 0 ||  param->values.llvalue)
	{
		switch (param->radix)
		{
			case 2:
				digit = param->values.llvalue & 0x01;
				param->values.llvalue >>= 1;
				break;

			case 8:
				digit = param->values.llvalue & 0x07;
				param->values.llvalue >>= 3;
				break;

			case 16:
				digit = param->values.llvalue & 0x0F;
				param->values.llvalue >>= 4;
				break;
				
			default:
			case 10:
				digit = param->values.llvalue % 10;
				param->values.llvalue /= 10;
				
				break;
		}


		*param->out -- = ms_digits[digit];
		param->length ++;

	}

}
#endif
#endif

/**
 * Printf like using variable arguments.
 * 
 * @param outchar - Pointer to the function to output one new char.
 * @param arg	- Argument for the output function.
 * @param fmt	- Format options for the list of parameters.
 * @param ...	- Arguments
 *
 * @return The number of char emitted.
 * 
 * @see xvformat
 */
unsigned xformat(void (*outchar)(void *,char),void *arg,const char * fmt,...)
{
	va_list list;
	unsigned count;

	va_start(list,fmt);
	count = xvformat(outchar,arg,fmt,list);
	va_end(list);

	(void)list;

	return count;
}


/**
 * We do not want use any library function.
 *
 * @param s - C	 string
 * @return The length of the string
 */
static unsigned xstrlen(const char *s)
{
	const char *i;

	for (i = s ; *i ; i++)
	{
	}

	return (unsigned)(i - s);
}

static unsigned outBuffer(void (*myoutchar)(void *arg,char),void *arg,const char *buffer,int len,unsigned char toupper)
{
	unsigned count = 0;
	int i;
	char c;

	for (i = 0; i < len ; i++)
	{
		c = buffer[i];

		if (toupper && (c >= 'a' && c <= 'z'))
		{
			c -= 'a' - 'A';
		}

		(*myoutchar)(arg,c);
		count++;
	}

	return count;
}


static unsigned outChars(void (*myoutchar)(void *arg,char),void *arg,char ch,int len)
{
	unsigned count= 0;

	while (len-- > 0)
	{
		(*myoutchar)(arg,ch);
		count++;
	}

	return count;
}



/*
 * Lint want declare list as const but list is an obscured pointer so
 * the warning is disabled.
 */
/*lint -save -e818 */


/**
 * Printf like format function.
 *
 * General format :
 * 
 * %[width][.precision][flags]type
 *
 * - width Is the minimum size of the field.
 * 
 * - precision Is the maximum size of the field.
 * 
 * Supported flags :
 * 
 * - l	With integer number the argument will be of type long.
 * - ll With integer number the argument will be of type long long.
 * -	Space for positive integer a space will be added before.
 * - z	Compatible with C99 the argument is size_t (aka sizeof(void *))
 * - +	A + sign prefix positive number.
 * - #	A prefix will be printed (o for octal,0x for hex,0b for binary)
 * - 0	Value will be padded with zero (default is spacwe)
 * - -	Left justify as default filed have rigth justification.
 * 
 * Supported type :
 * 
 * - s	Null terminated string of char.
 * - S	Null terminated string of char in upper case.
 * - i	Integer number.
 * - d	Integer number.
 * - u	Unsigned number.
 * - x	Unsigned number in hex.
 * - X	Unsigned number in hex upper case.
 * - b	Binary number
 * - o	Octal number
 * - p	Pointer will be emitted with the prefix ->
 * - P	Pointer in upper case letter.
 * - f	Floating point number.
 * - B	Boolean value printed as True / False.
 *
 * @param outchar - Pointer to the function to output one char.
 * @param arg	- Argument for the output function.
 * @param fmt	- Format options for the list of parameters.
 * @param args	-List parameters.
 *
 * @return The number of char emitted.
 */
unsigned xvformat(void (*outchar)(void *,char),void *arg,const char * fmt,va_list _args)
{
	XCFG_FORMAT_STATIC struct param_s param;
	int i;
	char c;

#if XCFG_FORMAT_VA_COPY
	va_list args;

	va_copy(args,_args);
#else
#define args	_args
#endif

	param.count = 0;
	param.state = ST_NORMAL;

	while (*fmt)
	{
		c = *fmt++;

		if (c < ' ' || c > 'z')
			i = (int)CH_OTHER;
		else
			i = formatStates[c - ' '] & 0x0F;

		param.state = formatStates[(i << 3) + param.state] >> 4;


		switch (param.state)
		{
			default:
			case	ST_NORMAL:
				(*outchar)(arg,c);
				param.count++;
				break;

			case	ST_PERCENT:
				param.length = param.prefixlen = param.width = param.prec = 0;
				param.flags.flags = 0;
				param.pad = ' ';
				break;

			case	ST_WIDTH:
				if (c == '*')
					param.width = (int)va_arg(args,int);
				else
					param.width = param.width * 10 + (c - '0');
				break;

			case	ST_DOT:
				break;

			case	ST_PRECIS:
				param.flags.flag.fl_prec = 1;
				if (c == '*')
					param.prec = (int)va_arg(args,int);
				else
					param.prec = param.prec * 10 + (c - '0');
				break;

			case	ST_SIZE:
				switch (c)
				{
					default:
						break;
					case 'z':
						param.flags.flag.fl_type = FLTYPE_SIZEOF;
						break;

					case 'l':
#if XCFG_FORMAT_LONGLONG
						if (param.flags.flag.fl_type == FLTYPE_LONG)
						{
							param.flags.flag.fl_type = FLTYPE_LONGLONG;
						}
						else
						{
							param.flags.flag.fl_type = FLTYPE_LONG;

						}
#else
						param.flags.flag.fl_type = FLTYPE_LONG;
#endif
						break;


				}
				break;

			case	ST_FLAG:
				switch (c)
				{
					default:
						break;
					case  '-':
						param.flags.flag.fl_left = 1;
						break;
					case  '0':
						param.pad = '0';
						break;
					case ' ':
						param.flags.flag.fl_blank = 1;
						break;
					case '#':
						param.flags.flag.fl_prefix = 1;
						break;
					case '+':
						param.flags.flag.fl_plus = 1;
						break;
				}
				break;

			case	ST_TYPE:
				switch (c)
				{
					default:
						param.length = 0;
						break;

						/*
						 * Pointer upper case
						 */
					case	'P':
						param.flags.flag.fl_upper = 1;
						/* no break */
						/*lint -fallthrough */

						/*
						 * Pointer 
						 */
					case	'p':
						param.flags.flag.fl_integer = 1;
						param.flags.flag.fl_type	= FLTYPE_SIZEOF;
						param.radix = 16;
						param.prec = sizeof(void *) * 2;
						param.prefix[0] = '-';
						param.prefix[1] = '>';
						param.prefixlen = 2;
						break;

						/*
						 * Binary number
						 */
					case	'b':
						param.flags.flag.fl_integer = 1;
						param.radix = 2;
						if (param.flags.flag.fl_prefix)
						{
							param.prefix[0] = '0';
							param.prefix[1] = 'b';
							param.prefixlen = 2;
						}
						break;

						/*
						 * Octal number
						 */
					case	'o':
						param.flags.flag.fl_integer = 1;
						param.radix = 8;
						if (param.flags.flag.fl_prefix)
						{
							param.prefix[0] = '0';
							param.prefixlen = 1;
						}
						break;

						/*
						 * Hex number upper case letter.
						 */
					case	'X':
						/* no break */
						param.flags.flag.fl_upper =1;

						/* no break */

						/* lint -fallthrough */

						/*
						 * Hex number lower case
						 */
					case	'x':
						param.flags.flag.fl_integer = 1;
						param.radix = 16;
						if (param.flags.flag.fl_prefix)
						{
							param.prefix[0] = '0';
							param.prefix[1] = 'x';
							param.prefixlen = 2;
						}
						break;

						/*
						 * Integer number radix 10
						 */
					case	'd':
					case	'i':
						param.flags.flag.fl_decimal =
						param.flags.flag.fl_integer = 1;
						param.radix = 10;
						break;

						/*
						 * Unsigned number
						 */
					case	'u':
						param.flags.flag.fl_integer = 1;
						param.radix = 10;
						break;

						/*
						 * Upper case string
						 */
					case	'S':
						param.flags.flag.fl_upper = 1;
						/* no break */
						/*lint -fallthrough */

						/*
						 * Normal string
						 */
					case	's':
						param.out = va_arg(args,char *);
						if (param.out == 0)
							param.out = (char *)ms_null;
						param.length = (int)xstrlen(param.out);
						break;

						/*
						 * Upper case char
						 */
					case	'C':
						param.flags.flag.fl_upper = 1;
						/* no break */
						/* lint -fallthrough */

						/*
						 * Char
						 */
					case	'c':
						param.out = param.buffer;
						param.buffer[0] = (char)va_arg(args,int);
						param.length = 1;
						break;

#if XCFG_FORMAT_FLOAT
						/**
						 * Floating point number
						 */
					case 'f':
						if (param.flags.flag.fl_prec == 0)
						{
							param.prec = 6;
						}

						param.dbl = va_arg(args,DOUBLE);
						param.values.dvalue = 0.51;
						for (i = 0 ; i < param.prec ; i++)
							param.values.dvalue /= 10.0;

						if (param.dbl < 0)
						{
							param.flags.flag.fl_minus = 1;
							param.dbl		-= param.values.dvalue;
							param.fPart	   = (LONG)param.dbl;
							param.dbl		-=	(LONG)param.fPart;
							param.dbl		 = - param.dbl;
						}
						else
						{
							param.dbl += param.values.dvalue;
							param.fPart = (LONG)param.dbl;
							param.dbl -= param.fPart;
						}


						for (i = 0 ;i < param.prec	;i++)
							param.dbl *= 10.0;

						param.values.lvalue = (unsigned LONG)param.dbl;

						param.out = param.buffer + sizeof(param.buffer) - 1;
						param.radix = 10;
						if (param.prec)
						{
							ulong2a(&param);
							*param.out -- = '.';
							param.length ++;
						}
						param.flags.flag.fl_integer =
							param.flags.flag.fl_buffer =
							param.flags.flag.fl_decimal =
							param.flags.flag.fl_value = 1;

						param.prec = 0;
						param.values.lvalue = (unsigned LONG)param.fPart;
						break;
#endif

						/**
						 * Boolean value
						 */
					case 'B':
						if (va_arg(args,int) != 0)
							param.out = (char*)ms_true;
						else
							param.out = (char*)ms_false;

						param.length = (int)xstrlen(param.out);
						break;


				}

				/*
				 * Process integer number
				 */
				if (param.flags.flag.fl_integer)
				{
					if (param.prec == 0)
						param.prec = 1;

					if (!param.flags.flag.fl_value)
					{
						if (param.flags.flag.fl_decimal)
						{
							switch (param.flags.flag.fl_type)
							{
								case FLTYPE_SIZEOF:
									param.values.lvalue = (unsigned LONG)va_arg(args,void *);
									break;
								case FLTYPE_LONG:
									param.values.lvalue = (LONG)va_arg(args,long);
									break;
								case FLTYPE_INT:
									param.values.lvalue = (LONG)va_arg(args,int);
									break;
#if XCFG_FORMAT_LONGLONG
								case FLTYPE_LONGLONG:
									param.values.llvalue = (LONGLONG)va_arg(args,long long);
									break;
#endif
							}

						}
						else
						{
							switch (param.flags.flag.fl_type)
							{
								case FLTYPE_SIZEOF:
									param.values.lvalue = (unsigned LONG)va_arg(args,void *);
									break;
								case FLTYPE_LONG:
									param.values.lvalue = (unsigned LONG)va_arg(args,unsigned long);
									break;
								case FLTYPE_INT:
									param.values.lvalue = (unsigned LONG)va_arg(args,unsigned int);
									break;
#if XCFG_FORMAT_LONGLONG
								case FLTYPE_LONGLONG:
									param.values.llvalue = (unsigned LONGLONG)va_arg(args,unsigned long long);
									break;
#endif
							}
						}

					}

					if ((param.flags.flag.fl_prefix) && param.values.lvalue == 0)
					{
						param.prefixlen = 0;
					}


					/*
					 * Manage signed integer
					 */
					if (param.flags.flag.fl_decimal)
					{
#if XCFG_FORMAT_LONGLONG
						if (param.flags.flag.fl_type == FLTYPE_LONGLONG)
						{
							if ((LONGLONG)param.values.llvalue < 0)
							{
								param.values.llvalue = ~param.values.llvalue + 1;
								param.flags.flag.fl_minus = 1;
							}
						}
						else 
						{
#endif
							if ((LONG)param.values.lvalue < 0)
							{
								param.values.lvalue = ~param.values.lvalue + 1;
								param.flags.flag.fl_minus = 1;

							}
#if XCFG_FORMAT_LONGLONG
						}
#endif
						if (!param.flags.flag.fl_minus && param.flags.flag.fl_blank)
						{
							param.prefix[0] = ' ';
							param.prefixlen = 1;
						}
					}

					if ((param.flags.flag.fl_buffer) == 0)
					{
						param.out = param.buffer + sizeof(param.buffer) - 1;
					}


#if XCFG_FORMAT_LONGLONG
					if (param.flags.flag.fl_type == FLTYPE_LONGLONG)
						ullong2a(&param);
					else
						ulong2a(&param);
#else

					ulong2a(&param);
#endif
					param.out++;

					/*
					 * Check if a sign is required
					 */
					if (param.flags.flag.fl_minus || param.flags.flag.fl_plus)
					{
						c = param.flags.flag.fl_minus ? '-' : '+';

						if (param.pad == '0')
						{
							param.prefixlen = 1;
							param.prefix[0] = c;
						}
						else
						{
							*--param.out = c;
							param.length++;
						}
					}


				}
				else
				{
					if (param.width && param.length > param.width)
					{
						param.length = param.width;
					}

				}

				/*
				 * Now width contain the size of the pad
				 */
				param.width -= (param.length + param.prefixlen);

				param.count += outBuffer(outchar,arg,param.prefix,param.prefixlen,(unsigned char)param.flags.flag.fl_upper);
				if (!(param.flags.flag.fl_left))
					param.count += outChars(outchar,arg,param.pad,param.width);
				param.count += outBuffer(outchar,arg,param.out,param.length,(unsigned char)param.flags.flag.fl_upper);
				if (param.flags.flag.fl_left)
					param.count += outChars(outchar,arg,param.pad,param.width);
				
		}
	}

#if XCFG_FORMAT_VA_COPY
	va_end(args);
#endif

	return param.count;
}

unsigned xsize = sizeof(struct param_s);

/*lint -restore */

