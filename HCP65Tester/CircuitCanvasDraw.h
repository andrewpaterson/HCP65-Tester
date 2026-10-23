#ifndef __CIRCUIT_CANVAS_DRAW_H__
#define __CIRCUIT_CANVAS_DRAW_H__
#include "SupportLib/ColourARGB32.h"
#include "WindowLib/CanvasDraw.h"


//Draws the editor's background with Logicim's point grid: a dot at every grid point, ten pixels apart at a zoom of
//one, from the centre of the canvas.  Like Logicim's the grid isn't drawn when zoomed far out, its dots are lighter
//from a zoom of 1.5 and become small squares from 2.
class CCircuitCanvasDraw : public CCanvasDraw
{
CONSTRUCTABLE(CCircuitCanvasDraw);
DESTRUCTABLE(CCircuitCanvasDraw);
protected:
	ARGB32		msBackground;
	ARGB32		msSmallGrid;
	ARGB32		msLargeGrid;
	float32		mfZoom;
	bool		mbGrid;

public:
	void		Init(ARGB32 sBackground, ARGB32 sSmallGrid, ARGB32 sLargeGrid);
	void		Class(void) override;

	bool		Draw(Ptr<CCanvas> pCanvas) override;

	void		SetZoom(float32 fZoom);
	float32		GetZoom(void);
	void		SetGrid(bool bGrid);
	bool		IsGrid(void);
	float32		GetGridSpacing(void);
};


#endif // __CIRCUIT_CANVAS_DRAW_H__

