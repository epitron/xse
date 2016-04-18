/*
 * parse.c - Parse event specifications (see Xt manual, appendix B)
 *
 * George Ferguson, ferguson@cs.rochester.edu, 1 Jun 1990.
 *
 * Version 1.6 - 5 Jan 1992.
 *	gf: Changed `==' to `=' in parseEventList().
 * Version 2.0 - 31 Jan 1992.
 *	gf: Completely rewritten.
 * Version 2.1 -  4 Apr 1993.
 *	cek@sdc.boeing.com: Added auto-modifiers for Keysyms.
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include "xse.h"
#include "stringdefs.h"
#include "ctype.h"
#include "window.h"
#include "strtol.h"
typedef Bool Boolean;	/* silly Xlib */

/*
 * Functions defined here:
 */
Boolean parseEvent();

static Boolean parseMods(),parseType(),parseDetails();
static Boolean parseUsingTable(),parseBoolean(),parseButton();
static Boolean parseMotionHint(),parseNotifyMode(),parseNotifyDetail();
static Boolean parseExposeMajorCode(),parseStackMode();
static Boolean parseCirculateDetail(),parseMappingDetail();
static Boolean parseVisibilityDetail(),parsePropertyDetail();
static Boolean parseKeyCode(),parseInteger(),parseWindow();
static Boolean parseAtom(), parseClientMessage();
static Boolean parseCommonKeyPtrOpts(),parseXYWH();
static Boolean parseTwoOptWindows(),parseTwoOptWindowsAndBool();
static void error();
static int dprintf();

/*
 * Data defined here
 */
char *eventSpec;			/* original string, for messages */

typedef struct {
        char *name;
        unsigned int value;
        unsigned int mods;
        unsigned int detail;
} NameValueModsDetail;

static NameValueModsDetail types[] = {
{"KeyPress",	     KeyPress,		0,		0},
{"KeyPress++",	     -KeyPress,		0,		0},
{"Key", 	     KeyPress,		0,		0},
{"Key++", 	     -KeyPress,		0,		0},
{"KeyDown",	     KeyPress,		0,		0},
{"KeyDown++",	     -KeyPress,		0,		0},
{"Ctrl",             KeyPress,		ControlMask,	0},
{"Ctrl++",           -KeyPress,		ControlMask,	0},
{"Shift",            KeyPress,		ShiftMask,	0},
{"Shift++",          -KeyPress,		ShiftMask,	0},
{"KeyUp",	     KeyRelease,	0,		0},
{"KeyRelease",	     KeyRelease,	0,		0},
{"ButtonPress",      ButtonPress,	0,		AnyButton},
{"BtnDown",	     ButtonPress,	0,		AnyButton},
{"Btn1Down",	     ButtonPress,	0,		Button1},
{"Btn2Down", 	     ButtonPress,	0,		Button2},
{"Btn3Down", 	     ButtonPress,	0,		Button3},
{"Btn4Down", 	     ButtonPress,	0,		Button4},
{"Btn5Down", 	     ButtonPress,	0,		Button5},
{"ButtonRelease",    ButtonRelease,	0,		AnyButton},
{"BtnUp", 	     ButtonRelease,	0,		AnyButton},
{"Btn1Up", 	     ButtonRelease,	0,		Button1},
{"Btn2Up", 	     ButtonRelease,	0,		Button2},
{"Btn3Up", 	     ButtonRelease,	0,		Button3},
{"Btn4Up", 	     ButtonRelease,	0,		Button4},
{"Btn5Up", 	     ButtonRelease,	0,		Button5},
{"MotionNotify",     MotionNotify,	0,		0},
{"PtrMoved", 	     MotionNotify,	0,		0},
{"Motion", 	     MotionNotify,	0,		0},
{"MouseMoved", 	     MotionNotify,	0,		0},
{"BtnMotion",        MotionNotify,	0,		0},
{"Btn1Motion",       MotionNotify,	Button1Mask,	0},
{"Btn2Motion",       MotionNotify,	Button2Mask,	0},
{"Btn3Motion",       MotionNotify,	Button3Mask,	0},
{"Btn4Motion",       MotionNotify,	Button4Mask,	0},
{"Btn5Motion",       MotionNotify,	Button5Mask,	0},
{"EnterNotify",      EnterNotify,	0, 0},
{"Enter",	     EnterNotify,	0, 0},
{"EnterWindow",      EnterNotify,	0, 0},
{"LeaveNotify",      LeaveNotify,	0, 0},
{"LeaveWindow",      LeaveNotify,	0, 0},
{"Leave",	     LeaveNotify,	0, 0},
{"FocusIn",	     FocusIn,		0, 0},
{"FocusOut",	     FocusOut,		0, 0},
{"KeymapNotify",     KeymapNotify,	0, 0},
{"Keymap",	     KeymapNotify,	0, 0},
{"Expose",	     Expose,		0, 0},
{"Exp", 	     Expose,		0, 0},
{"GraphicsExpose",   GraphicsExpose,	0, 0},
{"GrExp",	     GraphicsExpose,	0, 0},
{"NoExpose",	     NoExpose,		0, 0},
{"NoExp",	     NoExpose,		0, 0},
{"CirculateNotify",  CirculateNotify,	0, 0},
{"Circ",	     CirculateNotify,	0, 0},
{"ConfigureNotify",  ConfigureNotify,	0, 0},
{"Configure",	     ConfigureNotify,	0, 0},
{"CreateNotify",     CreateNotify,	0, 0},
{"Create",	     CreateNotify,	0, 0},
{"DestroyNotify",    DestroyNotify,	0, 0},
{"Destroy",	     DestroyNotify,	0, 0},
{"GravityNotify",    GravityNotify,	0, 0},
{"Gravity",	     GravityNotify,	0, 0},
{"MapNotify",	     MapNotify,		0, 0},
{"Map",		     MapNotify,		0, 0},
{"MappingNotify",    MappingNotify,	0, 0},
{"Mapping",	     MappingNotify,	0, 0},
{"ReparentNotify",   ReparentNotify,	0, 0},
{"Reparent",	     ReparentNotify,	0, 0},
{"UnmapNotify",      UnmapNotify,	0, 0},
{"Unmap",	     UnmapNotify,	0, 0},
{"VisibilityNotify", VisibilityNotify,	0, 0},
{"Visibility",	     VisibilityNotify,	0, 0},
{"Visible",	     VisibilityNotify,	0, 0},
{"CirculateRequest", CirculateRequest,	0, 0},
{"CircReq",	     CirculateRequest,	0, 0},
{"ConfigureRequest", ConfigureRequest,	0, 0},
{"ConfigureReq",     ConfigureRequest,	0, 0},
{"MapRequest",	     MapRequest,	0, 0},
{"MapReq",	     MapRequest,	0, 0},
{"ResizeRequest",    ResizeRequest,	0, 0},
{"ResizeReq",	     ResizeRequest,	0, 0},
{"ResReq",	     ResizeRequest,	0, 0},
{"ColormapNotify",   ColormapNotify,	0, 0},
{"Clrmap",	     ColormapNotify,	0, 0},
{"ClientMessage",    ClientMessage,	0, 0},
{"Message",	     ClientMessage,	0, 0},
{"PropertyNotify",   PropertyNotify,	0, 0},
{"Prop",	     PropertyNotify,	0, 0},
{"SelectionClear",   SelectionClear,	0, 0},
{"SelClr",	     SelectionClear,	0, 0},
{"SelectionRequest", SelectionRequest,	0, 0},
{"SelReq",	     SelectionRequest,	0, 0},
{"SelectionNotify",  SelectionNotify,	0, 0},
{"Select",	     SelectionNotify,	0, 0},
{NULL,		     -1, 0, 0}
};

