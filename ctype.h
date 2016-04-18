/*
 * ctype.h : Character type macros
 *
 * George Ferguson, ferguson@cs.rochester.edu, 31 Jan 1991.
 *
 */

#define NUL		'\0'
#define SPACE		' '
#define TAB		'\t'
#define NEWLINE		'\n'
#define ISSPACE(C)	((C) == SPACE || (C) == TAB || (C) == NEWLINE)
#define ISUPPER(C)	((C) >= 'A' && (C) <= 'Z')
#define ISLOWER(C)	((C) >= 'a' && (C) <= 'z')
#define ISALPHA(C)	(ISUPPER(C) || ISLOWER(C))
#define ISDIGIT(C)	((C) >= '0' && (C) <= '9')
#define ISALNUM(C)	(ISUPPER(C) || ISLOWER(C) || ISDIGIT(C))
#define ISOCTDIGIT(C)	((C) >= '0' && (C) <= '7')
#define ISHEXDIGIT(C)	(ISDIGIT(C) || ((C) >= 'A' && (C) <= 'F') || \
				       ((C) >= 'a' && (C) <= 'f'))

#define SKIPWHITE(S)	while (ISSPACE(*S)) S += 1

#define TOUPPER(C)	((C)-'a'+'A')
#define TOLOWER(C)	((C)-'A'+'a')
