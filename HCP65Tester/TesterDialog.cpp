#include "StandardLib/Objects.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/FillContainer.h"
#include "WindowLib/Label.h"
#include "TesterDialog.h"


#define DIALOG_INSET		16  //Between the window's edge and the form and buttons.
#define DIALOG_LABEL_GAP	12  //Between a label and its editor.
#define DIALOG_ROW_GAP		4  //Between rows.
#define DIALOG_DIVIDER_GAP	10  //Above a divider.
#define DIALOG_BUTTON_GAP	16  //Between the form and the buttons.


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void STesterDialogLook::Kill(void)
{
	pTheme = NULL;
	pDimFont = NULL;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialogTick::Init(void)
{
	PreInit();
	CWindowTick::Init();
	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialogTick::Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds)
{
	pWindow->Paint();
}


//////////////////////////////////////////////////////////////////////////
//
// The dialog's window shares the owner's native window factory and
// colour format.
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::Init(Ptr<CWindow> pOwner, STesterDialogLook* psLook, const char* szTitle)
{
	Ptr<CFillContainer>		pFill;
	Ptr<CGridContainer>		pGrid;
	SInsets					sInsets;
	int						aiInsets[4] = { DIALOG_INSET, DIALOG_INSET, DIALOG_INSET, DIALOG_INSET };

	mpsLook = psLook;
	mpOwner = pOwner;
	mpcApprover = NULL;
	mszClicked.Init();
	miRows = 0;

	mpDialog = OMalloc<CWindow>(szTitle, pOwner->GetFactory(), OMalloc<CTesterDialogTick>(), OMalloc<CClearCanvasDraw>(psLook->sBackground), pOwner->GetColourFormat(), pOwner->GetColourOrder(), pOwner->GetColourBits(), pOwner->GetAlphaBits());
	ORoot()->Add(mpDialog);

	pFill = OMalloc<CFillContainer>(mpDialog);
	sInsets.Init(aiInsets);
	pFill->SetInsets(&sInsets);
	mpDialog->SetContainer(pFill);

	//The form at the top and the buttons at the bottom.
	pGrid = OMalloc<CGridContainer>(mpDialog);
	pGrid->SetAlignment(CSH_Stretch, CSV_Stretch);
	pFill->AddComponent(pGrid);

	mpForm = OMalloc<CGridContainer>(mpDialog);
	mpForm->SetAlignment(CSH_Stretch, CSV_Top);
	pGrid->AddComponent(mpForm, 0, 0, 1, 1, 1, 1);

	mpOK = OMalloc<CButton>(mpDialog, psLook->pTheme, "OK");
	mpCancel = OMalloc<CButton>(mpDialog, psLook->pTheme, "Cancel");
	mpOK->SetDesiredSize(72, -1);
	mpCancel->SetDesiredSize(72, -1);
	mpOK->AddListener(this);
	mpCancel->AddListener(this);
	mpDialog->SetDefaultButton(mpOK);
	mpDialog->SetCancelButton(mpCancel);

	mpButtons = OMalloc<CButtonBar>(mpDialog);
	mpButtons->SetInsets(0, 0, 0, 0);
	mpButtons->SetAlignment(CSH_Stretch, CSV_Bottom);
	mpButtons->AddComponent(mpOK);
	mpButtons->AddComponent(mpCancel);
	pGrid->AddComponent(mpButtons, 0, 1, 1, 1, 1, 0);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::Kill(void)
{
	mpOK->RemoveListener(this);
	mpCancel->RemoveListener(this);
	if (mpExtra.IsNotNull())
	{
		mpExtra->RemoveListener(this);
	}

	mpExtra = NULL;
	mpFirstEditor = NULL;
	mpcApprover = NULL;
	mpOK = NULL;
	mpCancel = NULL;
	mpButtons = NULL;
	mpForm = NULL;
	ORoot()->Remove(mpDialog);
	mpDialog = NULL;
	mpOwner = NULL;
	mszClicked.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// The label is as wide as its text plus the gap, the editor takes the
// rest of the row.
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::AddRow(const char* szLabel, Ptr<CComponent> pEditor)
{
	Ptr<CLabel>		pLabel;
	Ptr<CCanvas>	pGap;

	if (miRows > 0)
	{
		pGap = OMalloc<CCanvas>(mpDialog, OMalloc<CClearCanvasDraw>(mpsLook->sBackground));
		pGap->SetDesiredSize(1, DIALOG_ROW_GAP);
		mpForm->AddComponent(pGap, 0, miRows);
		miRows++;
	}

	if (mpFirstEditor.IsNull() && pEditor->CanGetFocus())
	{
		mpFirstEditor = pEditor;
	}

	if (szLabel)
	{
		pLabel = OMalloc<CLabel>(mpDialog, mpsLook->pTheme->GetFont(), szLabel);
		pLabel->SetDesiredSize(pLabel->GetRequiredSize().x + DIALOG_LABEL_GAP, -1);
		pLabel->SetAlignment(CSH_Left, CSV_Center);
		mpForm->AddComponent(pLabel, 0, miRows);
		mpForm->AddComponent(pEditor, 1, miRows, 1, 1, 1, 0);
	}
	else
	{
		mpForm->AddComponent(pEditor, 0, miRows, 2, 1, 1, 0);
	}
	miRows++;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
Ptr<CTextField> CTesterDialog::AddTextField(const char* szLabel, const char* szText)
{
	Ptr<CTextField>		pField;

	pField = OMalloc<CTextField>(mpDialog, mpsLook->pTheme, szText);
	pField->SetColumns(10);
	pField->SelectAll();
	pField->SetAlignment(CSH_Stretch, CSV_Center);
	AddRow(szLabel, pField);
	return pField;
}


//////////////////////////////////////////////////////////////////////////
//
// A check box is its own label, it sits in the editor's column.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CCheckBox> CTesterDialog::AddCheckBox(const char* szLabel, bool bTicked)
{
	Ptr<CCheckBox>	pCheckBox;

	pCheckBox = OMalloc<CCheckBox>(mpDialog, mpsLook->pTheme, szLabel, bTicked);
	pCheckBox->SetAlignment(CSH_Left, CSV_Center);
	AddRow("", pCheckBox);
	return pCheckBox;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
Ptr<CComboBox> CTesterDialog::AddComboBox(const char* szLabel, const char** aszItems, int iNumItems, int iSelected)
{
	Ptr<CComboBox>	pCombo;
	int				i;

	pCombo = OMalloc<CComboBox>(mpDialog, mpsLook->pTheme);
	for (i = 0; i < iNumItems; i++)
	{
		pCombo->AddItem(aszItems[i]);
	}
	pCombo->Select(iSelected);
	pCombo->SetAlignment(CSH_Stretch, CSV_Center);
	AddRow(szLabel, pCombo);
	return pCombo;
}


//////////////////////////////////////////////////////////////////////////
//
// An editor made by the caller, on a row with its label.
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::AddEditor(const char* szLabel, Ptr<CComponent> pEditor)
{
	AddRow(szLabel, pEditor);
}


//////////////////////////////////////////////////////////////////////////
//
// A title and a line across the rest of the form, as Logicim's divider
// fields are shown.
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::AddDivider(const char* szTitle)
{
	Ptr<CGridContainer>	pDivider;
	Ptr<CLabel>			pTitle;
	Ptr<CCanvas>		pLine;
	Ptr<CCanvas>		pGap;

	pGap = OMalloc<CCanvas>(mpDialog, OMalloc<CClearCanvasDraw>(mpsLook->sBackground));
	pGap->SetDesiredSize(1, DIALOG_DIVIDER_GAP);
	mpForm->AddComponent(pGap, 0, miRows);
	miRows++;

	pDivider = OMalloc<CGridContainer>(mpDialog);
	pTitle = OMalloc<CLabel>(mpDialog, mpsLook->pDimFont, szTitle);
	pTitle->SetDesiredSize(pTitle->GetRequiredSize().x + 8, -1);
	pTitle->SetAlignment(CSH_Left, CSV_Center);
	pLine = OMalloc<CCanvas>(mpDialog, OMalloc<CClearCanvasDraw>(mpsLook->sDivider));
	pLine->SetDesiredSize(-1, 1);
	pLine->SetAlignment(CSH_Stretch, CSV_Center);
	pDivider->AddComponent(pTitle, 0, 0);
	pDivider->AddComponent(pLine, 1, 0, 1, 1, 1, 0);
	pDivider->SetDesiredSize(-1, pTitle->GetRequiredSize().y);
	pDivider->SetAlignment(CSH_Stretch, CSV_Center);
	AddRow(NULL, pDivider);
}


//////////////////////////////////////////////////////////////////////////
//
// Another button before OK, like Logicim's Save Defaults.  It closes the
// dialog too.
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::AddExtraButton(const char* szCaption)
{
	mpExtra = OMalloc<CButton>(mpDialog, mpsLook->pTheme, szCaption);
	mpExtra->AddListener(this);
	mpButtons->RemoveAllComponents();
	mpButtons->AddComponent(mpExtra);
	mpButtons->AddComponent(mpOK);
	mpButtons->AddComponent(mpCancel);
}


//////////////////////////////////////////////////////////////////////////
//
// The body fills the dialog above the buttons, in place of the rows.  The
// first editor has focus when the dialog opens.
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::SetBody(Ptr<CComponent> pBody, Ptr<CComponent> pFirstEditor)
{
	mpForm->RemoveAllComponents();
	mpForm->SetAlignment(CSH_Stretch, CSV_Stretch);
	pBody->SetAlignment(CSH_Stretch, CSV_Stretch);
	mpForm->AddComponent(pBody, 0, 0, 1, 1, 1, 1);
	mpFirstEditor = pFirstEditor;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::SetOKCaption(const char* szCaption)
{
	mpOK->SetCaption(szCaption);
}


//////////////////////////////////////////////////////////////////////////
//
// OK, or enter, only closes the dialog if the approver approves.
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::SetApprover(CTesterDialogApprover* pcApprover)
{
	mpcApprover = pcApprover;
}


//////////////////////////////////////////////////////////////////////////
//
// Shows the dialog iWidth wide and as tall as its rows and buttons.
// Returns true if OK closed it.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterDialog::Show(int32 iWidth)
{
	mpForm->SetRequiredSize();
	return Show(iWidth, DIALOG_INSET + mpForm->GetRequiredSize().y + DIALOG_BUTTON_GAP + mpOK->GetDesiredSize().y + DIALOG_INSET);
}


//////////////////////////////////////////////////////////////////////////
//
// Shows the dialog at the size given.  Returns true if OK closed it.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterDialog::Show(int32 iWidth, int32 iHeight)
{
	mpDialog->SetWindowSize(iWidth, iHeight);

	//Like IntelliJ's the first editor has focus, a text field with its text selected so typing replaces it.
	if (mpFirstEditor.IsNotNull())
	{
		mpDialog->GetFocus()->SetFocus(mpFirstEditor);
	}

	mszClicked.Kill();
	mszClicked.Init();
	mpDialog->ShowModal(mpOwner);
	return mszClicked.Equals("OK");
}


//////////////////////////////////////////////////////////////////////////
//
// Every button closes the dialog, OK as "OK" whatever its caption unless
// the approver stops it.
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::ButtonClicked(CUnknown* pcSource, void* pvContext)
{
	if (&mpOK == pcSource)
	{
		if ((mpcApprover == NULL) || mpcApprover->Approve())
		{
			Close("OK");
		}
		return;
	}
	Close(((CButton*)pcSource)->GetCaption());
}


//////////////////////////////////////////////////////////////////////////
//
// Closes the dialog as if the button captioned szClicked had been
// clicked, as a double click on a list's row closes it with OK.
//
//////////////////////////////////////////////////////////////////////////
void CTesterDialog::Close(const char* szClicked)
{
	mszClicked.Kill();
	mszClicked.Init(szClicked);
	mpDialog->Stop();
}


//////////////////////////////////////////////////////////////////////////
//
// The caption of the button that closed the dialog, empty if it was
// closed by the window manager.
//
//////////////////////////////////////////////////////////////////////////
char* CTesterDialog::GetClicked(void)
{
	return mszClicked.Text();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
Ptr<CWindow> CTesterDialog::GetWindow(void) { return mpDialog; }
STesterDialogLook* CTesterDialog::GetLook(void) { return mpsLook; }

