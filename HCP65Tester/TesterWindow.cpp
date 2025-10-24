#include "WinGdiLib/WinGDICanvas.h"
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
    RECT            cRect;
    CWinGDICanvas*  pcNativeCanvas;
    HDC				hMemDC;
    //uint8*          puiPixelData;

	if (iTotalTimeInMillieconds > miTime + 100)
	{
		miTime = iTotalTimeInMillieconds + 100;

        cRect.left = 0;
        cRect.right = mcCanvas.GetWidth();
        cRect.top = 0;
        cRect.bottom = mcCanvas.GetHeight();

        pcNativeCanvas = (CWinGDICanvas*)mcCanvas.GetNativeCanvas();
        hMemDC = pcNativeCanvas->GetMemDC();

        HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 255));
        FillRect(hMemDC, &cRect, blueBrush);
        DeleteObject(blueBrush);

        // Draw a 10x10 grid of colored pixels in memory DC
        for (int x = 50; x < 150; x += 10)
        {
            for (int y = 50; y < 150; y += 10)
            {
                SetPixel(hMemDC, x + miX, y, RGB(255, 0, 0)); // Red pixels
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

