#include "BaseLib/Logger.h"
#include "BaseLib/FileUtil.h"
#include "StandardLib/Objects.h"
#include "SupportLib/ColourARGB32.h"
#include "SupportLib/ImageColour.h"
#include "SupportLib/ImageColourConverter.h"
#include "SupportLib/ImageReader.h"
#include "SupportLib/TrueTypeFont.h"
#include "SupportLib/TrueTypeFontFactory.h"
#include "WindowLib/JewelColours.h"
#include "WindowLib/JewelTheme.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/FillContainer.h"
#include "WindowLib/RoundedBoxCanvasDraw.h"
#include "TesterWindowContent.h"


//IntelliJ IDEA's new UI dark theme.  The dim text is the tester's own, for the status bar and dividers.
#define TESTER_DIM_TEXT			Set32BitColour((uint8)0x9D, (uint8)0xA0, (uint8)0xA8)

#define TESTER_ROW_HEIGHT		24
#define TESTER_DIALOG_WIDTH		440


//The glyphs generated: printable ASCII.
#define TESTER_FONT_CHARACTERS	" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"


struct STesterActionDesc
{
	const char*		szName;
	const char*		szIcon;  //The Logicim icon's name, without _24.png.  NULL for none.
};


//In ETesterAction's order.
static STesterActionDesc gasTesterActions[NUM_TESTER_ACTIONS] =
{
	{ "Load Project", "directory_open" },
	{ "Save Project", "save" },
	{ "New Simulation...", "new_simulation" },
	{ "Reset Simulation", "sim_reset" },
	{ "Run Simulation", "sim_run" },
	{ "Pause Simulation", "sim_stop" },
	{ "Default Speed", "sim_default" },
	{ "Increase Speed", "sim_faster" },
	{ "Decrease Speed", "sim_slower" },
	{ "Rotate Left", "rotate_ccw" },
	{ "Rotate Right", "rotate_cw" },
	{ "Flip Horizontally", "mirror_h" },
	{ "Flip Vertically", "mirror_v" },
	{ "Toggle Point View", "grid" },
	{ "Zoom In", "zoom_in" },
	{ "Zoom Out", "zoom_out" },
	{ "Reset Zoom", "zoom_center_on_screen" },
	{ "Zoom to Fit All", "zoom_fit_in_page" },
	{ "Zoom to Fit Selection", "zoom_selection" },
	{ "Simulation Tree...", "simulation_tree" },
	{ "Add Component...", "add_component" },
	{ "Add Trace", "add_line" },
	{ "Add Splitter", "add_line2bus" },
	{ "Add Tunnel", "add_glabel" },
	{ "Add Connection", "add_junction" },
	{ "Add Power", "add_power" },
	{ "Add Label", "add_label" },
	{ "Add Subcircuit", "module" },
	{ "New Subcircuit...", "new_footprint" },
	{ "Properties...", "edit" },
	{ "Undo", "undo" },
	{ "Redo", "redo" },
	{ "Cut", "cut" },
	{ "Copy", "copy" },
	{ "Paste", NULL },
	{ "Duplicate", "duplicate" },
	{ "Delete", "trash" },
	{ "Exit", "exit" },
};


