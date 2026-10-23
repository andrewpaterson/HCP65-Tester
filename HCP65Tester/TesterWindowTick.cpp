#include "StandardLib/ClassDefines.h"
#include "SupportLib/ColourARGB32.h"
#include "WindowLib/Window.h"
#include "TesterWindowContent.h"
#include "TesterWindowTick.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowTick::Init(STesterWindowData* pcData)
{
    PreInit();

    CWindowTick::Init();
    mpcData = pcData;
    
    PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowTick::Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds)
{
    ETesterDialog   eDialog;

    pWindow->Paint();

    if ((mpcData->eDialog != TD_None) && mpcData->pcContent)
    {
        eDialog = mpcData->eDialog;
        mpcData->eDialog = TD_None;
        mpcData->pcContent->ShowDialog(eDialog);
    }
}

