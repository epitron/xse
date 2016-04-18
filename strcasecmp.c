/*
 * strcasecmp(s,t): Just like strcmp(3) but ignores case.
 * strncasecmp(s,t): Just like strncmp(3) but ignores case.
 *
 * Adapted from strcmp() in K&&R, page 102.
 *
 * George Ferguson, ferguson@cs.rochester.edu, 12 Feb 1991.
 */
#include "ctype.h"

int
strcasecmp(s,t)
char *s,*t;
{
    while (*s == *t ||
	   (ISUPPER(*s) && TOLOWER(*s) == *t) ||
	   (ISUPPER(*t) && TOLOWER(*t) == *s)) {
	if (*s == '\0')
	    return(0);
	s++;
	t++;
    }
    return(*s - *t);
}

int
strncasecmp(s,t,n)
char *s,*t;
int n;
{
    while (n-- &&
	   (*s == *t ||
	    (ISUPPER(*s) && TOLOWER(*s) == *t) ||
	    (ISUPPER(*t) && TOLOWER(*t) == *s))) {
	if (*s == '\0' || n == 0)
	    return(0);
	s++;
	t++;
    }
    return(*s - *t);
}
