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
    SInt2           sSize;

    if (pCanvas->IsValid())
    {
        sSize = pCanvas->GetActualSize();
        cRect.Init(sSize);

    }

    return true;
}