/*
 * Text to value mappings:
 */
typedef struct {
        char *name;
        unsigned int value;
	int len;
} NameValueLen;

static NameValueLen booleans[] = {
/*   name,	value,		len */
    {"True",	True,	4},
    {"False",	False,	5},
    {"On",	True,	2},
    {"Off",	False,	3},
    {"Yes",	True,	3},
    {"No",	False,	2},
    {NULL,	0,	0},
};

static NameValueLen modifiers[] = {
/*   name,	value,		len */
    {"None",	None,		4},
    {"Shift",	ShiftMask,	5},
    {"s",	ShiftMask,	1},
    {"Lock",	LockMask,	4},
    {"l",	LockMask,	1},
    {"Ctrl",	ControlMask,	4},
    {"c",	ControlMask,	1},
    {"Mod1",	Mod1Mask,	4},
    {"Mod2",	Mod2Mask,	4},
    {"Mod3",	Mod3Mask,	4},
    {"Mod4",	Mod4Mask,	4},
    {"Mod5",	Mod5Mask,	4},
    {"Button1",	Button1Mask,	7},
    {"Button2",	Button2Mask,	7},
    {"Button3",	Button3Mask,	7},
    {"Button4",	Button4Mask,	7},
    {"Button5",	Button5Mask,	7},
    {"Btn1",	Button1Mask,	4},
    {"Btn2",	Button2Mask,	4},
    {"Btn3",	Button3Mask,	4},
    {"Btn4",	Button4Mask,	4},
    {"Btn5",	Button5Mask,	4},
    {"Any",	AnyModifier,	3},
    {NULL,	0},
};

static NameValueLen buttonNames[] = {
/*   name,	value,		len */
    {"Button1",	Button1,	7},
    {"Button2", Button2,	7},
    {"Button3", Button3,	7},
    {"Button4", Button4,	7},
    {"Button5", Button5,	7},
    {"Btn1",	Button1,	4},
    {"Btn2",	Button2,	4},
    {"Btn3",	Button3,	4},
    {"Btn4",	Button4,	4},
    {"Btn5",	Button5,	4},
    {"AnyButton",AnyButton,	9},
    {"Any",	AnyButton,	3},
    {NULL,	0,		0},
};

static NameValueLen motionHints[] = {
/*   name,	value,		len */
    {"Normal",	NotifyNormal,	6},
    {"Hint",	NotifyHint,	4},
    {NULL,	0,		0},
};

static NameValueLen notifyModes[] = {
/*   name,		value,			len */
    {"Normal",		NotifyNormal,		6},
    {"Grab",		NotifyGrab,		4},
    {"Ungrab",		NotifyUngrab,		6},
    {"WhileGrabbed",    NotifyWhileGrabbed,	12},
    {NULL,		0,			0},
};

static NameValueLen notifyDetails[] = {
/*   name,		value,			len */
    {"Ancestor",	NotifyAncestor,		8},
    {"Virtual",		NotifyVirtual,		7},
    {"Inferior",	NotifyInferior,		8},
    {"Nonlinear",	NotifyNonlinear,	9},
    {"NonlinearVirtual",NotifyNonlinearVirtual,	16},
    {"PointerRoot",	NotifyPointerRoot,	11},	/* longer first */
    {"Pointer",		NotifyPointer,		7},
    {"DetailNone",	NotifyDetailNone,	10},
    {NULL,		0,			0},
};

static NameValueLen exposeMajorCodes[] = {
/*   name,		value,			len */
    {"CopyArea",	X_CopyArea,		8},
    {"CopyPlane",	X_CopyPlane,		9},
    {NULL,		0,			0},
};

static NameValueLen circulateDetails[] = {
/*   name,		value,			len */
    {"PlaceOnTop",	PlaceOnTop,		10},
    {"OnTop",		PlaceOnTop,		5},
    {"PlaceOnBottom", 	PlaceOnBottom,		13},
    {"OnBottom", 	PlaceOnBottom,		8},
    {NULL,		0,			0},
};

static NameValueLen mappingDetails[] = {
/*   name,		value,			len */
    {"Modifier",	MappingModifier,	8},
    {"Keyboard",	MappingKeyboard,	8},
    {"Pointer",		MappingPointer,		7},
    {NULL,		0,			0},
};

