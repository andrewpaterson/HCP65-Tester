#ifndef __TESTER_DIALOG_H__
#define __TESTER_DIALOG_H__
#include "SupportLib/Font.h"
#include "WindowLib/Window.h"
#include "WindowLib/WindowTick.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/ButtonBar.h"
#include "WindowLib/Button.h"
#include "WindowLib/TextField.h"
#include "WindowLib/CheckBox.h"
#include "WindowLib/ComboBox.h"
#include "WindowLib/WindowTheme.h"


//What every dialog is drawn with, made once by the window content.
struct STesterDialogLook
{
	Ptr<CWindowTheme>		pTheme;  //The widgets' look and the labels' font.
	Ptr<CFont>				pDimFont;  //For a divider's title.
	ARGB32					sBackground;
	ARGB32					sDivider;

	void Kill(void);
};


//Paints a dialog each tick.
class CTesterDialogTick : public CWindowTick
{
CONSTRUCTABLE(CTesterDialogTick);
DESTRUCTABLE(CTesterDialogTick);
public:
	void	Init(void);
	void	Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds) override;
};


//A form in its own window, shown modally over the tester's window, like Logicim's property editor dialogs but in
//IntelliJ's style: a label and an editor on each row, dividers with a title between groups of rows, and the buttons
//along the bottom on the right.  OK is the default button, enter clicks it; escape clicks Cancel.
//
//The dialog's window is in the root while the dialog exists.
class CTesterDialog : public CButtonListener
{
CONSTRUCTABLE(CTesterDialog);
protected:
	STesterDialogLook*		mpsLook;
	Ptr<CWindow>			mpOwner;
	Ptr<CWindow>			mpDialog;
	Ptr<CGridContainer>		mpForm;
	Ptr<CButtonBar>			mpButtons;
	Ptr<CButton>			mpOK;
	Ptr<CButton>			mpCancel;
	Ptr<CButton>			mpExtra;  //Like Save Defaults, NULL if there isn't one.
	Ptr<CComponent>			mpFirstEditor;  //Has focus when the dialog opens.
	CChars					mszClicked;
	int						miRows;

public:
	void				Init(Ptr<CWindow> pOwner, STesterDialogLook* psLook, const char* szTitle);
	void				Kill(void);

	Ptr<CTextField>		AddTextField(const char* szLabel, const char* szText);
	Ptr<CCheckBox>		AddCheckBox(const char* szLabel, bool bTicked);
	Ptr<CComboBox>		AddComboBox(const char* szLabel, const char** aszItems, int iNumItems, int iSelected);
	void				AddDivider(const char* szTitle);
	void				AddExtraButton(const char* szCaption);

	bool				Show(int32 iWidth);
	char*				GetClicked(void);

	void				ButtonClicked(CUnknown* pcSource, void* pvContext) override;

protected:
	void				AddRow(const char* szLabel, Ptr<CComponent> pEditor);
};


#endif // __TESTER_DIALOG_H__

