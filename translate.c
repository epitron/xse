/*
 *	translate.c : Map class names to classes. This code is
 *		      totally self-contained. A smaller executable
 *		      might result from only including the classes
 *		      an application needs.
 *
 *      George Ferguson, ferguson@cs.rochester.edu, 15 Jul 1990.
 *
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Grip.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Xaw/Simple.h>
#include <X11/Xaw/StripChart.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Shell.h>

static struct tableRec {
	char *name;
	WidgetClass *class;
	Boolean isShell;
} table[] = {
	{ "Command",	&commandWidgetClass,	False },
	{ "Grip",	&gripWidgetClass,	False },
	{ "Label",	&labelWidgetClass,	False },
	{ "List",	&listWidgetClass,	False },
	{ "Scrollbar",	&scrollbarWidgetClass,	False },
	{ "Simple",	&simpleWidgetClass,	False },
	{ "StripChart",	&stripChartWidgetClass,	False },
	{ "Toggle",	&toggleWidgetClass,	False },
	{ "SimpleMenu",	&simpleMenuWidgetClass,	True },
	{ "SmeBSB",	&smeBSBObjectClass,	False },
	{ "SmeLine",	&smeLineObjectClass,	False },
	{ "MenuButton",	&menuButtonWidgetClass,	False },
	{ "AsciiText",	&asciiTextWidgetClass,	False },
	{ "Box",	&boxWidgetClass,	False },
	{ "Dialog",	&dialogWidgetClass,	False },
	{ "Form",	&formWidgetClass,	False },
	{ "Paned",	&panedWidgetClass,	False },
	{ "Viewport",	&viewportWidgetClass,	False },
	{ "TransientShell", &transientShellWidgetClass, True },
	{ "ApplicationShell", &applicationShellWidgetClass, True },
};

WidgetClass
classNameToWidgetClass(name,isShell)
char *name;
Boolean *isShell;
{
    int i;

    for (i=0; i < XtNumber(table); i++)
	if (strcmp(name,table[i].name) == 0) {
	    *isShell = table[i].isShell;
	    return(*(table[i].class));
	}
    return((WidgetClass)NULL);
}