static NameValueLen visibilityDetails[] = {
/*   name,		 value,				len */
    {"Unobscured",	 VisibilityUnobscured,		10},
    {"PartiallyObscured",VisibilityPartiallyObscured,	17},
    {"FullyObscured",    VisibilityFullyObscured,	13},
    {NULL,		0,				0},
};

static NameValueLen stackModes[] = {
/*   name,		 value,		len */
    {"Above",		Above,		5},
    {"Below",		Below,		5},
    {"TopIf",		TopIf,		5},
    {"BottomIf",	BottomIf,	8},
    {"Opposite",	Opposite,	8},
    {NULL,		0,		0},
};

static NameValueLen propertyDetails[] = {
/*   name,		value,			len */
    {"NewValue",	PropertyNewValue,	8},
    {"New",		PropertyNewValue,	3},
    {"Delete",		PropertyDelete,		6},
    {NULL,		0,			0},
};

/*	-	-	-	-	-	-	-	-	*/
/*
 * Parses event in str into *evp, returns 0 if error, otherwise count.
 */
Boolean
parseEvent(str,xev,cntp)
char *str;
XEvent *xev;
int *cntp;
{
    unsigned int mods;
    int type;
    unsigned int detail;

    eventSpec = str;			/* set global for msgs */
    if (!parseMods(&str,&mods))
	return(False);
    SKIPWHITE(str);
    if (*str != '<') {
	error("'<' expected",str);
	return(False);
    } else
	str += 1;
    if (!parseType(&str,&type,&mods,&detail))
	return(False);
    if (*str != '>') {
	error("'>' expected",str);
	return(False);
    } else
	str += 1;
    SKIPWHITE(str);
    *cntp = 1;		/* default */
    if (*str == '(') {
	str += 1;
	if ((*cntp=(int)strtol(str,&str,0)) == 0) {
	    error("count expected",str);
	    return(False);
	}
	if (*str != ')')  {
	    error("')' expected",str);
	    return(False);
	} else
	    str += 1;
    }
    SKIPWHITE(str);
    xev->type = type;
    if (!parseDetails(str,xev,type,mods,detail))
	return(False);
    return(True);
}

/*
 * Parse string *sp into flags *modsp until '<'. Return False if error,
 * else True, and sets *sp to remainder of string.
 */
static Boolean
parseMods(sp,modsp)
char **sp;
unsigned int *modsp;
{
    unsigned int m;
    
    *modsp = (unsigned int)0;
    SKIPWHITE(*sp);
    while (**sp && **sp != '<') {
	if (!parseUsingTable(modifiers,sp,&m,"unknown modifier"))
	    return(False);
	*modsp |= m;
	dprintf("mods now %X\n",*modsp);
	SKIPWHITE(*sp);
    }
    return(True);
}

/*
 * Parse string in *sp into the three variables provided, and return
 * the start of the rest of the string in *sp. Some event types set
 * modifiers (eg. "Ctrl") and some set details (eg. "Btn1Dn") so these
 * have to passed. Return False if error, otherwise True.
 *
 * Can't use parseUsingTable() since we need to get back multiple values.
 */
static Boolean
parseType(sp,typep,modsp,detailp)
char **sp;
int *typep;
unsigned int *modsp,*detailp;
{
    char typestr[32];
    char *s;
    int i; 

    s = *sp;
    i = 0;
    while ((ISALNUM(*s) || *s == '+') && i < 30)
	typestr[i++] = *s++;
    typestr[i] = NUL;
    for (i=0; types[i].name != NULL &&
	      strcasecmp(types[i].name,typestr) != 0; i++)
	/*EMPTY*/;
    if (types[i].name == NULL) {
	error("unknown event type",*sp);
	return(False);
    }
    *typep = types[i].value;
    *detailp = types[i].detail;
    *modsp |= types[i].mods;
    dprintf("parsed event type \"%s\": ",typestr);
    dprintf("type = %d, detail = %d, mods = %X\n",*typep,*detailp,*modsp);
    *sp = s;
    return(True);
}

/*
 * Parse remainder of str in *xev, using initial information in type, mods,
 * and detail. Returns False if error, otherwise True. Doesn't update str.
 */
