/*
 * xse - an interface to XSendEvent()
 *
 * George Ferguson, ferguson@cs.rochester.edu,  1 Jun 1990.
 *
 * Version 1.6 - 5 Jan 1992.
 * Version 2.0 - 31 Jan 1992.
 *
 */
#include <stdio.h>
#include <ctype.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>	
#include <X11/Xaw/Form.h>	
#include <X11/Xaw/Command.h>	
#include "translate.h"
#include "parse.h"
#include "window.h"
#include "patchlevel.h"
#include "strtol.h"

/*	-	-	-	-	-	-	-	-	*/
/*
 * Functions defined in this file
 */
static void initGraphics(),initWidgets();
static void parseAndSendFile(),parseAndSendEvents();
static Window windowFromString();
static void quit(),send(),fail();

/*
 * Action binding table
 */
static XtActionsRec cmdActionsTable[] = {
    { "xse-quit", quit },
    { "xse-send", send },
};

/*
 * Global graphics data:
 */
Display *display;
Window root,window;

/*
 * Global widget data:
 */
static XtAppContext appContext;
static Widget toplevel;

/*
 * Other global data:
 */
char *program;		/* executable name, for messages */
char *filename;		/* file when -file given, else NULL */
int linenum;		/* line when -file, argv index when options, else 0 */
int debug;		/* True for debugging */

/*
 * The application resources structure
 */
typedef struct {
	String window;
	String widgets;
	String file;
	Boolean versionOnly;
	Boolean debug;
} AppResources;
static AppResources appResources;

/*
 * Non-widget resources obtained from resource manager
 */
static XtResource resources[] = {
    { "widgets", "Widgets", XtRString, sizeof(String),
      XtOffset(AppResources *,widgets), XtRImmediate, (XtPointer)NULL },
    { "window", "Window", XtRString, sizeof(String),
      XtOffset(AppResources *,window), XtRImmediate,(XtPointer)"ClickWindow" },
    { "file", "File", XtRString, sizeof(String),
      XtOffset(AppResources *,file), XtRImmediate,(XtPointer)NULL },
    { "versionOnly", "VersionOnly", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,versionOnly), XtRImmediate, (XtPointer)False },
    { "debug", "Debug", XtRBoolean, sizeof(Boolean),
      XtOffset(AppResources *,debug), XtRImmediate, (XtPointer)False },
};

/*
 * Non-widget resources set on command line.
 */
static XrmOptionDescRec options[] = {
    { "-window",	"window",	XrmoptionSepArg,	"ClickWindow"},
    { "-file",		"file",		XrmoptionSepArg,	""},
    { "-version",	"versionOnly",	XrmoptionNoArg,		"True"},
    { "-Debug",		"debug",	XrmoptionNoArg,		"True"},
};

/*
 * Widget and non-widget resources if the application defaults file can't
 * be found.
 * [ Generated automatically from Xse.ad by "ad2c". ]
 */
static String fallbackResources[] = {
#include "Xse.ad.h"
	NULL
};

/*	-	-	-	-	-	-	-	-	*/
/*
 * main() : Initialize the graphics, then if args remain send them directly
 *	(ie. we are in command mode), else if -file given do that,
 *	otherwise initialize the widgets  (ie. we are in widget mode)
 *	and call XtAppMainLoop().
 */
main(argc, argv)
int argc;
char **argv;
{
    program = argv[0];
    initGraphics(&argc,argv);
    XSynchronize(display,True);
    if (appResources.versionOnly) {
	printf("Xse %d.%d%s -- George Ferguson, ferguson@cs.rochester.edu\n",
	       XSE_MAJOR_VERSION,XSE_MINOR_VERSION,XSE_EXTRA_VERSION);
	exit(0);
    }
    debug = appResources.debug;
    argv += 1;
    argc -= 1;
    if (argc > 0 && appResources.file && *(appResources.file)) {
	fail("command-line arguments incompatible with file use");
    } else if (appResources.file && *(appResources.file)) {
	parseAndSendFile(appResources.file);
	XtDestroyApplicationContext(appContext);
	exit(0);
    } else if (argc > 0) {
	window = windowFromString(appResources.window);
	filename = NULL;
	linenum = 0;
	while (argc--) {
	    linenum += 1;
	    parseAndSendEvents(window,*argv++);
	}
	XtDestroyApplicationContext(appContext);
	exit(0);
    }
    window = windowFromString(appResources.window);
    filename = NULL;
    linenum = 0;
    initWidgets();
    XtRealizeWidget(toplevel);
    XtAppMainLoop(appContext);
    /*NOTREACHED*/
}

/*
 * initGraphics() : Initialize the application context and set global
 *	graphics variables.
 */
static void
initGraphics(argcp,argv)
int *argcp;
char **argv;
{
    toplevel = XtAppInitialize(&appContext, "Xse",
			       options, XtNumber(options),
			       argcp,argv,fallbackResources,NULL,0);
    XtGetApplicationResources(toplevel,(XtPointer)&appResources,
                              resources,XtNumber(resources),NULL,0);
    display = XtDisplay(toplevel);
    root = GetRootWindow(display,DefaultScreen(display));
    XtAppAddActions(appContext,cmdActionsTable,XtNumber(cmdActionsTable));
}

