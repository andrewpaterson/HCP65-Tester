#include <stdlib.h>
#include "BaseLib/FileUtil.h"
#include "BaseLib/StringHelper.h"
#include "StandardLib/Objects.h"
#include "WindowLib/FillContainer.h"
#include "WindowLib/RoundedBoxCanvasDraw.h"
#include "WindowLib/IconComponent.h"
#include "TesterFileChooser.h"


#define FILE_CHOOSER_WIDTH			620
#define FILE_CHOOSER_HEIGHT			440
#define FILE_CHOOSER_ROW_HEIGHT		24
#define FILE_CHOOSER_ICON_WIDTH		30  //The column of the icons, a name starts after it.
#define FILE_CHOOSER_GAP			8  //Between the rows of the dialog, and between the buttons along the top.
#define FILE_CHOOSER_LABEL_GAP		12  //Between a label and what it labels.
#define NEW_FOLDER_DIALOG_WIDTH		360


struct SFileChooserType
{
	const char*		szDescription;
	const char*		szExtension;  //NULL for every file.
};


//The Files of Type choices, Logicim's files first as its file chooser's filter is.
static SFileChooserType gasFileChooserTypes[] =
{
	{ "Logicim Projects (*.xml)", "xml" },
	{ "All Files", NULL },
};
#define NUM_FILE_CHOOSER_TYPES	(sizeof(gasFileChooserTypes) / sizeof(SFileChooserType))


//////////////////////////////////////////////////////////////////////////
//
// A whole path without a trailing separator, unless it's a root.  A drive
// alone ("D:") is its root.
//
//////////////////////////////////////////////////////////////////////////
void NormaliseDirectory(CChars* pszDirectory)
{
	CFileUtil	cFileUtil;

	if (pszDirectory->EndsWith(':'))
	{
		pszDirectory->Append(FILE_SEPARATOR);
	}
	cFileUtil.FullPath(pszDirectory);
	if (pszDirectory->FindFromEnd(FILE_SEPARATOR[0]) == ARRAY_ELEMENT_NOT_FOUND)
	{
		pszDirectory->Append(FILE_SEPARATOR);
	}
	else if ((pszDirectory->Count(FILE_SEPARATOR[0]) > 1) && pszDirectory->EndsWith(FILE_SEPARATOR[0]))
	{
		pszDirectory->RemoveEnd(pszDirectory->Length() - 1);
	}
}


//////////////////////////////////////////////////////////////////////////
//
// A normalised directory with one separator, at its end, is a root.
//
//////////////////////////////////////////////////////////////////////////
static bool IsRootDirectory(CChars* pszDirectory)
{
	return (pszDirectory->Count(FILE_SEPARATOR[0]) == 1) && pszDirectory->EndsWith(FILE_SEPARATOR[0]);
}


