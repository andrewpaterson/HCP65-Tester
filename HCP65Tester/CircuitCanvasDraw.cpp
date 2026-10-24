#include "StandardLib/ClassDefines.h"
#include "WindowLib/Canvas.h"
#include "LogicimLib/CircuitPainter.h"
#include "TesterCircuit.h"
#include "CircuitCanvasDraw.h"


//////////////////////////////////////////////////////////////////////////
//
// The circuit isn't owned; it must outlive the draw.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvasDraw::Init(CTesterCircuit* pcCircuit)
{
	PreInit();
	CCanvasDraw::Init();
	mpcCircuit = pcCircuit;
	mpMap = OMalloc<CGeometryMap>();
	mpSprites = OMalloc<CSpriteMap>();
	mpDestImage = NULL;
	mcViewport.Init(0, 0);
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
	U_Pointer(mpcCircuit);
	M_Pointer(mpMap);
	M_Pointer(mpSprites);
	M_Pointer(mpDestImage);
	U_Data(CCircuitViewport, mcViewport);
	U_Bool(mbGrid);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvasDraw::Free(void)
{
	mcViewport.Kill();
	CCanvasDraw::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// Canvases without an image of their own are drawn into an image the size
// of the canvas, which is then copied on.
//
//////////////////////////////////////////////////////////////////////////
bool CCircuitCanvasDraw::Draw(Ptr<CCanvas> pCanvas)
{
	SInt32Vec2			sSize;
	Ptr<CImage>			pImage;
	CCircuitPainter		cPainter;
	CCircuitColours*	pcColours;
	CInt32Rect			cRect;
	bool				bResult;

	if (!pCanvas->IsValid())
	{
		return true;
	}

	sSize = pCanvas->GetActualSize();
	pImage = pCanvas->GetImageOrNull();
	if (pImage.IsNull())
	{
		if (mpDestImage.IsNull() || (mpDestImage->GetWidth() != (uint32)sSize.x) || (mpDestImage->GetHeight() != (uint32)sSize.y) ||
			(mpDestImage->GetColourFormat() != pCanvas->GetColourFormat()) || (mpDestImage->GetAlphaBits() != pCanvas->GetAlphaBits()))
		{
			mpDestImage = OMalloc<CImage>(sSize.x, sSize.y, pCanvas->GetColourFormat(), pCanvas->GetColourOrder(), pCanvas->GetColourBits(), pCanvas->GetAlphaBits());
		}
		pImage = mpDestImage;
	}
	else
	{
		mpDestImage = NULL;
	}

	pcColours = mpcCircuit->GetColours();
	mcViewport.SetSize(sSize.x, sSize.y);
	mpMap->Clear();
	mpMap->SetViewport(pImage);
	mpSprites->Clear();
	if (mpSprites->GetViewport() != pImage)
	{
		mpSprites->SetViewport(pImage);
		mpSprites->SetBlitterCache(mpSprites->CreateBlitterCache());
	}
	cPainter.Init(&mcViewport, &mpMap, &mpSprites, pcColours, mpcCircuit->GetFonts(), mpcCircuit->GetTime());

	mpMap->AddRectangle(0, 0, sSize.x, sSize.y, pcColours->msBackground);
	if (mbGrid)
	{
		PaintCircuitGrid(&cPainter);
	}
	mpcCircuit->GetView()->Paint(&cPainter);
	cPainter.Kill();

	cRect.Init(0, 0, sSize.x, sSize.y);
	bResult = mpMap->Blit(&cRect, false);
	bResult &= mpSprites->Blit(&cRect, true);
	if (mpDestImage.IsNotNull())
	{
		pCanvas->DrawImage(0, 0, mpDestImage);
	}
	return bResult;
}


//////////////////////////////////////////////////////////////////////////
//
// Logicim's zoom goes from 0.2 to 6.
//
//////////////////////////////////////////////////////////////////////////
void CCircuitCanvasDraw::SetZoom(float32 fZoom)
{
	mcViewport.SetZoom(fZoom);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
float32 CCircuitCanvasDraw::GetZoom(void) { return (float32)mcViewport.GetZoom(); }
void CCircuitCanvasDraw::SetGrid(bool bGrid) { mbGrid = bGrid; }
bool CCircuitCanvasDraw::IsGrid(void) { return mbGrid; }
float32 CCircuitCanvasDraw::GetGridSpacing(void) { return (float32)mcViewport.GetPixelsPerGrid(); }
CCircuitViewport* CCircuitCanvasDraw::GetViewport(void) { return &mcViewport; }

