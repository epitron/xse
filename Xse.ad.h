"Xse.window:		ClickWindow",
"Xse.versionOnly:	False",
"Xse.debug:		False",
"Xse.widgets: \
	Form		form		toplevel \\n\
	Command		button1		form \\n\
	Command		button2		form \\n\
	Command		button3		form \\n",
"Xse*button1.label:	Send `a' to default window",
"Xse*button1.translations:#override\\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>a)\\n",
"Xse*button2.label:	Send `a' to ClickWindow",
"Xse*button2.fromVert:	button1",
"Xse*button2.translations:#override\\n\
	<Btn1Down>,<Btn1Up> : xse-send(ClickWindow,<Key>a)\\n",
"Xse*button3.label:	Quit Xse",
"Xse*button3.fromVert:	button2",
"Xse*button3.translations:#override\\n\
	<Btn1Down>,<Btn1Up> : xse-quit()\\n",
"xse-chester.window:	xchester",
"xse-chester.widgets:\
	Form		form		toplevel \\n\
	Command		hlpButton	form \\n\
	Command		catButton	form \\n\
	Command		stoButton	form \\n\
	Command		cmdButton	form \\n\
	Command		tilButton	form \\n\
	Command		autButton	form \\n\
	Command		subButton	form \\n\
	Command		numButton	form \\n\
	Command		forButton	form \\n\
	Command		bacButton	form \\n\
	Command		psButton	form \\n\
	Command		libButton	form \\n\
	Command		fulButton	form \\n\
	Command		brfButton	form \\n\
	Command		indButton	form \\n\
	Command		twoButton	form \\n\
	Command		closeButton	form \\n\
	Command		quitButton	form \\n",
"xse-chester*Form.translations:#override\\n\
	<Key> : xse-send() \\n",
"xse-chester*Command.width: 40",
"xse-chester*hlpButton.label:		HLP",
"xse-chester*hlpButton.fromHoriz:	NULL",
"xse-chester*hlpButton.fromVert:		NULL",
"xse-chester*hlpButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>H) \
			      xse-send(<Key>L) \
			      xse-send(<Key>P) \
			      xse-send(<Key>Return) \\n",
"xse-chester*catButton.label:		CAT",
"xse-chester*catButton.fromHoriz:	NULL",
"xse-chester*catButton.fromVert:		hlpButton",
"xse-chester*catButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>C) \
			      xse-send(<Key>A) \
			      xse-send(<Key>T) \
			      xse-send(<Key>Return) \\n",
"xse-chester*stoButton.label:		STO",
"xse-chester*stoButton.fromHoriz:	NULL",
"xse-chester*stoButton.fromVert:		catButton",
"xse-chester*stoButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>S) \
			      xse-send(<Key>T) \
			      xse-send(<Key>O) \
			      xse-send(<Key>Return) \\n",
"xse-chester*cmdButton.label:		CMD",
"xse-chester*cmdButton.fromHoriz:	NULL",
"xse-chester*cmdButton.fromVert:		stoButton",
"xse-chester*cmdButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>C) \
			      xse-send(<Key>M) \
			      xse-send(<Key>D) \
			      xse-send(<Key>Return) \\n",
"xse-chester*tilButton.label:		TIL",
"xse-chester*tilButton.fromHoriz:	hlpButton",
"xse-chester*tilButton.fromVert:		NULL",
"xse-chester*tilButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>T) \
			      xse-send(<Key>I) \
			      xse-send(<Key>L) \
			      xse-send(<Key>Return)\\n",
"xse-chester*autButton.label:		AUT",
"xse-chester*autButton.fromHoriz:	catButton",
"xse-chester*autButton.fromVert:		tilButton",
"xse-chester*autButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>A) \
			      xse-send(<Key>U) \
			      xse-send(<Key>T) \
			      xse-send(<Key>Return)\\n",
