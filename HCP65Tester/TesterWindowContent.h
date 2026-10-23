#ifndef __TESTER_WINDOW_CONTENT_H__
#define __TESTER_WINDOW_CONTENT_H__
#include "BaseLib/ArrayInt.h"
#include "SupportLib/Font.h"
#include "SupportLib/FontColours.h"
#include "WindowLib/Window.h"
#include "WindowLib/GridContainer.h"
#include "WindowLib/ScrollPane.h"
#include "WindowLib/ListContainer.h"
#include "WindowLib/MenuBar.h"
#include "WindowLib/Button.h"
#include "WindowLib/Label.h"
#include "TesterButtonListener.h"
#include "TesterWindowData.h"
#include "TesterDialog.h"
#include "CircuitCanvas.h"


//What a toolbar button or menu item does.  The buttons and menu items are those of Logicim's window.
enum ETesterAction
{
	TA_LoadProject,
	TA_SaveProject,
	TA_NewSimulation,
	TA_ResetSimulation,
	TA_RunSimulation,
	TA_PauseSimulation,
	TA_DefaultSpeed,
	TA_IncreaseSpeed,
	TA_DecreaseSpeed,
	TA_RotateLeft,
	TA_RotateRight,
	TA_FlipHorizontally,
	TA_FlipVertically,
	TA_TogglePointView,
	TA_ZoomIn,
	TA_ZoomOut,
	TA_ResetZoom,
	TA_ZoomFitAll,
	TA_ZoomFitSelection,
	TA_ViewSimulationTree,
	TA_AddComponent,
	TA_AddTrace,
	TA_AddSplitter,
	TA_AddTunnel,
	TA_AddConnection,
	TA_AddPower,
	TA_AddLabel,
	TA_AddSubcircuit,
	TA_NewSubcircuit,
	TA_EditProperties,
	TA_Undo,
	TA_Redo,
	TA_Cut,
	TA_Copy,
	TA_Paste,
	TA_Duplicate,
	TA_Delete,
	TA_Exit,

	NUM_TESTER_ACTIONS
};


//The tester's window, laid out like Logicim's in IntelliJ IDEA's new UI dark style.  From the top: the menu bar, the
//main toolbar (load and save, the simulation's controls, rotate and flip), then the editor between the view stripe on
//the left and the add stripe on the right, then the status bar.  The editor has the subcircuits' tool window on its
//left and the simulations' tool window below its circuit canvas.
//
//The dialogs (a component's properties, a new subcircuit, a new simulation) are native windows shown modally.
//
//The window must already be in the root, see CTesterWindowContent::Init.
class CTesterWindowContent
{
protected:
	Ptr<CWindow>			mpWindow;
	STesterWindowData*		mpsData;
	Ptr<CFontColours>		mpFontColours;
	Ptr<CFont>				mpFont;
	Ptr<CFont>				mpDimFont;
	Ptr<CWindowTheme>		mpTheme;
	STesterDialogLook		msDialogLook;
	CArray<CImageCel>		mapIcons;  //One for each ETesterAction, NULL if the action has no icon.

	Ptr<CMenuBar>			mpMenuBar;
	Ptr<CListContainer>		mpSubcircuits;
	Ptr<CListContainer>		mpSimulations;
	CArray<CLabel>			maSubcircuitLabels;  //The text of each row.
	CArray<CLabel>			maSimulationLabels;
	Ptr<CCircuitCanvas>		mpCircuit;
	Ptr<CLabel>				mpSubcircuitInfo;
	Ptr<CLabel>				mpSimulationInfo;
	Ptr<CLabel>				mpViewPathInfo;
	Ptr<CLabel>				mpMouseXInfo;
	Ptr<CLabel>				mpMouseYInfo;
	Ptr<CLabel>				mpZoomInfo;

	CArray<CButton>			maButtons;
	CArrayInt				maiButtonActions;
	CArray<CMenuItem>		maMenuItems;
	CArrayInt				maiMenuItemActions;

	CTesterButtonListener	mcButtonListener;
	CTesterMenuListener		mcMenuListener;
	CTesterListListener		mcListListener;

	int						miSimulationCount;
	bool					mbRunning;

public:
	bool			Init(Ptr<CWindow> pWindow, STesterWindowData* psData, const char* szFontFilename, const char* szIconDirectory);
	void			Kill(void);

	void			ButtonClicked(CButton* pcButton);
	void			MenuItemClicked(CMenuItem* pcItem);
	void			ListSelectionChanged(CListContainer* pcList);
	void			PointerOverCircuit(int x, int y);
	void			Zoom(float32 fZoom);
	void			ShowDialog(ETesterDialog eDialog);

protected:
	Ptr<CFont>				GenerateFont(const char* szFontFilename, const char* szName, float32 fPixelSize, EColourOrder eOrder);
	bool					LoadIcons(const char* szIconDirectory);
	void					CreateLooks(void);

	Ptr<CComponent>			CreateMenuBar(void);
	Ptr<CMenuContainer>		CreateMenu(ETesterAction* aeActions, int iNumActions);
	Ptr<CComponent>			CreateToolbar(void);
	Ptr<CComponent>			CreateStripe(ETesterAction* aeActions, int iNumActions);
	Ptr<CComponent>			CreateEditor(void);
	Ptr<CComponent>			CreateToolWindow(const char* szTitle, Ptr<CListContainer> pList);
	Ptr<CComponent>			CreateStatusBar(void);

	Ptr<CButton>			CreateToolButton(ETesterAction eAction);
	Ptr<CCanvas>			CreateLine(int32 iWidth, int32 iHeight, ARGB32 sColour);
	Ptr<CListContainer>		CreateList(void);
	void					AddListItem(Ptr<CListContainer> pList, CArray<CLabel>* paLabels, const char* szText);
	char*					GetListItemText(Ptr<CListContainer> pList, CArray<CLabel>* paLabels);
	Ptr<CLabel>				AddStatusLabel(Ptr<CGridContainer> pBar, int* piColumn, int32 iWidth);

	void					DoAction(ETesterAction eAction);
	void					UpdateInfo(void);
	void					ShowPropertiesDialog(void);
	void					ShowNewSubcircuitDialog(void);
	void					ShowNewSimulationDialog(void);
};


#endif // __TESTER_WINDOW_CONTENT_H__

