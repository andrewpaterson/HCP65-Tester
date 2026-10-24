#ifndef __CIRCUIT_CANVAS_DRAW_H__
#define __CIRCUIT_CANVAS_DRAW_H__
#include "SupportLib/ColourARGB32.h"
#include "SupportLib/GeometryMap.h"
#include "SupportLib/SpriteMap.h"
#include "WindowLib/CanvasDraw.h"
#include "LogicimLib/CircuitViewport.h"


class CTesterCircuit;


//Draws the editor: the background, Logicim's point grid and the circuit, through a geometry map.  Grid 0, 0 is at the
//centre of the canvas and a grid unit is ten pixels at a zoom of one.  Like Logicim's the grid isn't drawn when zoomed
//far out, its dots are lighter from a zoom of 1.5 and become small squares from 2.  Text goes in a sprite map drawn
//over the geometry map.
class CCircuitCanvasDraw : public CCanvasDraw
{
CONSTRUCTABLE(CCircuitCanvasDraw);
DESTRUCTABLE(CCircuitCanvasDraw);
protected:
	CTesterCircuit*		mpcCircuit;
	Ptr<CGeometryMap>	mpMap;
	Ptr<CSpriteMap>		mpSprites;
	Ptr<CImage>			mpDestImage;
	CCircuitViewport	mcViewport;
	bool				mbGrid;

public:
	void				Init(CTesterCircuit* pcCircuit);
	void				Class(void) override;
	void				Free(void) override;

	bool				Draw(Ptr<CCanvas> pCanvas) override;

	void				SetZoom(float32 fZoom);
	float32				GetZoom(void);
	void				SetGrid(bool bGrid);
	bool				IsGrid(void);
	float32				GetGridSpacing(void);
	CCircuitViewport*	GetViewport(void);
};


#endif // __CIRCUIT_CANVAS_DRAW_H__

