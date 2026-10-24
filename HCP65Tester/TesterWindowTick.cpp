#include "StandardLib/ClassDefines.h"
#include "SupportLib/ColourARGB32.h"
#include "SupportLib/PNGWriter.h"
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
    mszSnapshot.Init();
    miSnapshotTicks = -1;

    PostInit();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowTick::Class(void)
{
    CWindowTick::Class();
    U_Pointer(mpcData);
    U_String(mszSnapshot);
    U_Int32(miSnapshotTicks);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowTick::Free(void)
{
    mszSnapshot.Kill();
    CWindowTick::Free();
}


//////////////////////////////////////////////////////////////////////////
//
// Saves the window as a PNG after iTicks ticks and closes it, to check how
// it draws without looking at the screen.
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowTick::SetSnapshot(const char* szFilename, int iTicks)
{
    mszSnapshot.Set(szFilename);
    miSnapshotTicks = iTicks;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowTick::Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds)
{
    ETesterDialog   eDialog;

    if (mpcData->pcContent)
    {
        mpcData->pcContent->Tick(iUpdateTimeInMillieconds);
    }
    pWindow->Paint();

    if (miSnapshotTicks >= 0)
    {
        if (miSnapshotTicks == 0)
        {
            SavePNG(pWindow->GetCanvas()->GetImageOrNull(), mszSnapshot.Text());
            pWindow->Stop();
        }
        miSnapshotTicks--;
    }

    if ((mpcData->eDialog != TD_None) && mpcData->pcContent)
    {
        eDialog = mpcData->eDialog;
        mpcData->eDialog = TD_None;
        mpcData->pcContent->ShowDialog(eDialog);
    }
}

