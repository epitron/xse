/*
 * window.h : External defs for window-finding routines
 *
 * George Ferguson, ferguson@cs.rochester.edu, 31 Jan 1992.
 *
 */

extern Window WindowParent(),GetRootWindow();
extern Bool WindowOverrideRedirect();
extern Window Window_With_Name(), Select_Window();