static Boolean
parseDetails(str,xevp,type,mods,detail)
char *str;
XEvent *xevp;
int type;
unsigned int mods,detail;
{
    switch (type) {
	case -KeyPress:
	case KeyPress:
	case KeyRelease: {
	    XKeyEvent *xkevp = (XKeyEvent *)xevp;
	    KeyCode kc;
	    unsigned int keysym_mods;

	    if (!parseKeyCode(&str,&kc,&keysym_mods))
		return(False);
	    xkevp->keycode = kc;
	    xkevp->state = mods;
	    if (type < 0) {	/* incorporate mods derived from keysym */
		xevp->type = type = -type;
		mods |= keysym_mods;
	    }
	    if (!parseCommonKeyPtrOpts(str,xevp,&(xkevp->same_screen)))
		return(False);
	    break;
	}
	case ButtonPress:
	case ButtonRelease: {
	    XButtonEvent *xbevp = (XButtonEvent *)xevp;
	    unsigned int b;
	    char *s;

	    if (ISALPHA(*str)) {
		s = str;
		if (parseButton(&str,&b)) {
		    if (detail != AnyButton && b != detail) {
			error("button detail conflict",s);
			return(False);
		    }
		} else {
		    return(False);
		}
	    } else {
		b = detail;
	    }
	    xbevp->button = b;
	    xbevp->state = mods;
	    if (!parseCommonKeyPtrOpts(str,xevp,&(xbevp->same_screen)))
		return(False);
	    break;
	}
	case MotionNotify: {
	    XMotionEvent *xmevp = (XMotionEvent *)xevp;
	    unsigned int h;

	    if (!parseMotionHint(&str,&h))
		return(False);
	    xmevp->is_hint = (char)h;
	    xmevp->state = mods;
	    if (!parseCommonKeyPtrOpts(str,xevp,&(xmevp->same_screen)))
		return(False);
	    break;
	}
	case EnterNotify:
	case LeaveNotify: {
	    XCrossingEvent *xcevp = (XCrossingEvent *)xevp;
	    unsigned int m;

	    if (!parseNotifyMode(&str,&m))
		return(False);
	    xcevp->mode = (int)m;
	    if (!parseNotifyDetail(&str,&m))
		return(False);
	    xcevp->detail = (int)m;
	    xcevp->state = mods;
	    xcevp->focus = True;
	    if (*str && !parseBoolean(&str,&(xcevp->focus)))
		return(False);
	    if (!parseCommonKeyPtrOpts(str,xevp,&(xcevp->same_screen)))
		return(False);
	    break;
	}
	case FocusIn:
	case FocusOut: {
	    XFocusChangeEvent *xfcevp = (XFocusChangeEvent *)xevp;
	    unsigned int m;

	    if (!parseNotifyMode(&str,&m))
		return(False);
	    xfcevp->mode = (int)m;
	    if (!parseNotifyDetail(&str,&m))
		return(False);
	    xfcevp->detail = (int)m;
	    xfcevp->window = window;
	    if (*str && !parseWindow(&str,&(xfcevp->window)))
		return(False);
	    break;
	}
	case KeymapNotify: {
	    XKeymapEvent *xkevp = (XKeymapEvent *)xevp;
	    int i;
	    char *s;
	    unsigned int m;

	    for (i=0; i < 32; i++)
		xkevp->key_vector[i] = '\0';
	    for (i=0; i < 32; i++) {
		SKIPWHITE(str);
		if (!*str) {
		    break;
		} else {
		    s = str;
		    if (!parseInteger(&str,&m))
			return(False);
		    if (m > 255) {
			error("key_vector entry > 255",s);
			return(False);
		    }
		    xkevp->key_vector[i] = (char)m;
		}
	    }
/* Window member is just ignored, and can't be set */
#ifdef undef
	    xkevp->window = window;
	    if (*str && !parseWindow(&str,&(xkevp->window)))
		return(False);
#endif
	    break;
	}
	case Expose: {
	    XExposeEvent *xeevp = (XExposeEvent *)xevp;

	    if (!parseXYWH(&str,&(xeevp->x),&(xeevp->y),
			   &(xeevp->width),&(xeevp->height)))
		return(False);
	    xeevp->count = 0;
	    if (*str && !parseInteger(&str,&(xeevp->count)))
		return(False);
	    xeevp->window = window;
	    if (*str && !parseWindow(&str,&(xeevp->window)))
		return(False);
	    break;
	}
	case GraphicsExpose: {
	    XGraphicsExposeEvent *xgevp = (XGraphicsExposeEvent *)xevp;

	    if (!parseXYWH(&str,&(xgevp->x),&(xgevp->y),
			   &(xgevp->width),&(xgevp->height)))
		return(False);
	    if (!parseExposeMajorCode(&str,&(xgevp->major_code)))
		return(False);
	    xgevp->minor_code = 0;
	    if (*str && !parseInteger(&str,&(xgevp->minor_code)))
		return(False);
	    xgevp->count = 0;
	    if (*str && !parseInteger(&str,&(xgevp->count)))
		return(False);
	    xgevp->drawable = window;
	    if (*str && !parseWindow(&str,&(xgevp->drawable)))
		return(False);
	    break;
	}
	case NoExpose: {
	    XNoExposeEvent *xnevp = (XNoExposeEvent *)xevp;

	    if (!parseExposeMajorCode(&str,&(xnevp->major_code)))
		return(False);
	    xnevp->minor_code = 0;
	    if (*str && !parseInteger(&str,&(xnevp->minor_code)))
		return(False);
	    xnevp->drawable = window;
	    if (*str && !parseWindow(&str,&(xnevp->drawable)))
		return(False);
	    break;
	}
	case CirculateNotify: {
	    XCirculateEvent *xcevp = (XCirculateEvent *)xevp;

	    if (!parseCirculateDetail(&str,&(xcevp->place)))
		return(False);
	    xcevp->window = window;
	    if (*str && !parseWindow(&str,&(xcevp->window)))
		return(False);
	    xcevp->event = xcevp->window;
	    if (*str && !parseWindow(&str,&(xcevp->event)))
		return(False);
	    break;
	}
	case ConfigureNotify: {
	    XConfigureEvent *xcevp = (XConfigureEvent *)xevp;

	    if (!parseXYWH(&str,&(xcevp->x),&(xcevp->y),
			   &(xcevp->width),&(xcevp->height)))
		return(False);
	    if (!parseInteger(&str,&(xcevp->border_width)))
		return(False);
	    if (!parseWindow(&str,&(xcevp->above)))
		return(False);
	    xcevp->window = window;
	    xcevp->override_redirect = False;
	    if (!parseTwoOptWindowsAndBool(&str,
					   &(xcevp->window),&(xcevp->event),
					   &(xcevp->override_redirect)))
		return(False);
	    break;
	}
	case CreateNotify: {
	    XCreateWindowEvent *xcevp = (XCreateWindowEvent *)xevp;

	    if (!parseXYWH(&str,&(xcevp->x),&(xcevp->y),
			   &(xcevp->width),&(xcevp->height)))
		return(False);
	    if (!parseInteger(&str,&(xcevp->border_width)))
		return(False);
	    xcevp->window = window;
	    xcevp->override_redirect = False;
	    if (!parseTwoOptWindowsAndBool(&str,
					   &(xcevp->window),&(xcevp->parent),
					   &(xcevp->override_redirect)))
		return(False);
	    break;
	}
	case DestroyNotify: {
	    XDestroyWindowEvent *xdevp = (XDestroyWindowEvent *)xevp;

	    xdevp->window = window;
	    if (!parseTwoOptWindows(&str,&(xdevp->window),&(xdevp->event)))
		return(False);
	    break;
	}
	case GravityNotify: {
	    XGravityEvent *xgevp = (XGravityEvent *)xevp;

	    if (!parseInteger(&str,&(xgevp->x)))
		return(False);
	    if (!parseInteger(&str,&(xgevp->y)))
		return(False);
	    xgevp->window = window;
	    if (!parseTwoOptWindows(&str,&(xgevp->window),&(xgevp->event)))
		return(False);
	    break;
	}
	case MapNotify: {
	    XMapEvent *xmevp = (XMapEvent *)xevp;

	    xmevp->window = window;
	    xmevp->override_redirect = False;
	    if (!parseTwoOptWindowsAndBool(&str,
					   &(xmevp->window),&(xmevp->event),
					   &(xmevp->override_redirect)))
		return(False);
	    break;
	}
	case MappingNotify: {
	    XMappingEvent *xmevp = (XMappingEvent *)xevp;

	    if (!parseMappingDetail(&str,&(xmevp->request)))
		return(False);
	    if (xmevp->request == MappingKeyboard) {
		if (!parseInteger(&str,&(xmevp->first_keycode)))
		    return(False);
		if (!parseInteger(&str,&(xmevp->count)))
		    return(False);
	    }
	    break;
	}
	case ReparentNotify: {
	    XReparentEvent *xrevp = (XReparentEvent *)xevp;

	    if (!parseWindow(&str,&(xrevp->parent)))
		return(False);
	    if (!parseInteger(&str,&(xrevp->x)))
		return(False);
	    if (!parseInteger(&str,&(xrevp->y)))
		return(False);
	    xrevp->window = window;
	    xrevp->override_redirect = False;
	    if (!parseTwoOptWindowsAndBool(&str,
					   &(xrevp->window),&(xrevp->event),
					   &(xrevp->override_redirect)))
		return(False);
	    break;
	}
	case UnmapNotify: {
	    XUnmapEvent *xuevp = (XUnmapEvent *)xevp;

	    xuevp->window = window;
	    xuevp->from_configure = False;
	    if (!parseTwoOptWindowsAndBool(&str,
					   &(xuevp->window),&(xuevp->event),
					   &(xuevp->from_configure)))
		return(False);
	    break;
	}
	case VisibilityNotify: {
	    XVisibilityEvent *xvevp = (XVisibilityEvent *)xevp;

	    if (!parseVisibilityDetail(&str,&(xvevp->state)))
		return(False);
	    xvevp->window = window;
	    if (*str && !parseWindow(&str,&(xvevp->window)))
		return(False);
	    break;
	}
	case CirculateRequest: {
	    XCirculateRequestEvent *xcevp = (XCirculateRequestEvent *)xevp;

	    if (!parseCirculateDetail(&str,&(xcevp->place)))
		return(False);
	    xcevp->window = window;
	    if (!parseTwoOptWindows(&str,&(xcevp->window),&(xcevp->parent)))
		return(False);
	    break;
	}
	case ConfigureRequest: {
	    XConfigureRequestEvent *xcevp = (XConfigureRequestEvent *)xevp;

	    if (!parseXYWH(&str,&(xcevp->x),&(xcevp->y),
			   &(xcevp->width),&(xcevp->height)))
		return(False);
	    if (!parseInteger(&str,&(xcevp->border_width)))
		return(False);
	    if (!parseWindow(&str,&(xcevp->above)))
		return(False);
	    xcevp->detail = Above;
	    if (*str && !parseStackMode(&str,&(xcevp->detail)))
		return(False);
	    xcevp->value_mask = 0;
	    if (*str && !parseInteger(&str,&(xcevp->value_mask)))
		return(False);
	    xcevp->window = window;
	    if (!parseTwoOptWindows(&str,&(xcevp->window),&(xcevp->parent)))
		return(False);
	    break;
	}
	case MapRequest: {
	    XMapRequestEvent *xmevp = (XMapRequestEvent *)xevp;

	    xmevp->window = window;
	    if (!parseTwoOptWindows(&str,&(xmevp->window),&(xmevp->parent)))
		return(False);
	    break;
	}
	case ResizeRequest: {
	    XResizeRequestEvent *xrevp = (XResizeRequestEvent *)xevp;

	    if (!parseInteger(&str,&(xrevp->width)))
		return(False);
	    if (!parseInteger(&str,&(xrevp->height)))
		return(False);
	    xrevp->window = window;
	    if (*str && !parseWindow(&str,&(xrevp->window)))
		return(False);
	    break;
	}
	case ColormapNotify:
	    error("can't send ColormapNotify events",NULL);
	    return(False);
	case ClientMessage: {
	    XClientMessageEvent *xcevp = (XClientMessageEvent *)xevp;
	    char format;

	    if (!parseAtom(&str,&(xcevp->message_type)))
		return(False);
	    SKIPWHITE(str);
	    switch ((format = *str)) {
		case 'b':
		case 'c': xcevp->format = 8;
			  break;
		case 's': xcevp->format = 16;
			  break;
		case 'l': xcevp->format = 32;
			  break;
		default:  error("message format expected",str);
			  return(False);
	    }
	    str += 1;
	    if (!parseClientMessage(&str,format,xcevp))
		return(False);
	    xcevp->window = window;
	    if (*str && !parseWindow(&str,&(xcevp->window)))
		return(False);
	    break;
	}
	case PropertyNotify: {
	    XPropertyEvent *xpevp = (XPropertyEvent *)xevp;

	    if (!parseAtom(&str,&(xpevp->atom)))
		return(False);
	    if (!parsePropertyDetail(&str,&(xpevp->state)))
		return(False);
	    xpevp->window = window;
	    if (*str && !parseWindow(&str,&(xpevp->window)))
		return(False);
	    xpevp->time = CurrentTime;
	    if (*str && !parseInteger(&str,&(xpevp->time)))
		return(False);
	    break;
	}
	case SelectionClear: {
	    XSelectionClearEvent *xsevp = (XSelectionClearEvent *)xevp;

	    if (!parseAtom(&str,&(xsevp->selection)))
		return(False);
	    xsevp->window = window;
	    if (*str && !parseWindow(&str,&(xsevp->window)))
		return(False);
	    xsevp->time = CurrentTime;
	    if (*str && !parseInteger(&str,&(xsevp->time)))
		return(False);
	    break;
	}
	case SelectionRequest: {
	    XSelectionRequestEvent *xsevp = (XSelectionRequestEvent *)xevp;

	    if (!parseAtom(&str,&(xsevp->selection)))
		return(False);
	    if (!parseAtom(&str,&(xsevp->target)))
		return(False);
	    xsevp->property = None;
	    if (*str && !parseAtom(&str,&(xsevp->property)))
		return(False);
	    xsevp->owner = window;
	    if (*str && !parseWindow(&str,&(xsevp->owner)))
		return(False);
	    xsevp->requestor = xsevp->owner;
	    if (*str && !parseWindow(&str,&(xsevp->requestor)))
		return(False);
	    xsevp->time = CurrentTime;
	    if (*str && !parseInteger(&str,&(xsevp->time)))
		return(False);
	    break;
	}
	case SelectionNotify: {
	    XSelectionEvent *xsevp = (XSelectionEvent *)xevp;

	    if (!parseAtom(&str,&(xsevp->selection)))
		return(False);
	    if (!parseAtom(&str,&(xsevp->target)))
		return(False);
	    xsevp->property = None;
	    if (*str && !parseAtom(&str,&(xsevp->property)))
		return(False);
	    xsevp->requestor = window;
	    if (*str && !parseWindow(&str,&(xsevp->requestor)))
		return(False);
	    xsevp->time = CurrentTime;
	    if (*str && !parseInteger(&str,&(xsevp->time)))
		return(False);
	    break;
	}
    }
    return(True);
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * parseUsingTable(): Generic table lookup function
 */
static Boolean
parseUsingTable(table,sp,ret,msg)
NameValueLen table[];
char **sp;
unsigned int *ret;
char *msg;
{
    int i;

    SKIPWHITE(*sp);
    for (i=0; table[i].name != NULL &&
		strncasecmp(*sp,table[i].name,table[i].len) != 0; i++)
	/*EMPTY*/;
    if (table[i].name == NULL) {
	error(msg,*sp);
	return(False);
    }
    *ret = table[i].value;
    dprintf("parsed \"%s\" in table %X, value = %d; ",*sp,table,*ret);
    *sp += table[i].len;
    dprintf("*sp now \"%s\"\n",*sp);
    return(True);
}

/*
 * Wrappers on the generic lookup function
 */

static Boolean
parseBoolean(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing Boolean\n");
    return(parseUsingTable(booleans,sp,ret,"boolean expected"));
}

static Boolean
parseButton(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing Button\n");
    return(parseUsingTable(buttonNames,sp,ret,"button name expected"));
}

static Boolean
parseMotionHint(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing MotionHint\n");
    return(parseUsingTable(motionHints,sp,ret,"motion hint expected"));
}

static Boolean
parseNotifyMode(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing NotifyMode\n");
    return(parseUsingTable(notifyModes,sp,ret,"notify mode expected"));
}

static Boolean
parseNotifyDetail(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing NotifyDetail\n");
    return(parseUsingTable(notifyDetails,sp,ret,"notify detail expected"));
}

static Boolean
parseExposeMajorCode(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing ExposeMajorCode\n");
    return(parseUsingTable(exposeMajorCodes,sp,ret,
			   "expose major code expected"));
}

static Boolean
parseCirculateDetail(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing CirculateDetail\n");
    return(parseUsingTable(circulateDetails,sp,ret,
			   "circulate detail expected"));
}

static Boolean
parseMappingDetail(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing MappingDetail\n");
    return(parseUsingTable(mappingDetails,sp,ret,"mapping detail expected"));
}

static Boolean
parseVisibilityDetail(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing VisibilityDetail\n");
    return(parseUsingTable(visibilityDetails,sp,ret,
			   "visibility detail expected"));
}

static Boolean
parseStackMode(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing StackMode\n");
    return(parseUsingTable(stackModes,sp,ret,"stack mode expected"));
}

static Boolean
parsePropertyDetail(sp,ret)
char **sp;
unsigned int *ret;
{
    dprintf("parsing PropertyDetail\n");
    return(parseUsingTable(propertyDetails,sp,ret,"property detail expected"));
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * Special parsing routines for things that aren't looked up, such as
 * keycodes, integers, and windows.
 */

#define GroupMask (Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask)
static unsigned int xkev_states[] = {
    0,
    ShiftMask,
    LockMask,
    GroupMask,
    ShiftMask | LockMask,
    ShiftMask | GroupMask,
    LockMask  | GroupMask,
    ShiftMask | LockMask | GroupMask,
};

static Boolean
parseKeyCode(sp,kcp,modsp)
char **sp;
KeyCode *kcp;
unsigned int *modsp;
{
    XKeyEvent xkev;
    KeySym ks, ks2;
    char *s,buf[64],buf2[1];
    int i;

    s = *sp;
    SKIPWHITE(s);
    i = 0;
    while (*s && !ISSPACE(*s) && i < 63)
	buf[i++] = *s++;
    buf[i] = NUL;
    if (i == 0) {
	error("keycode expected",*sp);
	return(False);
    }
    if ((ks=XStringToKeysym(buf)) == NoSymbol) {
	if (!ISDIGIT(buf[0])) {
	    error("no keysym for string",*sp);
	    return(False);
	} else {
	    char *cp;
	    ks=(KeySym)strtol(buf,&cp,0);
	    if (*cp != NUL) {
		error("no keysym for string",*sp);
		return(False);
	    }
	}
    }
    if ((*kcp=XKeysymToKeycode(display,ks)) == 0) {
	error("no keycode for keysym",*sp);
	return(False);
    }
    /* figure out which modifiers reproduce the original keysym */
    xkev.type = KeyPress;
    xkev.serial = 0L;
    xkev.send_event = False;
    xkev.display = display;
    xkev.x = xkev.y = xkev.x_root = xkev.y_root = 0;
    xkev.time = CurrentTime;
    xkev.same_screen = True;
    xkev.subwindow = None;
    xkev.window = window;
    xkev.root = root;
    xkev.keycode = *kcp;
    for (i = 0; i < sizeof(xkev_states)/sizeof(unsigned int); i++) {
	xkev.state = xkev_states[i];
	ks2 = 0;
	XLookupString(&xkev, buf2, sizeof(buf2), &ks2, NULL);
	if (ks2 == ks) break;
    }
    if (ks2 == ks) {
	*modsp = xkev.state;
    } else {
	error("can't determine modifiers for keysym",*sp);
	*modsp = 0;
    }
    dprintf("parsed keycode \"%s\" = %X\n",buf,*kcp);
    *sp = s;
    return(True);
}

static Boolean
parseInteger(sp,ip)
char **sp;
int *ip;
{
    char *s;

    s = *sp;
    SKIPWHITE(s);
    if (!ISDIGIT(*s)) {
	error("integer expected",*sp);
	return(False);
    }
    *ip = strtol(s,&s,0);
    if (!ISSPACE(*s) && *s != NUL) {
	error("whitespace or EOS expected after integer",*sp);
	return(False);
    }
    dprintf("parsed integer %d = %o = %X\n",*ip,*ip,*ip);
    *sp = s;
    return(True);
}

/*
 * Parse a window from *sp into *wp, updating *sp, as follows:
 *  - if *sp starts with a digit, then it must be an integer until the
 *    next whitespace character (ie. not "123xx", but "0x123" is ok).
 *  - it can be a special string "PointerWindow", "InputFocus", "None"
 *    or "Parent". In the last case, the parent of whatever is in *wp
 *    to start with is used. If the string is "Window" then *wp is set
 *    to "window". If the string is "Root", then the current root window
 *    is returned in *wp.
 *  - otherwise it must be the name of a window, which is searched for.
 * Returns False if there's an error, True otherwise.
 */
static Boolean
parseWindow(sp,wp)
char **sp;
Window *wp;
{
    Window w;
    char *s,buf[128];
    int i,slash;

    s = *sp;
    SKIPWHITE(s);
    i = slash = 0;
    while (*s && i < sizeof(buf)-1) {
	if (ISSPACE(*s) && !slash)
	    break;
	if (*s == '\\') {
	    if (slash)
		buf[i++] = *s;
	    slash = !slash;
	    s += 1;
	} else {
	    buf[i++] = *s++;
	    slash = 0;
	}
    }
    if (slash && i < sizeof(buf)-1)
	buf[i++] = '\\';
    buf[i] = NUL;
    if (i == 0) {
	error("window expected",*sp);
	return(False);
    } else if (i == sizeof(buf)-1) {
	error("window name too long",*sp);
	return(False);
    }
    if (!ISDIGIT(buf[0])) {
	if (strcmp(buf,"PointerWindow") == 0)
	    *wp = PointerWindow;
	else if (strcmp(buf,"InputFocus") == 0)
	    *wp = InputFocus;
	else if (strcmp(buf,"Window") == 0)
	    *wp = window;
	else if (strcmp(buf,"Parent") == 0)
	    *wp = WindowParent(*wp);
	else if (strcmp(buf,"Root") == 0)
	    *wp = root;
	else if (strcmp(buf,"None") == 0)
	    *wp = None;
	else if ((w=Window_With_Name(display,root,buf)) != (Window)NULL)
	    *wp = w;
	else {
	    error("bad window",*sp);
	    return(False);
	}
    } else if ((*wp=(Window)strtol(buf,NULL,0)) == (Window)0) {
	error("whitespace or EOS expected after window",*sp);
	return(False);
    }
    dprintf("parsed window \"%s\" = %X\n",buf,*wp);
    *sp = s;
    return(True);
}

static Boolean
parseAtom(sp,ap)
char **sp;
Atom *ap;
{
    char *s,buf[64];
    int i;

    s = *sp;
    SKIPWHITE(s);
    i = 0;
    while (*s && !ISSPACE(*s) && i < 63)
	buf[i++] = *s++;
    buf[i] = NUL;
    if (i == 0) {
	error("atom expected",*sp);
	return(False);
    }
    *ap = XInternAtom(display,buf,False);	/* only_if_exists == False */
    dprintf("parsed atom \"%s\" = %X\n",buf,*ap);
    *sp = s;
    return(True);
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * Parses options common to Key, Button, and Motion events. Returns the
 * same_screen member rather than setting it since its position can vary.
 */
static Boolean
parseCommonKeyPtrOpts(str,xevp,samep)
char *str;
XEvent *xevp;
Boolean *samep;
{
    XKeyEvent *xkevp = (XKeyEvent *)xevp;

    xkevp->x = xkevp->y = xkevp->x_root = xkevp->y_root = 0;
    xkevp->time = CurrentTime;
    xkevp->same_screen = True;
    xkevp->subwindow = None;
    xkevp->window = window;
    xkevp->root = root;
    SKIPWHITE(str);
    if (*str && !parseInteger(&str,&(xkevp->x)))
	return(False);
    SKIPWHITE(str);
    if (*str && !parseInteger(&str,&(xkevp->y)))
	return(False);
    SKIPWHITE(str);
    if (*str && !parseInteger(&str,&(xkevp->x_root)))
	return(False);
    SKIPWHITE(str);
    if (*str && !parseInteger(&str,&(xkevp->y_root)))
	return(False);
    SKIPWHITE(str);
    if (*str && !parseInteger(&str,&(xkevp->time)))
	return(False);
    SKIPWHITE(str);
    if (*str && !parseBoolean(&str,samep))	/* same_screen */
	return(False);
    SKIPWHITE(str);
    if (*str && !parseWindow(&str,&(xkevp->window)))
	return(False);
    SKIPWHITE(str);
    if (*str && !parseWindow(&str,&(xkevp->subwindow)))
	return(False);
    SKIPWHITE(str);
    if (*str && !parseWindow(&str,&(xkevp->root)))
	return(False);
    return(True);
}

/*
 * Parses four required integers into the given variables. Returns False
 * if there's an error, True otherwise, and updates *sp.
 */
static Boolean
parseXYWH(sp,xp,yp,wp,hp)
char **sp;
int *xp,*yp,*wp,*hp;
{
    SKIPWHITE(*sp);
    if (!parseInteger(sp,xp))
	return(False);
    SKIPWHITE(*sp);
    if (!parseInteger(sp,yp))
	return(False);
    SKIPWHITE(*sp);
    if (!parseInteger(sp,wp))
	return(False);
    SKIPWHITE(*sp);
    if (!parseInteger(sp,hp))
	return(False);
    return(True);
}

/*
 * Parses two optional windows into the given variables. After parsing the
 * first one (defaults to window), it sets the second to the first one
 * then parses the second one.
 */
static Boolean
parseTwoOptWindows(sp,w1p,w2p)
char **sp;
Window *w1p,*w2p;
{
    SKIPWHITE(*sp);
    *w1p = window;
    if (**sp && !parseWindow(sp,w1p))
	return(False);
    SKIPWHITE(*sp);
    *w2p = *w1p;
    if (**sp && !parseWindow(sp,w2p))
	return(False);
    return(True);
}

static Boolean
parseTwoOptWindowsAndBool(sp,w1p,w2p,bp)
char **sp;
Window *w1p,*w2p;
Boolean *bp;
{
    if (!parseTwoOptWindows(sp,w1p,w2p))
	return(False);
    SKIPWHITE(*sp);
    if (**sp && !parseBoolean(sp,bp))
	return(False);
    return(True);
}

static Boolean
parseClientMessage(sp,format,xcevp)
char **sp;
char format;
XClientMessageEvent *xcevp;
{
    int i,j;
    char *s;

    switch (format) {
	case 'b':
	    for (i=0; i < 20; i++) {
		SKIPWHITE(*sp);
		if (!**sp)
		    break;
		s = *sp;
		if (!parseInteger(sp,&j))
		    return(False);
		if (j > 255) {
		    error("`b' value > 255",s);
		    return(False);
		}
		xcevp->data.b[i] = (char)j;
	    }
	case 's':
	    for (i=0; i < 10; i++) {
		SKIPWHITE(*sp);
		if (!**sp)
		    break;
		s = *sp;
		if (!parseInteger(sp,&j))
		    return(False);
		if (j > 65535) {
		    error("`s' value > 65535",s);
		    return(False);
		}
		xcevp->data.s[i] = (short)j;
	    }
	case 'l':
	    for (i=0; i < 5; i++) {
		SKIPWHITE(*sp);
		if (!**sp)
		    break;
		s = *sp;
		if (!parseInteger(sp,&j))
		    return(False);
		xcevp->data.l[i] = (long)j;
	    }
	case 'c':
	    for (i=0; i < 20; i++) {
		if (!**sp)
		    break;
		if (**sp == '\\') {
		    *sp += 1;
		    if (**sp == '\\' || **sp == ' ') {
			xcevp->data.b[i] = **sp;
			*sp += 1;
		    } else if (**sp == 'n') {
			xcevp->data.b[i] = '\n';
			*sp += 1;
		    } else if (**sp == 't') {
			xcevp->data.b[i] = '\t';
			*sp += 1;
		    } else if (**sp == 'r') {
			xcevp->data.b[i] = '\r';
			*sp += 1;
		    } else if (**sp == 'f') {
			xcevp->data.b[i] = '\f';
			*sp += 1;
		    } else if (**sp == 'b') {
			xcevp->data.b[i] = '\b';
			*sp += 1;
		    } else if (!ISOCTDIGIT(**sp) || !ISOCTDIGIT(*(*sp+1)) ||
			       !ISOCTDIGIT(*(*sp+2))) {
			error("three octal digits expected",*sp);
			return(False);
		    } else {
			xcevp->data.b[i] = (**sp - '0')*64 +
			    (*(*sp+1) - '0')*8 + (*(*sp+2) - '0');
			*sp += 3;
		    }
		} else {
		    xcevp->data.b[i] = **sp;
		    *sp += 1;
		}
		dprintf("parsed character '%c' = %d = %o = %X\n",
			xcevp->data.b[i],xcevp->data.b[i],xcevp->data.b[i],
			xcevp->data.b[i]);
	    }
    }
    return(True);
}

/*	-	-	-	-	-	-	-	-	*/
/*
 * error() : First argument is error indication, second is remainder of
 *	string after point of error.
 */
static void
error(s1,s2)
char *s1,*s2;
{
    int n;

    fprintf(stderr,"%s: %s, ",program,s1);
    if (filename != NULL)
	fprintf(stderr,"file \"%s\", line %d\n",filename,linenum);
    else if (linenum != 0)
	fprintf(stderr,"event argument %d\n",linenum);
    else
	fprintf(stderr,"xse-send()\n");
    if (s2 != NULL) {
	fprintf(stderr,"%*s  context: \"%s\"\n",strlen(program),"",eventSpec);
	fprintf(stderr,"%*s  error is -",strlen(program),"");
	n = s2 - eventSpec;
	while (n-- > 0)
	    fprintf(stderr,"-");
	fprintf(stderr,"^ here\n");
    }
}

/*VARARGS*/
static int
dprintf(fmt,a1,a2,a3,a4)
char *fmt,*a1,*a2,*a3,*a4;
{
    if (debug)
	return(printf(fmt,a1,a2,a3,a4));
    return(0);
}