//////////////////////////////////////////////////////////////////////////
//
// Whether the file's extension is szExtension, ignoring case.
//
//////////////////////////////////////////////////////////////////////////
static bool HasExtension(const char* szPath, const char* szExtension)
{
	CFileUtil	cFileUtil;
	size		uiExtension;

	uiExtension = cFileUtil.FindExtension(szPath);
	if (uiExtension == ARRAY_ELEMENT_NOT_FOUND)
	{
		return false;
	}
	return StringInsensitiveCompare(&szPath[uiExtension + 1], szExtension) == 0;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileChooserListListener::Init(CTesterFileChooser* pcChooser)
{
	mpcChooser = pcChooser;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileChooserListListener::SelectionChanged(CUnknown* pcSource, void* pvContext)
{
	mpcChooser->SelectionChanged();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileChooserListListener::ItemActivated(CUnknown* pcSource, void* pvContext)
{
	mpcChooser->ItemActivated();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileChooserComboListener::Init(CTesterFileChooser* pcChooser)
{
	mpcChooser = pcChooser;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileChooserComboListener::SelectionChanged(CUnknown* pcSource, void* pvContext)
{
	mpcChooser->ComboChanged(pcSource);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileChooserButtonListener::Init(CTesterFileChooser* pcChooser)
{
	mpcChooser = pcChooser;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CFileChooserButtonListener::ButtonClicked(CUnknown* pcSource, void* pvContext)
{
	mpcChooser->ButtonClicked(pcSource);
}


//////////////////////////////////////////////////////////////////////////
//
// Starts in the folder of szFilename with the file selected and its name
// in File Name, or in the current folder if that folder doesn't exist.
// The icons are shown beside the folders and files.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::Init(Ptr<CWindow> pOwner, STesterDialogLook* psLook, const char* szTitle, bool bSave, const char* szFilename, Ptr<CImageCel> pFolderIcon, Ptr<CImageCel> pFileIcon)
{
	CFileUtil		cFileUtil;
	Ptr<CComponent>	pBody;
	CChars			szPath;
	CChars			szName;
	CChars			szDirectory;
	size			i;

	mbSave = bSave;
	mpFolderIcon = pFolderIcon;
	mpFileIcon = pFileIcon;
	mszDirectory.Init();
	maszLookIn.Init();
	maszRows.Init();
	miNumFolders = 0;
	mszChosen.Init();
	mcListListener.Init(this);
	mcComboListener.Init(this);
	mcButtonListener.Init(this);

	mcDialog.Init(pOwner, psLook, szTitle);
	mcDialog.SetOKCaption(bSave ? "Save" : "Open");
	mcDialog.SetApprover(this);
	pBody = CreateBody();
	mcDialog.SetBody(pBody, mpFileName);

	for (i = 0; i < NUM_FILE_CHOOSER_TYPES; i++)
	{
		mpFileType->AddItem(gasFileChooserTypes[i].szDescription);
	}
	mpFileType->Select(0);
	mpFileType->AddListener(&mcComboListener);
	mpLookIn->AddListener(&mcComboListener);
	mpList->AddListener(&mcListListener);

	szPath.Init(szFilename);
	if (!szPath.Empty())
	{
		cFileUtil.FullPath(&szPath);
	}
	szName.Init();
	szDirectory.Init();
	cFileUtil.SplitPath(szPath.Text(), &szName, &szDirectory);
	if (szDirectory.Empty() || !Open(szDirectory.Text(), szPath.Text()))
	{
		szDirectory.Kill();
		szDirectory.Init();
		cFileUtil.CurrentDirectory(&szDirectory);
		Open(szDirectory.Text(), NULL);
	}
	mpFileName->SetText(szName.Text());
	mpFileName->SelectAll();
	szDirectory.Kill();
	szName.Kill();
	szPath.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::Kill(void)
{
	mpLookIn->RemoveListener(&mcComboListener);
	mpFileType->RemoveListener(&mcComboListener);
	mpList->RemoveListener(&mcListListener);
	mpUp->RemoveListener(&mcButtonListener);
	mpHome->RemoveListener(&mcButtonListener);
	mpNewFolder->RemoveListener(&mcButtonListener);

	mpFileIcon = NULL;
	mpFolderIcon = NULL;
	mpFileType = NULL;
	mpFileName = NULL;
	mpList = NULL;
	mpScrollPane = NULL;
	mpNewFolder = NULL;
	mpHome = NULL;
	mpUp = NULL;
	mpLookIn = NULL;
	mcDialog.Kill();

	mszChosen.Kill();
	maszRows.Kill();
	maszLookIn.Kill();
	mszDirectory.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// Look In and its buttons along the top, the folder's contents under
// them, then File Name and Files of Type, their labels lined up.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CComponent> CTesterFileChooser::CreateBody(void)
{
	Ptr<CWindow>			pWindow;
	STesterDialogLook*		psLook;
	Ptr<CGridContainer>		pBody;
	Ptr<CGridContainer>		pTop;
	Ptr<CFillContainer>		pGap;
	Ptr<CLabel>				pLabel;
	SInsets					sInsets;
	int						aiInsets[4] = { 0, 4, 0, 4 };
	int32					iLabelWidth;
	int						iRow;

	pWindow = mcDialog.GetWindow();
	psLook = mcDialog.GetLook();
	pBody = OMalloc<CGridContainer>(pWindow);

	//Every label is as wide as the widest.
	pLabel = OMalloc<CLabel>(pWindow, psLook->pTheme->GetFont(), "Files of Type:");
	iLabelWidth = pLabel->GetRequiredSize().x + FILE_CHOOSER_LABEL_GAP;

	//Look In and its buttons are a row of their own, so the buttons don't widen the labels' column.
	iRow = 0;
	pTop = OMalloc<CGridContainer>(pWindow);
	pTop->SetAlignment(CSH_Stretch, CSV_Top);
	pTop->AddComponent(CreateLabel(mbSave ? "Save In:" : "Look In:", iLabelWidth), 0, 0);
	mpLookIn = OMalloc<CComboBox>(pWindow, psLook->pTheme);
	mpLookIn->SetAlignment(CSH_Stretch, CSV_Center);
	pTop->AddComponent(mpLookIn, 1, 0, 1, 1, 1, 0);
	mpUp = CreateButton("Up");
	mpHome = CreateButton("Home");
	mpNewFolder = CreateButton("New Folder");
	AddButton(pTop, mpUp, 2, FILE_CHOOSER_GAP);
	AddButton(pTop, mpHome, 4, FILE_CHOOSER_GAP / 2);
	AddButton(pTop, mpNewFolder, 6, FILE_CHOOSER_GAP / 2);
	//A grid in a grid is the size it's given, not the size its parts need.
	pTop->SetDesiredSize(-1, (mpLookIn->GetDesiredSize().y > mpUp->GetDesiredSize().y) ? mpLookIn->GetDesiredSize().y : mpUp->GetDesiredSize().y);
	pBody->AddComponent(pTop, 0, iRow, 3, 1, 1, 0);
	iRow++;

	pGap = OMalloc<CFillContainer>(pWindow);
	pGap->SetDesiredSize(FILE_CHOOSER_GAP, FILE_CHOOSER_GAP);
	pBody->AddComponent(pGap, 0, iRow);
	iRow++;

	sInsets.Init(aiInsets);
	mpList = OMalloc<CListContainer>(pWindow, psLook->pTheme);
	mpList->SetHighlightDraws(OMalloc<CRoundedBoxCanvasDraw>(psLook->pTheme->GetListSelectedColour(), 4, &sInsets), OMalloc<CRoundedBoxCanvasDraw>(psLook->pTheme->GetListHoverColour(), 4, &sInsets));
	mpList->SetAlignment(CSH_Stretch, CSV_Top);
	mpScrollPane = OMalloc<CScrollPane>(pWindow, psLook->pTheme);
	mpScrollPane->SetContent(mpList);
	mpScrollPane->SetAlignment(CSH_Stretch, CSV_Stretch);
	pBody->AddComponent(mpScrollPane, 0, iRow, 3, 1, 1, 1);
	iRow++;

	pGap = OMalloc<CFillContainer>(pWindow);
	pGap->SetDesiredSize(FILE_CHOOSER_GAP, FILE_CHOOSER_GAP);
	pBody->AddComponent(pGap, 0, iRow);
	iRow++;

	pBody->AddComponent(CreateLabel("File Name:", iLabelWidth), 0, iRow);
	mpFileName = OMalloc<CTextField>(pWindow, psLook->pTheme, "");
	mpFileName->SetColumns(10);
	mpFileName->SetAlignment(CSH_Stretch, CSV_Center);
	pBody->AddComponent(mpFileName, 1, iRow, 2, 1, 1, 0);
	iRow++;

	pGap = OMalloc<CFillContainer>(pWindow);
	pGap->SetDesiredSize(FILE_CHOOSER_GAP, FILE_CHOOSER_GAP / 2);
	pBody->AddComponent(pGap, 0, iRow);
	iRow++;

	pBody->AddComponent(CreateLabel("Files of Type:", iLabelWidth), 0, iRow);
	mpFileType = OMalloc<CComboBox>(pWindow, psLook->pTheme);
	mpFileType->SetAlignment(CSH_Stretch, CSV_Center);
	pBody->AddComponent(mpFileType, 1, iRow, 2, 1, 1, 0);
	return pBody;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
Ptr<CLabel> CTesterFileChooser::CreateLabel(const char* szText, int32 iWidth)
{
	Ptr<CLabel>		pLabel;

	pLabel = OMalloc<CLabel>(mcDialog.GetWindow(), mcDialog.GetLook()->pTheme->GetFont(), szText);
	pLabel->SetDesiredSize(iWidth, -1);
	pLabel->SetAlignment(CSH_Left, CSV_Center);
	return pLabel;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
Ptr<CButton> CTesterFileChooser::CreateButton(const char* szCaption)
{
	Ptr<CButton>	pButton;

	pButton = OMalloc<CButton>(mcDialog.GetWindow(), mcDialog.GetLook()->pTheme, szCaption);
	pButton->AddListener(&mcButtonListener);
	pButton->SetAlignment(CSH_Center, CSV_Center);
	return pButton;
}


//////////////////////////////////////////////////////////////////////////
//
// The button in iColumn of the row, after a gap in the column before.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::AddButton(Ptr<CGridContainer> pRow, Ptr<CButton> pButton, int iColumn, int32 iGap)
{
	Ptr<CFillContainer>		pGap;

	pGap = OMalloc<CFillContainer>(mcDialog.GetWindow());
	pGap->SetDesiredSize(iGap, 1);
	pRow->AddComponent(pGap, iColumn - 1, 0);
	pRow->AddComponent(pButton, iColumn, 0);
}


//////////////////////////////////////////////////////////////////////////
//
// Shows the folder, the row of szSelect (a whole path) selected if it's
// in it, otherwise none.  Returns false, showing what was shown, if the
// folder doesn't exist.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterFileChooser::Open(const char* szDirectory, const char* szSelect)
{
	CFileUtil	cFileUtil;
	CChars		szPath;
	CChars		szSelectPath;

	//Copied first, either can be one of Look In's or the rows' paths.
	szPath.Init(szDirectory);
	szSelectPath.Init();
	if (szSelect)
	{
		szSelectPath.Append(szSelect);
	}
	NormaliseDirectory(&szPath);
	if (!cFileUtil.IsDirectory(szPath.Text()))
	{
		szSelectPath.Kill();
		szPath.Kill();
		return false;
	}

	mszDirectory.Set(szPath);
	FillLookIn();
	FillList(szSelectPath.Empty() ? NULL : szSelectPath.Text());
	mpScrollPane->SetScrollPosition(0, 0);
	szSelectPath.Kill();
	szPath.Kill();
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
// The root first and the folder shown last, as JFileChooser's combo box
// has them.  Each is its whole path rather than its name indented, which
// reads oddly with the list closed.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::FillLookIn(void)
{
	CFileUtil		cFileUtil;
	CArrayChars		aszUp;  //The folder shown first, the root last.
	CChars			szPath;
	size			i;

	aszUp.Init();
	szPath.Init(mszDirectory);
	for (;;)
	{
		aszUp.Add(szPath.Text());
		if (IsRootDirectory(&szPath))
		{
			break;
		}
		cFileUtil.RemoveLastFromPath(&szPath);
		NormaliseDirectory(&szPath);
	}
	szPath.Kill();

	maszLookIn.ReInit();
	mpLookIn->RemoveAllItems();
	for (i = aszUp.NumElements(); i > 0; i--)
	{
		maszLookIn.Add(aszUp.GetText(i - 1));
		mpLookIn->AddItem(aszUp.GetText(i - 1));
	}
	mpLookIn->Select((int32)maszLookIn.NumElements() - 1);
	aszUp.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// The folder's folders then its files of the type chosen, hidden ones
// left out.  The row of szSelect is selected if it's shown.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::FillList(const char* szSelect)
{
	CFileUtil		cFileUtil;
	CArrayChars		aszFolders;
	CArrayChars		aszFiles;
	size			i;
	int32			iSelect;

	mpList->RemoveListener(&mcListListener);
	mpList->RemoveAllComponents();
	maszRows.ReInit();

	aszFolders.Init();
	aszFiles.Init();
	cFileUtil.FindAllDirectories(mszDirectory.Text(), &aszFolders, false);
	cFileUtil.FindAllFiles(mszDirectory.Text(), &aszFiles, false, false);

	iSelect = -1;
	for (i = 0; i < aszFolders.NumElements(); i++)
	{
		if (szSelect && (StringCompare(aszFolders.GetText(i), szSelect) == 0))
		{
			iSelect = mpList->NumRows();
		}
		AddRow(aszFolders.GetText(i), true);
	}
	miNumFolders = mpList->NumRows();
	for (i = 0; i < aszFiles.NumElements(); i++)
	{
		if (IsShownFile(aszFiles.GetText(i)))
		{
			if (szSelect && (StringCompare(aszFiles.GetText(i), szSelect) == 0))
			{
				iSelect = mpList->NumRows();
			}
			AddRow(aszFiles.GetText(i), false);
		}
	}
	aszFiles.Kill();
	aszFolders.Kill();

	mpList->Select(iSelect);
	mpList->AddListener(&mcListListener);
}


//////////////////////////////////////////////////////////////////////////
//
// An icon and the folder's or file's name.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::AddRow(const char* szPath, bool bFolder)
{
	Ptr<CWindow>			pWindow;
	Ptr<CIconComponent>		pIcon;
	Ptr<CLabel>				pName;
	CFileUtil				cFileUtil;
	CChars					szName;
	int32					iRow;

	pWindow = mcDialog.GetWindow();
	iRow = mpList->AddRow(true);
	pIcon = OMalloc<CIconComponent>(pWindow, bFolder ? mpFolderIcon : mpFileIcon, FILE_CHOOSER_ICON_WIDTH, FILE_CHOOSER_ROW_HEIGHT);
	pIcon->SetCelAlignment(CSH_Center, CSV_Center);
	szName.Init();
	cFileUtil.GetFileName(&szName, (char*)szPath);
	pName = OMalloc<CLabel>(pWindow, mcDialog.GetLook()->pTheme->GetFont(), szName.Text());
	pName->SetAlignment(CSH_Left, CSV_Center);
	szName.Kill();
	mpList->AddToRow(iRow, pIcon, 0);
	mpList->AddToRow(iRow, pName, 1, 1, 1);
	maszRows.Add(szPath);
}


//////////////////////////////////////////////////////////////////////////
//
// Whether the type chosen in Files of Type shows the file.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterFileChooser::IsShownFile(const char* szPath)
{
	const char*	szExtension;

	szExtension = gasFileChooserTypes[mpFileType->GetSelected()].szExtension;
	if (szExtension == NULL)
	{
		return true;
	}
	return HasExtension(szPath, szExtension);
}


//////////////////////////////////////////////////////////////////////////
//
// Returns true if a file was chosen.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterFileChooser::Show(void)
{
	return mcDialog.Show(FILE_CHOOSER_WIDTH, FILE_CHOOSER_HEIGHT);
}


//////////////////////////////////////////////////////////////////////////
//
// The whole path of the file chosen.
//
//////////////////////////////////////////////////////////////////////////
char* CTesterFileChooser::GetChosen(void)
{
	return mszChosen.Text();
}


//////////////////////////////////////////////////////////////////////////
//
// Open (or Save, or enter) chooses the file File Name names.  A folder is
// opened instead, and with File Name empty the selected folder is.  A
// file being opened must exist.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterFileChooser::Approve(void)
{
	CFileUtil	cFileUtil;
	CChars		szPath;
	char*		szText;
	const char*	szExtension;
	int32		iRow;

	szText = mpFileName->GetText();
	if (StrEmpty(szText))
	{
		iRow = mpList->GetSelected();
		if ((iRow >= 0) && (iRow < miNumFolders))
		{
			Open(maszRows.GetText(iRow), NULL);
		}
		return false;
	}

	if (cFileUtil.IsAbsolutePath(szText))
	{
		szPath.Init(szText);
	}
	else
	{
		szPath.Init(mszDirectory);
		cFileUtil.AppendToPath(&szPath, szText);
	}
	cFileUtil.FullPath(&szPath);

	if (cFileUtil.IsDirectory(szPath.Text()))
	{
		Open(szPath.Text(), NULL);
		mpFileName->SetText("");
		szPath.Kill();
		return false;
	}

	if (mbSave)
	{
		szExtension = gasFileChooserTypes[mpFileType->GetSelected()].szExtension;
		if (szExtension && !HasExtension(szPath.Text(), szExtension))
		{
			szPath.Append('.');
			szPath.Append(szExtension);
		}
	}
	else if (!cFileUtil.Exists(szPath.Text()))
	{
		szPath.Kill();
		return false;
	}

	mszChosen.Set(szPath);
	szPath.Kill();
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
// A file selected has its name put in File Name.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::SelectionChanged(void)
{
	CFileUtil	cFileUtil;
	CChars		szName;
	int32		iRow;

	iRow = mpList->GetSelected();
	if ((iRow < miNumFolders) || (iRow >= (int32)maszRows.NumElements()))
	{
		return;
	}
	szName.Init();
	cFileUtil.GetFileName(&szName, maszRows.GetText(iRow));
	mpFileName->SetText(szName.Text());
	szName.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// A double click, or enter, on a folder opens it; on a file it chooses
// it.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::ItemActivated(void)
{
	int32	iRow;

	iRow = mpList->GetSelected();
	if ((iRow < 0) || (iRow >= (int32)maszRows.NumElements()))
	{
		return;
	}
	if (iRow < miNumFolders)
	{
		Open(maszRows.GetText(iRow), NULL);
	}
	else if (Approve())
	{
		mcDialog.Close("OK");
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Look In opens the folder chosen; Files of Type shows its files.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::ComboChanged(CUnknown* pcSource)
{
	int32	iIndex;

	if (&mpLookIn == pcSource)
	{
		iIndex = mpLookIn->GetSelected();
		if ((iIndex >= 0) && (iIndex < (int32)maszLookIn.NumElements()))
		{
			Open(maszLookIn.GetText(iIndex), NULL);
		}
	}
	else if (&mpFileType == pcSource)
	{
		FillList(NULL);
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Up opens the parent folder, the folder come from selected.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::ButtonClicked(CUnknown* pcSource)
{
	CFileUtil	cFileUtil;
	CChars		szParent;

	if (&mpUp == pcSource)
	{
		if (!IsRootDirectory(&mszDirectory))
		{
			szParent.Init(mszDirectory);
			cFileUtil.RemoveLastFromPath(&szParent);
			Open(szParent.Text(), mszDirectory.Text());
			szParent.Kill();
		}
	}
	else if (&mpHome == pcSource)
	{
		GoHome();
	}
	else if (&mpNewFolder == pcSource)
	{
		NewFolder();
	}
}


//////////////////////////////////////////////////////////////////////////
//
// The user's home folder.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::GoHome(void)
{
	const char*	szHome;

#ifdef _MSC_VER
#pragma warning(suppress : 4996)
#endif
#ifdef LINUX_GNU
	szHome = getenv("HOME");
#else
	szHome = getenv("USERPROFILE");
#endif
	if (!StrEmpty(szHome))
	{
		Open(szHome, NULL);
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Asks for the new folder's name and makes it in the folder shown, then
// selects it.
//
//////////////////////////////////////////////////////////////////////////
void CTesterFileChooser::NewFolder(void)
{
	CTesterDialog		cDialog;
	Ptr<CTextField>		pName;
	CFileUtil			cFileUtil;
	CChars				szPath;
	CChars				szDirectory;

	cDialog.Init(mcDialog.GetWindow(), mcDialog.GetLook(), "New Folder");
	pName = cDialog.AddTextField("Name", "New Folder");

	if (cDialog.Show(NEW_FOLDER_DIALOG_WIDTH) && !StrEmpty(pName->GetText()))
	{
		szPath.Init(mszDirectory);
		cFileUtil.AppendToPath(&szPath, pName->GetText());
		if (cFileUtil.MakeDir(szPath.Text()))
		{
			szDirectory.Init(mszDirectory);
			Open(szDirectory.Text(), szPath.Text());
			szDirectory.Kill();
		}
		szPath.Kill();
	}

	pName = NULL;
	cDialog.Kill();
}