/*
 * initWidgets() : Parse the widgets resource and create whatever widgets
 *	specified in it as children of toplevel (or sub-children, etc.).
 */
static void
initWidgets()
{
    char name[32],class[32],parent[32];
    char *s;
    int i;
    Boolean isShell;
    WidgetClass wc;
    Widget w;

    if ((s=appResources.widgets) == NULL)
	fail("no widgets specified!\n","");
    while (*s) {
        while (isspace(*s))
            s += 1;
	if (!*s)
	    break;
        i = 0;
        while (*s && !isspace(*s))
            class[i++] = *s++;
        class[i] = '\0';
        while (isspace(*s))
            s += 1;
        i = 0;
        while (*s && !isspace(*s))
            name[i++] = *s++;
        name[i] = '\0';
        while (isspace(*s))
            s += 1;
        i = 0;
        while (*s && !isspace(*s))
            parent[i++] = *s++;
        parent[i] = '\0';
	isShell = False;
        if ((wc=classNameToWidgetClass(class,&isShell)) == NULL)
	    fail("can't convert string \"%s\" to widgetClass\n",class);
	if (strcmp(parent,"toplevel") == 0)
	    w = toplevel;
	else if ((w=XtNameToWidget(toplevel,parent)) == NULL)
	    fail("can't convert string \"%s\" to widget\n",parent);
	if (isShell)
            w = XtCreatePopupShell(name,wc,w,NULL,0);
        else
            w = XtCreateManagedWidget(name,wc,w,NULL,0);
    }
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * parseAndSendFile() : Read each line of the file, then parse and send
 * it.
 */
static void
parseAndSendFile(file)
char *file;
{
    FILE *fp;
    char buf[512];

    if ((fp=fopen(appResources.file,"r")) == NULL) {
	perror(file);
	return;
    }
    /* set window after making sure file is ok */
    window = windowFromString(appResources.window);
    filename = file;
    linenum = 0;
    while (fgets(buf,sizeof(buf),fp)) {
	buf[strlen(buf)-1] = '\0';
	linenum += 1;
	if (buf[0] && buf[0] != '#')		/* skip comment */
	    parseAndSendEvents(window,buf);
    }
    fclose(fp);
}

/*
 * parseAndSendEvents() : Calls parseEvent() then dispatches the returned
 *	event as many times as required. We save and restore "window" because
 *	the parsing routines use it as a default, and it can change when this
 *	is called by xse-send(). We want the correct default or events can
 *	vanish despite being sent to the right window.
 */
static void
parseAndSendEvents(w,str)
Window w;
char *str;
{
    Window oldWindow;
    XEvent xev;
    int cnt;

    /* clear out just in case */
    bzero(&xev,sizeof(XEvent));
    oldWindow = window;
    window = w;
    if (parseEvent(str,&xev,&cnt)) {
	while (cnt--) {
	    if (XSendEvent(display,w,True,0xfff,&xev) == 0)
		printf("xse: XSendEvent: conversion to wire protocol failed for event: \"%s\"\n",str);
	}
    }
    window = oldWindow;
}

/*
 * windowFromString() : Convert a string to a Window, handling our special
 *	cases of PointerWindow, InputFocus, or ClickWindow.
 */
static Window
windowFromString(str)
char *str;
{
    Window w;

    if (strcmp(str,"PointerWindow") == 0)
	return(PointerWindow);
    else if (strcmp(str,"InputFocus") == 0)
	return(InputFocus);
    else if (strcmp(str,"ClickWindow") == 0) {
	fprintf(stderr,"Click mouse in desired window...\n");
	return(Select_Window(display));
    } else if ((w=Window_With_Name(display,root,str)) != (Window)NULL)
	return(w);
    else
	return((Window)strtol(str,NULL,0));
}

/*	-	-	-	-	-	-	-	-	*/
/* Action procedures */

/*
 * quit() : Quit the tool.
 */
/*ARGSUSED*/
static void
quit(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
    XtDestroyApplicationContext(appContext);
    exit(0);
}

/*
 * send() : With one argument, send the event sequence given by the
 *	argument to the default window. With two arguments, the first
 *	is the window to send the event sequence given by the second to.
 *	With no arguments, propagate the event to the default window.
 */
/*ARGSUSED*/
static void
send(w,event,params,num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
    Window win;
    char *espec;

    if (*num_params == 0) {
	XSendEvent(display,window,True,0xfff,event);
	return;
    } else if (*num_params == 1) {
	win = window;
        espec = params[0];
    } else if (*num_params == 2) {
	if ((win=windowFromString(params[0])) == (Window)NULL) {
	    fprintf(stderr,"%s: xse-send(): bad window: \"%s\"\n",
		    program,params[0]);
	    return;
	}
        espec = params[1];
    } else {
	fprintf(stderr,"%s: too many arguments to xse-send()\n",program);
        return;
    }
    parseAndSendEvents(win,espec);
}

/*	-	-	-	-	-	-	-	-	*/
/*
 *	fail() : Print a message and die.
 */
static void
fail(fmt,arg)
char *fmt,*arg;
{
    fprintf(stderr,"%s: ",program);
    fprintf(stderr,fmt,arg);
    XtDestroyApplicationContext(appContext);
    exit(1);
}
