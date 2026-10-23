#ifndef __CIRCUIT_CANVAS_H__
#define __CIRCUIT_CANVAS_H__
#include "WindowLib/Canvas.h"
#include "CircuitCanvasDraw.h"


class CTesterWindowContent;


//The circuit editor's canvas.  It tells the window content where the pointer is over it and zooms with the wheel.
class CCircuitCanvas : public CCanvas
{
CONSTRUCTABLE(CCircuitCanvas);
DESTRUCTABLE(CCircuitCanvas);
protected:
	Ptr<CCircuitCanvasDraw>		mpCircuitDraw;
	CTesterWindowContent*		mpcContent;

public:
	void						Init(Ptr<CWindow> pWindow, Ptr<CCircuitCanvasDraw> pDraw, CTesterWindowContent* pcContent);
	void						Class(void) override;

	Ptr<CCircuitCanvasDraw>		GetCircuitDraw(void);

	void						MovedOver(int x, int y) override;
	bool						Wheel(int iRight, int iDown) override;
};


#endif // __CIRCUIT_CANVAS_H__

