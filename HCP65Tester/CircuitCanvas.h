#ifndef __CIRCUIT_CANVAS_H__
#define __CIRCUIT_CANVAS_H__
#include "WindowLib/Canvas.h"
#include "CircuitCanvasDraw.h"


class CTesterWindowContent;
class CCircuitEditor;


//The circuit editor's canvas.  It passes the pointer, button one and the keys to the editor (see CCircuitEditor) in
//grid space, tells the window content where the pointer is over it, zooms with the wheel and pans with button three.
//
//Button one presses and releases come as ActivateComponent and ExecuteComponent, or with shift or control held as a
//select drag.  Keys are Logicim's: r turns right and R left; Delete deletes; Escape stops the edit and clears the
//selection; e, or a double click, edits the properties of the component under the pointer or selected; and letters
//place components, as Logicim's KeyInputsFactory (g a ground, v a supply, p a pin, c a constant; with shift: A an AND
//gate, O an OR, X an XOR, N an inverter, C a clock, P a probe, S a splitter, T a tunnel, L a label and F a flip flop).
//Logicim's control and alt placements aren't printable keys and aren't here.
class CCircuitCanvas : public CCanvas
{
CONSTRUCTABLE(CCircuitCanvas);
DESTRUCTABLE(CCircuitCanvas);
protected:
	Ptr<CCircuitCanvasDraw>		mpCircuitDraw;
	CTesterWindowContent*		mpcContent;
	CCircuitEditor*				mpcEditor;
	SInt32Vec2					msScroll;
	SInt32Vec2					msPointer;  //Where the pointer last moved over the canvas, the wheel zooms around it.
	int64					miLastRelease;  //When button one was last released, for double clicks, 0 if that was one.
	SInt32Vec2					msLastRelease;

public:
	void						Init(Ptr<CWindow> pWindow, Ptr<CCircuitCanvasDraw> pDraw, CTesterWindowContent* pcContent, CCircuitEditor* pcEditor);
	void						Class(void) override;

	Ptr<CCircuitCanvasDraw>		GetCircuitDraw(void);

	void						MovedOver(int x, int y) override;
	bool						Wheel(int iRight, int iDown) override;
	void						ActivateComponent(int x, int y) override;
	void						ExecuteComponent(int x, int y) override;
	void						StartDragSelect(int x, int y) override;
	void						ContDragSelect(int x, int y) override;
	void						StopDragSelect(int x, int y) override;
	void						StartDragScroll(int x, int y) override;
	void						ContDragScroll(int x, int y) override;
	void						Delete(void) override;
	void						ClearSelection(void) override;
	void						Printable(char c, bool bInsert) override;

protected:
	SFloat64Vec2				ToGrid(int x, int y);
	void						UpdateModifiers(void);
};


#endif // __CIRCUIT_CANVAS_H__

