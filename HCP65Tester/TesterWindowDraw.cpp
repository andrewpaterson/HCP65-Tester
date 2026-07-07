#include "StandardLib/ClassDefines.h"
#include "SupportLib/ColourARGB32.h"
#include "WindowLib/Canvas.h"
#include "TesterWindowDraw.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowDraw::Init(STesterWindowData* pcData)
{
    PreInit();

    CCanvasDraw::Init();
    mpcData = pcData;

    PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CTesterWindowDraw::Draw(Ptr<CCanvas> pCanvas)
{
    CRectangle      cRect;
    ARGB32	        sBlack;
    ARGB32	        sWhite;
    ARGB32	        sGrey;
    SInt32Vec2      sSize;

    if (pCanvas->IsValid())
    {
        sWhite = Set32BitColour((uint8)255, 255, 255);
        sGrey = Set32BitColour((uint8)64, 64, 64);
        sBlack = Set32BitColour((uint8)0, 0, 0);

        sSize = pCanvas->GetActualSize();
        sSize.x--;
        sSize.y--;
        cRect.Init(sSize);

        pCanvas->DrawBox(&cRect, true, sBlack);
        pCanvas->DrawBox(&cRect, false, sWhite);
        cRect.Shrink(1, 1, 1, 1);
        if (cRect.IsValid())
        {
            pCanvas->DrawBox(&cRect, false, sGrey);
        }
    }

    return true;
}

