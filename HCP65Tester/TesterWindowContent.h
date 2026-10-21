#ifndef __TESTER_WINDOW_CONTENT_H__
#define __TESTER_WINDOW_CONTENT_H__
#include "SupportLib/Font.h"
#include "WindowLib/Window.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/ScrollPane.h"
#include "WindowLib/TextArea.h"
#include "WindowLib/ButtonBar.h"
#include "WindowLib/Button.h"
#include "TesterButtonListener.h"


//The tester window's components.  A grid fills the window: its top row, which takes all the spare space, is a scroll
//pane showing a text area without word wrap and its bottom row is a bar with OK and Cancel aligned right.  The text and
//the captions are drawn in a 10 pixel JetBrains Mono generated from the TrueType font when the window is made.
//
//The window must already be in the root, see CTesterWindowContent::Init.
class CTesterWindowContent
{
protected:
	Ptr<CFont>				mpFont;
	Ptr<CGridContainer>		mpGrid;
	Ptr<CScrollPane>		mpScrollPane;
	Ptr<CTextArea>			mpTextArea;
	Ptr<CButtonBar>			mpButtonBar;
	Ptr<CButton>			mpOK;
	Ptr<CButton>			mpCancel;
	CTesterButtonListener	mcListener;

public:
	bool			Init(Ptr<CWindow> pWindow, const char* szFontFilename, const char* szTextFilename);
	void			Kill(void);

protected:
	Ptr<CFont>		GenerateFont(const char* szFontFilename, float32 fPixelSize, EColourOrder eOrder);
	bool			ReadText(const char* szTextFilename, CChars* pszText);
};


#endif // __TESTER_WINDOW_CONTENT_H__

