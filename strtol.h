/*
 * strtol.h : Declaration for strtol(), which is either in your library
 *            or comes from strtol.c.
 *
 * George Ferguson, ferguson@cs.rochester.edu, 31 Jan 1992.
 *
 */

#if NeedFunctionPrototypes || defined(FUNCPROTO)
extern long strtol(char *str, char **ptr, int base);
#else
extern long strtol();
#endif
