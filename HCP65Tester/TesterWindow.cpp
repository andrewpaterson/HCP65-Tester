#include "BaseLib/Chars.h"
#include "SupportLib/ColourARGB32.h"
#include "TesterWindow.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindow::Init(char* szWindowTitle, CNativeWindowFactory* pcWindowFactory)
{
	CWindow::Init(szWindowTitle, pcWindowFactory);
	miTime = 0;
    miX = 0;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindow::Kill(void)
{
	CWindow::Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindow::Tick(int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds)
{
    CRectangle      cRect;
    ARGB32          sGrey;
    ARGB32          sRed;

	if (iTotalTimeInMillieconds > miTime + 16)
	{
		miTime = iTotalTimeInMillieconds + 16;

        cRect.miLeft = 0;
        cRect.miRight = mcCanvas.GetWidth();
        cRect.miTop = 0;
        cRect.miBottom = mcCanvas.GetHeight();

        sGrey = Set32BitColour((uint8)96, 96, 96);
        mcCanvas.FillRect(&cRect, sGrey);

        sRed = Set32BitColour((uint8)255, 96, 255);
        for (int x = 50; x < 200; x += 10)
        {
            for (int y = 50; y < 200; y += 10)
            {
                mcCanvas.SetPixel(x + miX, y, sRed);
            }
        }

        miX++;
        if (miX > 100)
        {
            miX -= 100;
        }

        Paint();
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindow::CanvasChanged(CCanvas* pcNewCanvas)
{
}