"xse-chester*subButton.label:		SUB",
"xse-chester*subButton.fromHoriz:	stoButton",
"xse-chester*subButton.fromVert:		autButton",
"xse-chester*subButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>S) \
			      xse-send(<Key>U) \
			      xse-send(<Key>B) \
			      xse-send(<Key>Return)\\n",
"xse-chester*numButton.label:		NUM",
"xse-chester*numButton.fromHoriz:	cmdButton",
"xse-chester*numButton.fromVert:		subButton",
"xse-chester*numButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>N) \
			      xse-send(<Key>U) \
			      xse-send(<Key>M) \
			      xse-send(<Key>Return)\\n",
"xse-chester*forButton.label:		FOR",
"xse-chester*forButton.fromHoriz:	tilButton",
"xse-chester*forButton.fromVert:		NULL",
"xse-chester*forButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>F) \
			      xse-send(<Key>O) \
			      xse-send(<Key>R) \
			      xse-send(<Key>Return)\\n",
"xse-chester*bacButton.label:		BAC",
"xse-chester*bacButton.fromHoriz:	autButton",
"xse-chester*bacButton.fromVert:		forButton",
"xse-chester*bacButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>B) \
			      xse-send(<Key>A) \
			      xse-send(<Key>C) \
			      xse-send(<Key>Return)\\n",
"xse-chester*psButton.label:		PS",
"xse-chester*psButton.fromHoriz:		subButton",
"xse-chester*psButton.fromVert:		bacButton",
"xse-chester*psButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>P) \
			      xse-send(<Key>S) \
			      xse-send(<Key>Return)\\n",
"xse-chester*libButton.label:		LIB",
"xse-chester*libButton.fromHoriz:	numButton",
"xse-chester*libButton.fromVert:		psButton",
"xse-chester*libButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>L) \
			      xse-send(<Key>I) \
			      xse-send(<Key>B) \
			      xse-send(<Key>Return)\\n",
"xse-chester*fulButton.label:		FUL",
"xse-chester*fulButton.fromHoriz:	forButton",
"xse-chester*fulButton.fromVert:		NULL",
"xse-chester*fulButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>F) \
			      xse-send(<Key>U) \
			      xse-send(<Key>L) \
			      xse-send(<Key>Return)\\n",
"xse-chester*brfButton.label:		BRF",
"xse-chester*brfButton.fromHoriz:	bacButton",
"xse-chester*brfButton.fromVert:		fulButton",
"xse-chester*brfButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>B) \
			      xse-send(<Key>R) \
			      xse-send(<Key>F) \
			      xse-send(<Key>Return)\\n",
"xse-chester*indButton.label:		IND",
"xse-chester*indButton.fromHoriz:	psButton",
"xse-chester*indButton.fromVert:		brfButton",
"xse-chester*indButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>I) \
			      xse-send(<Key>N) \
			      xse-send(<Key>D) \
			      xse-send(<Key>Return)\\n",
"xse-chester*twoButton.label:		TWO",
"xse-chester*twoButton.fromHoriz:	libButton",
"xse-chester*twoButton.fromVert:		indButton",
"xse-chester*twoButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(<Key>T) \
			      xse-send(<Key>W) \
			      xse-send(<Key>O) \
			      xse-send(<Key>Return)\\n",
"xse-chester*closeButton.label:		Close",
"xse-chester*closeButton.fromHoriz:	NULL",
"xse-chester*closeButton.fromVert:	cmdButton",
"xse-chester*closeButton.translations:#override\\n\
	<Key> : xse-send() \\n\
	<Btn1Down>,<Btn1Up> : xse-send(Ctrl<Key>]) \
			      xse-send(<Key>q) \
			      xse-send(<Key>u) \
			      xse-send(<Key>i) \
			      xse-send(<Key>t) \
			      xse-send(<Key>Return) \\n",
"xse-chester*quitButton.label:		Quit",
"xse-chester*quitButton.fromHoriz:	closeButton",
"xse-chester*quitButton.fromVert:	numButton",
"xse-chester*quitButton.translations:#override\\n\
	<Btn1Down>,<Btn1Up> : xse-quit() \\n",
