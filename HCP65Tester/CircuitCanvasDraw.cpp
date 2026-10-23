#include "StandardLib/ClassDefines.h"
#include "WindowLib/Canvas.h"
#include "CircuitCanvasDraw.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvasDraw::Init(ARGB32 sBackground, ARGB32 sSmallGrid, ARGB32 sLargeGrid)
{
	PreInit();
	CCanvasDraw::Init();
	msBackground = sBackground;
	msSmallGrid = sSmallGrid;
	msLargeGrid = sLargeGrid;
	mfZoom = 1.0f;
	mbGrid = true;
	PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvasDraw::Class(void)
{
	CCanvasDraw::Class();
	U_UInt32(msBackground);
	U_UInt32(msSmallGrid);
	U_UInt32(msLargeGrid);
	U_Float32(mfZoom);
	U_Bool(mbGrid);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CCircuitCanvasDraw::Draw(Ptr<CCanvas> pCanvas)
{
	CInt32Rect	cRect;
	SInt32Vec2	sSize;
	float32		fSpacing;
	float32		fX;
	float32		fY;
	float32		fStartX;
	float32		fStartY;
	int			iDot;
	ARGB32		sDot;

	if (!pCanvas->IsValid())
	{
		return true;
	}

	sSize = pCanvas->GetActualSize();
	cRect.Init(0, 0, sSize.x - 1, sSize.y - 1);
	pCanvas->DrawBox(&cRect, true, msBackground);

	if (!mbGrid || (mfZoom < 0.3f))
	{
		return true;
	}

	fSpacing = GetGridSpacing();
	sDot = (mfZoom < 1.5f) ? msSmallGrid : msLargeGrid;
	iDot = (mfZoom < 2.0f) ? 1 : (int)mfZoom;

	//A grid point is at the centre of the canvas.
	fStartX = (sSize.x / 2.0f);
	while (fStartX - fSpacing >= 0.0f)
	{
		fStartX -= fSpacing;
	}
	fStartY = (sSize.y / 2.0f);
	while (fStartY - fSpacing >= 0.0f)
	{
		fStartY -= fSpacing;
	}

	for (fY = fStartY; fY < sSize.y; fY += fSpacing)
	{
		for (fX = fStartX; fX < sSize.x; fX += fSpacing)
		{
			if (iDot == 1)
			{
				pCanvas->DrawPixel((int)fX, (int)fY, sDot);
			}
			else
			{
				cRect.Init((int)fX - iDot / 2, (int)fY - iDot / 2, (int)fX - iDot / 2 + iDot - 1, (int)fY - iDot / 2 + iDot - 1);
				pCanvas->DrawBox(&cRect, true, sDot);
			}
		}
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
// Logicim's zoom goes from 0.2 to 6.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvasDraw::SetZoom(float32 fZoom)
{
	if (fZoom < 0.2f)
	{
		fZoom = 0.2f;
	}
	if (fZoom > 6.0f)
	{
		fZoom = 6.0f;
	}
	mfZoom = fZoom;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
float32 CCircuitCanvasDraw::GetZoom(void) { return mfZoom; }
void CCircuitCanvasDraw::SetGrid(bool bGrid) { mbGrid = bGrid; }
bool CCircuitCanvasDraw::IsGrid(void) { return mbGrid; }
float32 CCircuitCanvasDraw::GetGridSpacing(void) { return 10.0f * mfZoom; }

