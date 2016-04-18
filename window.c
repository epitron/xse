/*
 * window.c : Window-finding routines
 *
 * George Ferguson, ferguson@cs.rochester.edu, 31 Jan 1992.
 *
 * Additional routines from the file "dsimple.c" used with xwininfo,
 * written by Mark Lillibridge.   Last updated 7/1/87.
 *
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xmu/WinUtil.h>
#include <X11/cursorfont.h>
extern Display *display;
extern Window root;

/*
 * Functions defined here:
 */
Window WindowParent(), GetRootWindow();
Bool WindowOverrideRedirect();
Window Window_With_Name(), Select_Window();

Window
WindowParent(w)
Window w;
{
    Window wroot,parent,*children;
    unsigned int nchildren;

    if (XQueryTree(display,w,&wroot,&parent,&children,&nchildren)) {
	XFree((char*)children);
	return(parent);
    } else {
	fprintf(stderr,"xse: XQueryTree failed!\n");
	return((Window)NULL);
    }
}

Bool
WindowOverrideRedirect(w)
Window w;
{
    XWindowAttributes attrs;

    (void)XGetWindowAttributes(display,w,&attrs);
    return(attrs.override_redirect);
}

/* Added for window managers like swm and tvtwm that follow solbourne's 
 * virtual root window concept 
 */
Window
GetRootWindow(disp,scrn)
Display *disp;
int scrn;
{
  Atom __SWM_VROOT = None;
  Window        root, rootReturn, parentReturn, *children;
  unsigned int  numChildren;
  int           i;

  root = RootWindow(disp, scrn);

  /* see if there is a virtual root */
  __SWM_VROOT = XInternAtom(disp, "__SWM_VROOT", False);
  XQueryTree(disp, root, &rootReturn, &parentReturn, &children, &numChildren);
  for(i = 0; i < numChildren; i++) {
    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytesafter;
    Window *newRoot = (Window)0;

    if (XGetWindowProperty (disp, children[i], __SWM_VROOT,0,1,
			    False, XA_WINDOW, &actual_type, &actual_format,
			    &nitems, &bytesafter,
			    (unsigned char **) &newRoot) ==
	Success && newRoot) {
      root = *newRoot;
      break;
    }
  }
  if (children)
      XFree((char *)children);
  return(root);
}

/*	-	-	-	-	-	-	-	-	-
 *
 * [These functions are from the file "dsimple.c" used with xwininfo.]
 *
 * Written by Mark Lillibridge.   Last updated 7/1/87
 *
 *
 * Window_With_Name: routine to locate a window with a given name on a display.
 *                   If no window with the given name is found, 0 is returned.
 *                   If more than one window has the given name, the first
 *                   one found will be returned.  Only top and its subwindows
 *                   are looked at.  Normally, top should be the Root Window.
 */
Window
Window_With_Name(dpy, top, name)
     Display *dpy;
     Window top;
     char *name;
{
	Window *children, dummy;
	unsigned int nchildren;
	int i;
	Window w=0;
	char *window_name;

	if (XFetchName(dpy, top, &window_name) && !strcmp(window_name, name))
	  return(top);

	if (!XQueryTree(dpy, top, &dummy, &dummy, &children, &nchildren))
	  return(0);

	for (i=0; i<nchildren; i++) {
		w = Window_With_Name(dpy, children[i], name);
		if (w)
		  break;
	}
	if (children) XFree ((char *)children);
	return(w);
}

/*
 * Routine to let user select a window using the mouse
 * gf: Doesn't need "screen" defined.
 * gf: Uses global "root" rather than calling DefaultRootWindow() again,
 *     also so virtual root windows can be handled.
 */
Window
Select_Window(dpy)
      Display *dpy;
{
  static Cursor cursor = (Cursor)NULL;
  Window target_win = None;
  Window win_root;
  XEvent event;
  int status,dummyi;
  unsigned int dummy;
  int buttons = 0;

  if (cursor == (Cursor)NULL)
    cursor = XCreateFontCursor(dpy, XC_crosshair);
  /* Grab the pointer using target cursor, letting it room all over */
  status = XGrabPointer(dpy, root, False,
			ButtonPressMask|ButtonReleaseMask, GrabModeSync,
			GrabModeAsync, root, cursor, CurrentTime);
  if (status != GrabSuccess) {
    fprintf(stderr,"xse: can't grab pointer");
    return(None);
  }
  /* Let the user select a window... */
  while ((target_win == None) || (buttons != 0)) {
    /* allow one more event */
    XAllowEvents(dpy, SyncPointer, CurrentTime);
    XWindowEvent(dpy, root, ButtonPressMask|ButtonReleaseMask, &event);
    switch (event.type) {
    case ButtonPress:
      if (target_win == None) {
	if ((target_win = event.xbutton.subwindow) != (Window)NULL &&
	    XGetGeometry(display,target_win,&win_root,&dummyi,&dummyi,
			 &dummy,&dummy,&dummy,&dummy) &&
	    target_win != win_root) {
	    target_win = XmuClientWindow(display,target_win);
	  }
	if (target_win == None) target_win = root;
      }
      buttons++;
      break;
    case ButtonRelease:
      if (buttons > 0) /* there may have been some down before we started */
	buttons--;
       break;
    }
  } 
  XUngrabPointer(dpy, CurrentTime);      /* Done with pointer */
  return(target_win);
}
