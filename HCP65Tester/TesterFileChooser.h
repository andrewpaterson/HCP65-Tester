#ifndef __TESTER_FILE_CHOOSER_H__
#define __TESTER_FILE_CHOOSER_H__
#include "BaseLib/ArrayChars.h"
#include "WindowLib/ListContainer.h"
#include "WindowLib/ListListener.h"
#include "WindowLib/ComboBoxListener.h"
#include "WindowLib/ButtonListener.h"
#include "WindowLib/ScrollPane.h"
#include "WindowLib/Label.h"
#include "TesterDialog.h"


class CTesterFileChooser;


//The chooser can't be every kind of listener itself, each is a CListener, so these pass what they hear on.
class CFileChooserListListener : public CListListener
{
CONSTRUCTABLE(CFileChooserListListener);
public:
	CTesterFileChooser*	mpcChooser;

	void	Init(CTesterFileChooser* pcChooser);
	void	SelectionChanged(CUnknown* pcSource, void* pvContext) override;
	void	ItemActivated(CUnknown* pcSource, void* pvContext) override;
};


class CFileChooserComboListener : public CComboBoxListener
{
CONSTRUCTABLE(CFileChooserComboListener);
public:
	CTesterFileChooser*	mpcChooser;

	void	Init(CTesterFileChooser* pcChooser);
	void	SelectionChanged(CUnknown* pcSource, void* pvContext) override;
};


class CFileChooserButtonListener : public CButtonListener
{
CONSTRUCTABLE(CFileChooserButtonListener);
public:
	CTesterFileChooser*	mpcChooser;

	void	Init(CTesterFileChooser* pcChooser);
	void	ButtonClicked(CUnknown* pcSource, void* pvContext) override;
};


//Logicim's load and save dialog, Swing's JFileChooser as its Metal look lays it out: Look In (Save In when saving)
//with the folders from the root down to the one shown, and Up One Level, Home and New Folder buttons, along the top;
//the folder's folders then its files under it; and File Name and Files of Type along the bottom.  Open (or Save)
//and Cancel are the buttons.
//
//Selecting a file puts its name in File Name.  A double click on a folder (or enter) opens it; on a file it chooses
//it.  Open chooses what File Name names, relative to the folder shown unless it's a whole path, but if that's a
//folder it opens it instead, as JFileChooser does.  A file that doesn't exist can't be opened.
//
//Saving with Logicim's files as the type, a name without their extension has it added, as Logicim's save does.
class CTesterFileChooser : public CTesterDialogApprover
{
protected:
	CTesterDialog				mcDialog;
	Ptr<CComboBox>				mpLookIn;
	Ptr<CButton>				mpUp;
	Ptr<CButton>				mpHome;
	Ptr<CButton>				mpNewFolder;
	Ptr<CScrollPane>			mpScrollPane;
	Ptr<CListContainer>			mpList;
	Ptr<CTextField>				mpFileName;
	Ptr<CComboBox>				mpFileType;
	Ptr<CImageCel>				mpFolderIcon;
	Ptr<CImageCel>				mpFileIcon;
	CFileChooserListListener	mcListListener;
	CFileChooserComboListener	mcComboListener;
	CFileChooserButtonListener	mcButtonListener;
	CChars						mszDirectory;  //The folder shown, a whole path.
	CArrayChars					maszLookIn;  //Each of Look In's folders, the root first.
	CArrayChars					maszRows;  //Each row's whole path.
	int32						miNumFolders;  //The rows before the files.
	CChars						mszChosen;
	bool						mbSave;

public:
	void			Init(Ptr<CWindow> pOwner, STesterDialogLook* psLook, const char* szTitle, bool bSave, const char* szFilename, Ptr<CImageCel> pFolderIcon, Ptr<CImageCel> pFileIcon);
	void			Kill(void);

	bool			Show(void);
	char*			GetChosen(void);

	bool			Approve(void) override;
	void			SelectionChanged(void);
	void			ItemActivated(void);
	void			ComboChanged(CUnknown* pcSource);
	void			ButtonClicked(CUnknown* pcSource);

protected:
	Ptr<CComponent>	CreateBody(void);
	Ptr<CLabel>		CreateLabel(const char* szText, int32 iWidth);
	Ptr<CButton>	CreateButton(const char* szCaption);
	void			AddButton(Ptr<CGridContainer> pRow, Ptr<CButton> pButton, int iColumn, int32 iGap);
	bool			Open(const char* szDirectory, const char* szSelect);
	void			FillLookIn(void);
	void			FillList(const char* szSelect);
	void			AddRow(const char* szPath, bool bFolder);
	bool			IsShownFile(const char* szPath);
	void			NewFolder(void);
	void			GoHome(void);
};


void	NormaliseDirectory(CChars* pszDirectory);


#endif // __TESTER_FILE_CHOOSER_H__

