#include <stdlib.h>
#include "BaseLib/Timer.h"
#include "StandardLib/ClassDefines.h"
#include "WindowLib/Window.h"
#include "WindowLib/ComponentInput.h"
#include "LogicimLib/CircuitEditor.h"
#include "TesterWindowContent.h"
#include "CircuitCanvas.h"


#define CIRCUIT_CANVAS_DOUBLE_CLICK_MILLISECONDS	500  //As Logicim's when the desktop doesn't say.
#define CIRCUIT_CANVAS_DOUBLE_CLICK_PIXELS			4


//////////////////////////////////////////////////////////////////////////
//
// The canvas takes focus when clicked, so the keys come to it.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::Init(Ptr<CWindow> pWindow, Ptr<CCircuitCanvasDraw> pDraw, CTesterWindowContent* pcContent, CCircuitEditor* pcEditor)
{
	PreInit();
	CCanvas::Init(pWindow, pDraw);
	mpCircuitDraw = pDraw;
	mpcContent = pcContent;
	mpcEditor = pcEditor;
	msScroll.Init(0, 0);
	miLastRelease = 0;
	msLastRelease.Init(0, 0);
	SetCanGetFocus(true);
	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::Class(void)
{
	CCanvas::Class();
	M_Pointer(mpCircuitDraw);
	U_Pointer(mpcContent);
	U_Pointer(mpcEditor);
	U_Data(SInt32Vec2, msScroll);
	U_Int64(miLastRelease);
	U_Data(SInt32Vec2, msLastRelease);
}


//////////////////////////////////////////////////////////////////////////
//
// x and y are relative to the canvas' top left.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::MovedOver(int x, int y)
{
	mpcContent->PointerOverCircuit(x, y);
	UpdateModifiers();
	mpcEditor->PointerMoved(ToGrid(x, y));
}


//////////////////////////////////////////////////////////////////////////
//
// Each notch zooms by a tenth, as Logicim's does.
//
//////////////////////////////////////////////////////////////////////////
bool CCircuitCanvas::Wheel(int iRight, int iDown)
{
	if (iDown != 0)
	{
		mpcContent->Zoom(mpCircuitDraw->GetZoom() - iDown * 0.1f);
		return true;
	}
	return false;
}


//////////////////////////////////////////////////////////////////////////
//
// Button one pressed.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::ActivateComponent(int x, int y)
{
	UpdateModifiers();
	mpcEditor->ButtonPressed(ToGrid(x, y));
}


//////////////////////////////////////////////////////////////////////////
//
// Button one released.  Released twice in half a second within a few
// pixels it's a double click, which edits the properties of the
// component it's on, as Logicim's componentViewDoubleClicked.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::ExecuteComponent(int x, int y)
{
	int64	iNow;

	UpdateModifiers();
	mpcEditor->ButtonReleased(ToGrid(x, y));

	iNow = GetDebugTickCount();
	if ((miLastRelease != 0) && (iNow - miLastRelease <= CIRCUIT_CANVAS_DOUBLE_CLICK_MILLISECONDS) && (abs(x - msLastRelease.x) <= CIRCUIT_CANVAS_DOUBLE_CLICK_PIXELS) && (abs(y - msLastRelease.y) <= CIRCUIT_CANVAS_DOUBLE_CLICK_PIXELS))
	{
		miLastRelease = 0;
		mpcContent->EditProperties();
	}
	else
	{
		miLastRelease = iNow;
		msLastRelease.Init(x, y);
	}
}


//////////////////////////////////////////////////////////////////////////
//
// Button one pressed with shift or control held.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::StartDragSelect(int x, int y)
{
	ActivateComponent(x, y);
}


//////////////////////////////////////////////////////////////////////////
//
// The drag goes on outside the canvas.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::ContDragSelect(int x, int y)
{
	UpdateModifiers();
	mpcEditor->PointerMoved(ToGrid(x, y));
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::StopDragSelect(int x, int y)
{
	ExecuteComponent(x, y);
}


//////////////////////////////////////////////////////////////////////////
//
// Button three drags the circuit about, as Logicim's middle button.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::StartDragScroll(int x, int y)
{
	msScroll.Init(x, y);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::ContDragScroll(int x, int y)
{
	mpCircuitDraw->GetViewport()->Scroll(msScroll.x - x, msScroll.y - y);
	msScroll.Init(x, y);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::Delete(void)
{
	mpcEditor->Delete();
}


//////////////////////////////////////////////////////////////////////////
//
// Escape.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::ClearSelection(void)
{
	mpcEditor->StopEdit();
}


//////////////////////////////////////////////////////////////////////////
//
// Logicim's keys that are printable.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::Printable(char c, bool bInsert)
{
	const char*	szXMLName;

	switch (c)
	{
	case 'r':
		mpcEditor->RotateRight();
		return;
	case 'R':
		mpcEditor->RotateLeft();
		return;
	case 'e':
		mpcContent->EditProperties();
		return;
	case 'g':
		szXMLName = "Ground";
		break;
	case 'v':
		szXMLName = "PositivePower";
		break;
	case 'p':
		szXMLName = "Pin";
		break;
	case 'c':
		szXMLName = "Constant";
		break;
	case 'A':
		szXMLName = "AndGate";
		break;
	case 'O':
		szXMLName = "OrGate";
		break;
	case 'X':
		szXMLName = "XorGate";
		break;
	case 'N':
		szXMLName = "Inverter";
		break;
	case 'C':
		szXMLName = "Clock";
		break;
	case 'P':
		szXMLName = "Probe";
		break;
	case 'S':
		szXMLName = "Splitter";
		break;
	case 'T':
		szXMLName = "Tunnel";
		break;
	case 'L':
		szXMLName = "Label";
		break;
	case 'F':
		szXMLName = "DTypeFlipFlop";
		break;
	default:
		return;
	}
	mpcEditor->StartPlaceComponent(szXMLName);
}


//////////////////////////////////////////////////////////////////////////
//
// x and y are relative to the canvas' top left.
//
//////////////////////////////////////////////////////////////////////////
SFloat64Vec2 CCircuitCanvas::ToGrid(int x, int y)
{
	SFloat64Vec2	sScreen;
	SFloat64Vec2	sGrid;

	sScreen.Init(x, y);
	mpCircuitDraw->GetViewport()->ScreenToGrid(&sGrid, &sScreen);
	return sGrid;
}


//////////////////////////////////////////////////////////////////////////
//
// The selection mode follows control and shift.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvas::UpdateModifiers(void)
{
	CComponentInput*	pcInput;

	pcInput = mpWindow->GetComponentInput();
	if (pcInput != NULL)
	{
		mpcEditor->SetModifiers(pcInput->IsControl(), pcInput->IsShift());
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
Ptr<CCircuitCanvasDraw> CCircuitCanvas::GetCircuitDraw(void) { return mpCircuitDraw; }