//A separator in a menu or a toolbar.
#define TA_Separator	NUM_TESTER_ACTIONS


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CTesterWindowContent::Init(Ptr<CWindow> pWindow, STesterWindowData* psData, const char* szFontFilename, const char* szIconDirectory)
{
	CColourFormatHelper		cFormat;
	Ptr<CGridContainer>		pGrid;
	Ptr<CFont>				pWhite;

	mpWindow = pWindow;
	mpsData = psData;
	mpsData->pcContent = this;
	mpsData->eDialog = TD_None;
	miSimulationCount = 0;
	mbRunning = false;
	maButtons.Init();
	maiButtonActions.Init();
	maMenuItems.Init();
	maiMenuItemActions.Init();
	mapIcons.Init();
	maSubcircuitLabels.Init();
	maSimulationLabels.Init();
	mcButtonListener.Init(this);
	mcMenuListener.Init(this);
	mcListListener.Init(this);

	//The font is white and coloured as it is drawn.
	pWindow->GetColourFormat(&cFormat);
	pWhite = GenerateFont(szFontFilename, "Inter 13px", 13.0f, cFormat.meOrder);
	if (pWhite.IsNull())
	{
		return false;
	}
	//Nothing in the window holds the font colours or the theme, the widgets hold their parts, so they are rooted.
	mpFontColours = OMalloc<CFontColours>(pWhite);
	ORoot()->Add(mpFontColours);
	mpFont = mpFontColours->GetFont(JEWEL_TEXT);
	mpDimFont = mpFontColours->GetFont(TESTER_DIM_TEXT);

	if (!LoadIcons(szIconDirectory))
	{
		return false;
	}
	CreateLooks();

	pGrid = OMalloc<CGridContainer>(pWindow);
	pWindow->SetContainer(pGrid);
	pGrid->AddComponent(CreateMenuBar(), 0, 0, 1, 1, 1, 0);
	pGrid->AddComponent(CreateToolbar(), 0, 1, 1, 1, 1, 0);
	pGrid->AddComponent(CreateLine(-1, 1, JEWEL_EDITOR), 0, 2, 1, 1, 1, 0);
	pGrid->AddComponent(CreateEditor(), 0, 3, 1, 1, 1, 1);
	pGrid->AddComponent(CreateLine(-1, 1, JEWEL_EDITOR), 0, 4, 1, 1, 1, 0);
	pGrid->AddComponent(CreateStatusBar(), 0, 5, 1, 1, 1, 0);

	//Like Logicim the project starts with a top level subcircuit and a simulation of it.
	AddListItem(mpSubcircuits, &maSubcircuitLabels, "Top");
	AddListItem(mpSubcircuits, &maSubcircuitLabels, "Address Decode");
	AddListItem(mpSubcircuits, &maSubcircuitLabels, "Board Pins");
	mpSubcircuits->Select(0);
	AddListItem(mpSimulations, &maSimulationLabels, "Top Top 1");
	miSimulationCount = 1;
	mpSimulations->Select(0);
	UpdateInfo();
	Zoom(1.0f);

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::Kill(void)
{
	size	i;

	for (i = 0; i < maButtons.Size(); i++)
	{
		maButtons.Get(i)->RemoveListener(&mcButtonListener);
	}
	if (mpMenuBar.IsNotNull())
	{
		mpMenuBar->RemoveListener(&mcMenuListener);
	}
	if (mpSubcircuits.IsNotNull())
	{
		mpSubcircuits->RemoveListener(&mcListListener);
		mpSimulations->RemoveListener(&mcListListener);
	}
	if (mpsData)
	{
		mpsData->pcContent = NULL;
	}

	maButtons.Kill();
	maiButtonActions.Kill();
	maMenuItems.Kill();
	maiMenuItemActions.Kill();
	mapIcons.Kill();
	maSubcircuitLabels.Kill();
	maSimulationLabels.Kill();
	msDialogLook.Kill();
	if (mpTheme.IsNotNull())
	{
		ORoot()->Remove(mpTheme);
	}
	mpTheme = NULL;

	mpMenuBar = NULL;
	mpSubcircuits = NULL;
	mpSimulations = NULL;
	mpCircuit = NULL;
	mpSubcircuitInfo = NULL;
	mpSimulationInfo = NULL;
	mpViewPathInfo = NULL;
	mpMouseXInfo = NULL;
	mpMouseYInfo = NULL;
	mpZoomInfo = NULL;
	mpFont = NULL;
	mpDimFont = NULL;
	if (mpFontColours.IsNotNull())
	{
		ORoot()->Remove(mpFontColours);
	}
	mpFontColours = NULL;
	mpWindow = NULL;
}


//////////////////////////////////////////////////////////////////////////
//
// The icons converted from Logicim's by Tools/ConvertLogicimIcons.py, in
// the window's colour order.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterWindowContent::LoadIcons(const char* szIconDirectory)
{
	CFileUtil			cFileUtil;
	CChars				szFilename;
	Ptr<CImage>			pImage;
	int					i;
	bool				bResult;

	bResult = true;
	for (i = 0; i < NUM_TESTER_ACTIONS; i++)
	{
		if (gasTesterActions[i].szIcon == NULL)
		{
			mapIcons.Add(NULL);
			continue;
		}

		szFilename.Init(szIconDirectory);
		cFileUtil.AppendToPath(&szFilename, gasTesterActions[i].szIcon);
		szFilename.Append("_24.png");
		pImage = ReadImage(szFilename.Text());
		if (pImage.IsNull())
		{
			gcLogger.Error2(__METHOD__, " Could not read icon [", szFilename.Text(), "].", NULL);
			bResult = false;
			mapIcons.Add(NULL);
		}
		else
		{
			pImage = CImageColourConverter::ConvertColour(pImage, mpWindow->GetColourOrder());
			mapIcons.Add(OMalloc<CImageCel>(pImage));
		}
		szFilename.Kill();
	}
	return bResult;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::CreateLooks(void)
{
	//The dialogs share the window's theme, they have the same colour format.
	mpTheme = CJewelTheme::CreateDarkTheme(mpWindow, mpFont);
	ORoot()->Add(mpTheme);
	msDialogLook.pTheme = mpTheme;
	msDialogLook.pDimFont = mpDimFont;
	msDialogLook.sBackground = JEWEL_PANEL;
	msDialogLook.sDivider = JEWEL_SEPARATOR;
}


//////////////////////////////////////////////////////////////////////////
//
// The actions are the menu's items in order, TA_Separator between groups.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CMenuContainer> CTesterWindowContent::CreateMenu(ETesterAction* aeActions, int iNumActions)
{
	Ptr<CMenuContainer>		pMenu;
	Ptr<CMenuItem>			pItem;
	int						i;

	pMenu = OMalloc<CMenuContainer>(mpWindow, mpTheme);
	for (i = 0; i < iNumActions; i++)
	{
		if (aeActions[i] == TA_Separator)
		{
			pMenu->AddSeparator();
		}
		else
		{
			pItem = pMenu->AddItem(gasTesterActions[aeActions[i]].szName, mapIcons.Get(aeActions[i]));
			maMenuItems.Add(pItem);
			maiMenuItemActions.Add(aeActions[i]);
		}
	}
	return pMenu;
}


//////////////////////////////////////////////////////////////////////////
//
// Logicim's menu bar is only a placeholder; these menus hold its actions.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CComponent> CTesterWindowContent::CreateMenuBar(void)
{
	ETesterAction	aeFile[] = { TA_LoadProject, TA_SaveProject, TA_Separator, TA_Exit };
	ETesterAction	aeEdit[] = { TA_Undo, TA_Redo, TA_Separator, TA_Cut, TA_Copy, TA_Paste, TA_Duplicate, TA_Delete, TA_Separator, TA_RotateLeft, TA_RotateRight, TA_FlipHorizontally, TA_FlipVertically, TA_Separator, TA_EditProperties };
	ETesterAction	aeView[] = { TA_TogglePointView, TA_Separator, TA_ZoomIn, TA_ZoomOut, TA_ResetZoom, TA_ZoomFitAll, TA_ZoomFitSelection, TA_Separator, TA_ViewSimulationTree };
	ETesterAction	aeSimulation[] = { TA_NewSimulation, TA_Separator, TA_ResetSimulation, TA_RunSimulation, TA_PauseSimulation, TA_Separator, TA_DefaultSpeed, TA_IncreaseSpeed, TA_DecreaseSpeed };
	ETesterAction	aePlace[] = { TA_AddComponent, TA_AddTrace, TA_AddSplitter, TA_AddTunnel, TA_AddConnection, TA_AddPower, TA_AddLabel, TA_AddSubcircuit, TA_Separator, TA_NewSubcircuit };

	mpMenuBar = OMalloc<CMenuBar>(mpWindow, mpTheme);
	mpMenuBar->AddMenu("File", CreateMenu(aeFile, sizeof(aeFile) / sizeof(ETesterAction)));
	mpMenuBar->AddMenu("Edit", CreateMenu(aeEdit, sizeof(aeEdit) / sizeof(ETesterAction)));
	mpMenuBar->AddMenu("View", CreateMenu(aeView, sizeof(aeView) / sizeof(ETesterAction)));
	mpMenuBar->AddMenu("Simulation", CreateMenu(aeSimulation, sizeof(aeSimulation) / sizeof(ETesterAction)));
	mpMenuBar->AddMenu("Place", CreateMenu(aePlace, sizeof(aePlace) / sizeof(ETesterAction)));
	mpMenuBar->AddListener(&mcMenuListener);
	mpMenuBar->SetAlignment(CSH_Stretch, CSV_Stretch);
	return mpMenuBar;
}


//////////////////////////////////////////////////////////////////////////
//
// A flat button showing the action's icon, highlighted under the pointer.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CButton> CTesterWindowContent::CreateToolButton(ETesterAction eAction)
{
	Ptr<CButton>	pButton;

	pButton = OMalloc<CButton>(mpWindow, mpTheme, BL_Tool, mapIcons.Get(eAction), (const char*)NULL);
	pButton->SetPadding(0, 0);
	pButton->SetShowsPushed(true);
	pButton->AddListener(&mcButtonListener);
	pButton->SetAlignment(CSH_Center, CSV_Center);
	maButtons.Add(pButton);
	maiButtonActions.Add(eAction);
	return pButton;
}


//////////////////////////////////////////////////////////////////////////
//
// A line iWidth by iHeight, -1 to stretch along an axis.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CCanvas> CTesterWindowContent::CreateLine(int32 iWidth, int32 iHeight, ARGB32 sColour)
{
	Ptr<CCanvas>	pLine;

	pLine = OMalloc<CCanvas>(mpWindow, OMalloc<CClearCanvasDraw>(sColour));
	pLine->SetDesiredSize(iWidth, iHeight);
	pLine->SetAlignment(iWidth < 0 ? CSH_Stretch : CSH_Center, iHeight < 0 ? CSV_Stretch : CSV_Center);
	return pLine;
}


//////////////////////////////////////////////////////////////////////////
//
// Logicim's toolbar: load and save, the simulation, then rotate and flip.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CComponent> CTesterWindowContent::CreateToolbar(void)
{
	ETesterAction			aeActions[] = { TA_LoadProject, TA_SaveProject, TA_Separator,
											TA_NewSimulation, TA_ResetSimulation, TA_RunSimulation, TA_PauseSimulation, TA_DefaultSpeed, TA_IncreaseSpeed, TA_DecreaseSpeed, TA_Separator,
											TA_RotateLeft, TA_RotateRight, TA_FlipHorizontally, TA_FlipVertically };
	Ptr<CGridContainer>		pBar;
	Ptr<CFillContainer>		pFiller;
	Ptr<CCanvas>			pLine;
	int						i;
	int						iColumn;

	//A grid only has a size if it is given one.
	pBar = OMalloc<CGridContainer>(mpWindow);
	pBar->SetAlignment(CSH_Stretch, CSV_Stretch);
	pBar->SetDesiredSize(-1, 40);

	//A little space before the first button.
	pFiller = OMalloc<CFillContainer>(mpWindow);
	pFiller->SetDesiredSize(4, 1);
	pBar->AddComponent(pFiller, 0, 0);
	iColumn = 1;
	for (i = 0; i < (int)(sizeof(aeActions) / sizeof(ETesterAction)); i++)
	{
		if (aeActions[i] == TA_Separator)
		{
			pLine = CreateLine(1, 22, JEWEL_SEPARATOR);
			pFiller = OMalloc<CFillContainer>(mpWindow);
			pFiller->SetDesiredSize(13, 22);
			pFiller->AddComponent(pLine);
			pBar->AddComponent(pFiller, iColumn, 0);
		}
		else
		{
			pBar->AddComponent(CreateToolButton(aeActions[i]), iColumn, 0);
		}
		iColumn++;
	}

	pFiller = OMalloc<CFillContainer>(mpWindow);
	pBar->AddComponent(pFiller, iColumn, 0, 1, 1, 1, 0);
	return pBar;
}


//////////////////////////////////////////////////////////////////////////
//
// A column of tool buttons like IntelliJ's tool window stripes; Logicim's
// display and creation panels.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CComponent> CTesterWindowContent::CreateStripe(ETesterAction* aeActions, int iNumActions)
{
	Ptr<CGridContainer>		pStripe;
	Ptr<CFillContainer>		pFiller;
	Ptr<CCanvas>			pLine;
	int						i;
	int						iRow;

	pStripe = OMalloc<CGridContainer>(mpWindow);
	pStripe->SetAlignment(CSH_Stretch, CSV_Stretch);
	pStripe->SetDesiredSize(40, -1);

	pFiller = OMalloc<CFillContainer>(mpWindow);
	pFiller->SetDesiredSize(1, 4);
	pStripe->AddComponent(pFiller, 0, 0);
	iRow = 1;
	for (i = 0; i < iNumActions; i++)
	{
		if (aeActions[i] == TA_Separator)
		{
			pLine = CreateLine(22, 1, JEWEL_SEPARATOR);
			pFiller = OMalloc<CFillContainer>(mpWindow);
			pFiller->SetDesiredSize(22, 13);
			pFiller->AddComponent(pLine);
			pStripe->AddComponent(pFiller, 0, iRow);
		}
		else
		{
			pStripe->AddComponent(CreateToolButton(aeActions[i]), 0, iRow);
		}
		iRow++;
	}

	pFiller = OMalloc<CFillContainer>(mpWindow);
	pStripe->AddComponent(pFiller, 0, iRow, 1, 1, 0, 1);
	return pStripe;
}


//////////////////////////////////////////////////////////////////////////
//
// A list whose rows are highlighted with rounded blocks, like IntelliJ's
// trees.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CListContainer> CTesterWindowContent::CreateList(void)
{
	Ptr<CListContainer>		pList;
	SInsets					sInsets;
	int						aiInsets[4] = { 0, 6, 0, 6 };

	sInsets.Init(aiInsets);
	pList = OMalloc<CListContainer>(mpWindow, mpTheme);
	pList->SetHighlightDraws(OMalloc<CRoundedBoxCanvasDraw>(mpTheme->GetListSelectedColour(), 4, &sInsets), OMalloc<CRoundedBoxCanvasDraw>(mpTheme->GetListHoverColour(), 4, &sInsets));
	pList->SetAlignment(CSH_Stretch, CSV_Top);
	pList->AddListener(&mcListListener);
	return pList;
}


//////////////////////////////////////////////////////////////////////////
//
// A row is a gap and the text, a row high.
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::AddListItem(Ptr<CListContainer> pList, CArray<CLabel>* paLabels, const char* szText)
{
	Ptr<CFillContainer>		pGap;
	Ptr<CLabel>				pLabel;
	int32					iRow;

	iRow = pList->AddRow(true);
	pGap = OMalloc<CFillContainer>(mpWindow);
	pGap->SetDesiredSize(16, TESTER_ROW_HEIGHT);
	pLabel = OMalloc<CLabel>(mpWindow, mpFont, szText);
	pLabel->SetAlignment(CSH_Left, CSV_Center);
	pList->AddToRow(iRow, pGap, 0);
	pList->AddToRow(iRow, pLabel, 1, 1, 1);
	paLabels->Add(pLabel);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
char* CTesterWindowContent::GetListItemText(Ptr<CListContainer> pList, CArray<CLabel>* paLabels)
{
	int32	iRow;

	iRow = pList->GetSelected();
	if ((iRow < 0) || ((size)iRow >= paLabels->Size()))
	{
		return "";
	}
	return paLabels->Get(iRow)->GetText();
}


//////////////////////////////////////////////////////////////////////////
//
// A tool window: its title at the top then its list, scrolled.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CComponent> CTesterWindowContent::CreateToolWindow(const char* szTitle, Ptr<CListContainer> pList)
{
	Ptr<CGridContainer>		pToolWindow;
	Ptr<CFillContainer>		pGap;
	Ptr<CLabel>				pTitle;
	Ptr<CScrollPane>		pScrollPane;

	pToolWindow = OMalloc<CGridContainer>(mpWindow);
	pToolWindow->SetAlignment(CSH_Stretch, CSV_Stretch);

	pGap = OMalloc<CFillContainer>(mpWindow);
	pGap->SetDesiredSize(12, 32);
	pTitle = OMalloc<CLabel>(mpWindow, mpFont, szTitle);
	pTitle->SetAlignment(CSH_Left, CSV_Center);
	pToolWindow->AddComponent(pGap, 0, 0);
	pToolWindow->AddComponent(pTitle, 1, 0, 1, 1, 1, 0);

	pScrollPane = OMalloc<CScrollPane>(mpWindow, mpTheme);
	pScrollPane->SetPolicy(SBO_Horizontal, SPP_Never);
	pScrollPane->SetContent(pList);
	pScrollPane->SetAlignment(CSH_Stretch, CSV_Stretch);
	pToolWindow->AddComponent(pScrollPane, 0, 1, 2, 1, 1, 1);
	return pToolWindow;
}


//////////////////////////////////////////////////////////////////////////
//
// The view stripe, the subcircuits' tool window, the circuit with the
// simulations' tool window below it, and the add stripe.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CComponent> CTesterWindowContent::CreateEditor(void)
{
	ETesterAction			aeDisplay[] = { TA_TogglePointView, TA_Separator, TA_ZoomIn, TA_ZoomOut, TA_ResetZoom, TA_ZoomFitAll, TA_ZoomFitSelection, TA_Separator, TA_ViewSimulationTree };
	ETesterAction			aeCreation[] = { TA_AddComponent, TA_AddTrace, TA_AddSplitter, TA_AddTunnel, TA_AddConnection, TA_AddPower, TA_AddLabel, TA_AddSubcircuit, TA_Separator, TA_NewSubcircuit };
	Ptr<CGridContainer>		pEditor;
	Ptr<CGridContainer>		pCentre;
	Ptr<CComponent>			pSubcircuits;
	Ptr<CComponent>			pSimulations;

	pEditor = OMalloc<CGridContainer>(mpWindow);
	pEditor->SetAlignment(CSH_Stretch, CSV_Stretch);

	mpSubcircuits = CreateList();
	pSubcircuits = CreateToolWindow("Subcircuits", mpSubcircuits);
	pSubcircuits->SetDesiredSize(200, -1);

	mpCircuit = OMalloc<CCircuitCanvas>(mpWindow, OMalloc<CCircuitCanvasDraw>(JEWEL_EDITOR, JEWEL_BORDER, JEWEL_BORDER_HOVER), this);
	mpCircuit->SetAlignment(CSH_Stretch, CSV_Stretch);
	mpSimulations = CreateList();
	pSimulations = CreateToolWindow("Simulations", mpSimulations);
	pSimulations->SetDesiredSize(-1, 150);

	pCentre = OMalloc<CGridContainer>(mpWindow);
	pCentre->SetAlignment(CSH_Stretch, CSV_Stretch);
	pCentre->AddComponent(mpCircuit, 0, 0, 1, 1, 1, 1);
	pCentre->AddComponent(CreateLine(-1, 1, JEWEL_EDITOR), 0, 1, 1, 1, 1, 0);
	pCentre->AddComponent(pSimulations, 0, 2, 1, 1, 1, 0);

	pEditor->AddComponent(CreateStripe(aeDisplay, sizeof(aeDisplay) / sizeof(ETesterAction)), 0, 0, 1, 1, 0, 1);
	pEditor->AddComponent(CreateLine(1, -1, JEWEL_EDITOR), 1, 0, 1, 1, 0, 1);
	pEditor->AddComponent(pSubcircuits, 2, 0, 1, 1, 0, 1);
	pEditor->AddComponent(CreateLine(1, -1, JEWEL_EDITOR), 3, 0, 1, 1, 0, 1);
	pEditor->AddComponent(pCentre, 4, 0, 1, 1, 1, 1);
	pEditor->AddComponent(CreateLine(1, -1, JEWEL_EDITOR), 5, 0, 1, 1, 0, 1);
	pEditor->AddComponent(CreateStripe(aeCreation, sizeof(aeCreation) / sizeof(ETesterAction)), 6, 0, 1, 1, 0, 1);
	return pEditor;
}


//////////////////////////////////////////////////////////////////////////
//
// A label iWidth wide followed by a separator.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CLabel> CTesterWindowContent::AddStatusLabel(Ptr<CGridContainer> pBar, int* piColumn, int32 iWidth)
{
	Ptr<CLabel>			pLabel;
	Ptr<CFillContainer>	pGap;

	pGap = OMalloc<CFillContainer>(mpWindow);
	pGap->SetDesiredSize(10, 26);
	pBar->AddComponent(pGap, *piColumn, 0);
	(*piColumn)++;

	pLabel = OMalloc<CLabel>(mpWindow, mpDimFont, "");
	pLabel->SetDesiredSize(iWidth, -1);
	pLabel->SetAlignment(CSH_Left, CSV_Center);
	pBar->AddComponent(pLabel, *piColumn, 0);
	(*piColumn)++;

	pBar->AddComponent(CreateLine(1, 16, JEWEL_SEPARATOR), *piColumn, 0);
	(*piColumn)++;
	return pLabel;
}


//////////////////////////////////////////////////////////////////////////
//
// Logicim's circuit information: the subcircuit, the simulation and the
// view path on the left, the pointer and zoom on the right.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CComponent> CTesterWindowContent::CreateStatusBar(void)
{
	Ptr<CGridContainer>		pBar;
	Ptr<CFillContainer>		pFiller;
	int						iColumn;

	pBar = OMalloc<CGridContainer>(mpWindow);
	pBar->SetAlignment(CSH_Stretch, CSV_Stretch);
	pBar->SetDesiredSize(-1, 26);

	iColumn = 0;
	mpSubcircuitInfo = AddStatusLabel(pBar, &iColumn, 190);
	mpSimulationInfo = AddStatusLabel(pBar, &iColumn, 250);
	mpViewPathInfo = AddStatusLabel(pBar, &iColumn, 250);

	pFiller = OMalloc<CFillContainer>(mpWindow);
	pBar->AddComponent(pFiller, iColumn, 0, 1, 1, 1, 0);
	iColumn++;

	mpMouseXInfo = AddStatusLabel(pBar, &iColumn, 56);
	mpMouseYInfo = AddStatusLabel(pBar, &iColumn, 56);
	mpZoomInfo = AddStatusLabel(pBar, &iColumn, 44);
	return pBar;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::UpdateInfo(void)
{
	CChars	sz;

	sz.Init("Subcircuit: ");
	sz.Append(GetListItemText(mpSubcircuits, &maSubcircuitLabels));
	mpSubcircuitInfo->SetText(sz.Text());
	sz.Kill();

	sz.Init("Simulation: ");
	sz.Append(GetListItemText(mpSimulations, &maSimulationLabels));
	mpSimulationInfo->SetText(sz.Text());
	sz.Kill();

	sz.Init("View Path: ");
	sz.Append(GetListItemText(mpSubcircuits, &maSubcircuitLabels));
	if (mbRunning)
	{
		sz.Append(" (running)");
	}
	mpViewPathInfo->SetText(sz.Text());
	sz.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// Appends f to one decimal place, like Logicim's "%.1f".
//
//////////////////////////////////////////////////////////////////////////
void AppendTenths(CChars* psz, float32 f)
{
	int32	iTenths;

	if (f < 0.0f)
	{
		iTenths = (int32)(-f * 10.0f + 0.5f);
		if (iTenths != 0)
		{
			psz->Append("-");
		}
	}
	else
	{
		iTenths = (int32)(f * 10.0f + 0.5f);
	}
	psz->Append(iTenths / 10);
	psz->Append(".");
	psz->Append(iTenths % 10);
}


//////////////////////////////////////////////////////////////////////////
//
// x and y are relative to the circuit's top left.  Shown in grid units
// from the centre, as Logicim does.
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::PointerOverCircuit(int x, int y)
{
	CChars		sz;
	SInt32Vec2	sSize;
	float32		fSpacing;

	sSize = mpCircuit->GetActualSize();
	fSpacing = mpCircuit->GetCircuitDraw()->GetGridSpacing();

	sz.Init("X ");
	AppendTenths(&sz, (x - sSize.x / 2) / fSpacing);
	mpMouseXInfo->SetText(sz.Text());
	sz.Kill();

	sz.Init("Y ");
	AppendTenths(&sz, (y - sSize.y / 2) / fSpacing);
	mpMouseYInfo->SetText(sz.Text());
	sz.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::Zoom(float32 fZoom)
{
	CChars	sz;

	mpCircuit->GetCircuitDraw()->SetZoom(fZoom);
	sz.Init("Z ");
	AppendTenths(&sz, mpCircuit->GetCircuitDraw()->GetZoom());
	mpZoomInfo->SetText(sz.Text());
	sz.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::ButtonClicked(CButton* pcButton)
{
	size	i;

	for (i = 0; i < maButtons.Size(); i++)
	{
		if (&maButtons.Get(i) == pcButton)
		{
			DoAction((ETesterAction)maiButtonActions.GetValue(i));
			return;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::MenuItemClicked(CMenuItem* pcItem)
{
	size	i;

	for (i = 0; i < maMenuItems.Size(); i++)
	{
		if (&maMenuItems.Get(i) == pcItem)
		{
			DoAction((ETesterAction)maiMenuItemActions.GetValue(i));
			return;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::ListSelectionChanged(CListContainer* pcList)
{
	UpdateInfo();
}


//////////////////////////////////////////////////////////////////////////
//
// The view and dialog actions work, the rest are logged.  A dialog is
// shown by the window's next tick.
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::DoAction(ETesterAction eAction)
{
	Ptr<CCircuitCanvasDraw>	pDraw;

	pDraw = mpCircuit->GetCircuitDraw();
	switch (eAction)
	{
	case TA_NewSimulation:
		mpsData->eDialog = TD_NewSimulation;
		break;
	case TA_NewSubcircuit:
		mpsData->eDialog = TD_NewSubcircuit;
		break;
	case TA_AddComponent:
	case TA_EditProperties:
		mpsData->eDialog = TD_Properties;
		break;
	case TA_RunSimulation:
		mbRunning = true;
		UpdateInfo();
		break;
	case TA_PauseSimulation:
		mbRunning = false;
		UpdateInfo();
		break;
	case TA_TogglePointView:
		pDraw->SetGrid(!pDraw->IsGrid());
		break;
	case TA_ZoomIn:
		Zoom(pDraw->GetZoom() + 1.0f);
		break;
	case TA_ZoomOut:
		Zoom(pDraw->GetZoom() - 1.0f);
		break;
	case TA_ResetZoom:
		Zoom(1.0f);
		break;
	case TA_Exit:
		mpWindow->Stop();
		break;
	default:
		gcLogger.Info2(gasTesterActions[eAction].szName, " isn't done yet.", NULL);
		break;
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::ShowDialog(ETesterDialog eDialog)
{
	if (eDialog == TD_Properties)
	{
		ShowPropertiesDialog();
	}
	else if (eDialog == TD_NewSubcircuit)
	{
		ShowNewSubcircuitDialog();
	}
	else if (eDialog == TD_NewSimulation)
	{
		ShowNewSimulationDialog();
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Logicim's property dialog for a pin, its fields made by its reflective
// property editor: text for strings and numbers, a check box for a bool,
// a combo box for an enum, and its dividers.
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::ShowPropertiesDialog(void)
{
	CTesterDialog		cDialog;
	const char*			aszRotations[] = { "North", "East", "South", "West" };
	const char*			aszFamilies[] = { "74HC", "74HCT", "74LS", "CMOS 5V" };
	const char*			aszAlignments[] = { "Left", "Right", "Top", "Bottom", "Not Shown" };
	const char*			aszAnchours[] = { "Negative", "Center", "Positive" };
	const char*			aszRadices[] = { "Binary", "Decimal", "Hexadecimal" };
	Ptr<CTextField>		pName;
	Ptr<CTextField>		pBitWidth;

	cDialog.Init(mpWindow, &msDialogLook, "Pin Properties");
	cDialog.AddComboBox("Rotation", aszRotations, 4, 1);
	pName = cDialog.AddTextField("Name", "D0");
	pBitWidth = cDialog.AddTextField("Bit Width", "1");
	cDialog.AddComboBox("Family", aszFamilies, 4, 0);
	cDialog.AddCheckBox("Explicit Power Ports", false);
	cDialog.AddDivider("Instance Location");
	cDialog.AddComboBox("Alignment", aszAlignments, 5, 0);
	cDialog.AddComboBox("Anchour", aszAnchours, 3, 1);
	cDialog.AddTextField("Weight", "0");
	cDialog.AddDivider("Instance Display");
	cDialog.AddCheckBox("Inverting", false);
	cDialog.AddCheckBox("Clock Notch", false);
	cDialog.AddComboBox("Radix", aszRadices, 3, 2);
	cDialog.AddExtraButton("Save Defaults");

	cDialog.Show(TESTER_DIALOG_WIDTH);
	gcLogger.Info2("Pin Properties closed by [", cDialog.GetClicked(), "] with name [", pName->GetText(), "] and bit width [", pBitWidth->GetText(), "].", NULL);

	pName = NULL;
	pBitWidth = NULL;
	cDialog.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::ShowNewSubcircuitDialog(void)
{
	CTesterDialog		cDialog;
	Ptr<CTextField>		pName;
	char*				szName;

	cDialog.Init(mpWindow, &msDialogLook, "New Subcircuit");
	pName = cDialog.AddTextField("Name", "");

	if (cDialog.Show(TESTER_DIALOG_WIDTH))
	{
		szName = pName->GetText();
		if (szName[0] != '\0')
		{
			AddListItem(mpSubcircuits, &maSubcircuitLabels, szName);
			mpSubcircuits->Select(mpSubcircuits->NumRows() - 1);
			UpdateInfo();
		}
	}

	pName = NULL;
	cDialog.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// Named like Logicim's: the top subcircuit and a count.
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::ShowNewSimulationDialog(void)
{
	CTesterDialog		cDialog;
	Ptr<CTextField>		pName;
	CChars				szDefault;
	char*				szName;

	szDefault.Init("Top ");
	szDefault.Append(GetListItemText(mpSubcircuits, &maSubcircuitLabels));
	szDefault.Append(" ");
	szDefault.Append(miSimulationCount + 1);

	cDialog.Init(mpWindow, &msDialogLook, "New Simulation");
	pName = cDialog.AddTextField("Name", szDefault.Text());
	szDefault.Kill();

	if (cDialog.Show(TESTER_DIALOG_WIDTH))
	{
		szName = pName->GetText();
		if (szName[0] != '\0')
		{
			miSimulationCount++;
			AddListItem(mpSimulations, &maSimulationLabels, szName);
			mpSimulations->Select(mpSimulations->NumRows() - 1);
			UpdateInfo();
		}
	}

	pName = NULL;
	cDialog.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
// Rasterises the glyphs from the TrueType font in white into an image in
// the window's colour order and makes a font from them.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CFont> CTesterWindowContent::GenerateFont(const char* szFontFilename, const char* szName, float32 fPixelSize, EColourOrder eOrder)
{
	CTrueTypeFont			cTrueTypeFont;
	CTrueTypeFontFactory	cFactory;
	CImageColourRGB			cColour;
	CChars					szCharacters;
	Ptr<CFontDescription>	pDescription;
	Ptr<CImage>				pImage;
	Ptr<CFont>				pFont;

	cTrueTypeFont.Init();
	if (!cTrueTypeFont.Load((char*)szFontFilename))
	{
		cTrueTypeFont.Kill();
		gcLogger.Error2(__METHOD__, " Could not load font [", szFontFilename, "].", NULL);
		return NULL;
	}

	cColour.Init(1.0f, 1.0f, 1.0f);
	szCharacters.Init(TESTER_FONT_CHARACTERS);
	cFactory.Init(&cTrueTypeFont, fPixelSize);
	pDescription = cFactory.Describe((char*)szName, szCharacters.Text());
	pImage = cFactory.Rasterise(pDescription, &cColour);
	if (pImage.IsNotNull())
	{
		if (pImage->GetColourOrder() != eOrder)
		{
			pImage = CImageColourConverter::ConvertColour(pImage, eOrder);
		}
		pFont = OMalloc<CFont>(pDescription, pImage);
	}
	cFactory.Kill();
	szCharacters.Kill();
	cTrueTypeFont.Kill();

	if (pFont.IsNull())
	{
		gcLogger.Error2(__METHOD__, " Could not make a font from [", szFontFilename, "].", NULL);
	}
	return pFont;
}

