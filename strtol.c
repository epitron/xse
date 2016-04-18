/*
 * strtol.c : String-to-long conversion in various bases
 *
 * George Ferguson, 31 Jan 1992.
 *
 */
#include "ctype.h"

/*
 * Functions defined here:
 */
long strtol();
static int digit_value();

/*
 * Data defined here:
 */
static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

/*
 * strtol(str,ptr,base) : Returns as a long integer the value represented
 *	by "str". Leading whitespace is ignored. The value is converted
 *	according to "base". If "base" is 0, then "str" itself indicates
 *	the base: a leading zero means octal, a leading "0x" or "0X" means
 *	hexadecimal. If "ptr" is not NULL, then it is set to point to the
 *	character after the digits used in the conversion. If no number
 *	could be formed, it is set to "str" and zero is returned.
 */
long
strtol(str,ptr,base)
char *str,**ptr;
int base;
{
    long n;
    int value;

    /* sanity check */
    if (str == 0 || base < 0 || base > 36) {
	if (ptr)
	    *ptr = str;
	return(0L);
    }
    /* skip leading whitespace */
    SKIPWHITE(str);
    /* check base specifier in string */
    if (base == 0) {
	if (*str == '0') {
	    if (*(str+1) == 'x' || *(str+1) == 'X') {
		base = 16;
		str += 2;
	    } else {
		base = 8;
		str += 1;
	    }
	} else {
	    base = 10;
	}
    } else if (base == 16 && *str == '0' && (*str == 'x' || *str == 'X')) {
	str += 2;
    }
    /* parse digits */
    n = 0L;
    while ((value=digit_value(base,*str)) >= 0) {
	n = n * base + value;
	str += 1;
    }
    /* update return string */
    if (ptr)
	*ptr = str;
    /* return value */
    return(n);
}

/*
 * digit_value(base,digit) : Returns value of "digit" in "base", or -1 if
 *	"digit"	is not a valid digit in that base.
 */
static int
digit_value(base,digit)
int base;
char digit;
{
    int i;

    if (ISUPPER(digit))
	digit = TOLOWER(digit);
    for (i=0; i < base; i++)
	if (digit == digits[i])
	    return(i);
    return(-1);
}
