#include "TesterWindow.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
int FAR PASCAL WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	bool			bResult;
	int				iScreenWidth;
	int				iScreenHeight;
	bool			bWindowed;
	CWinConfig		cConfig;
	D3DDEVTYPE		d3dDevType;
	char			szTitle[] = "Viewer Release " ENGINE_VERSION;
	CTesterWindow	gcWindow;

	FastFunctionsInit();
	
	cConfig.Init();
	if ((cConfig.miWidth == 0) || (cConfig.miHeight == 0))
	{
		iScreenWidth = CWindow::GetScreenWidth();
		iScreenHeight = CWindow::GetScreenHeight();
	}
	else
	{
		iScreenWidth = cConfig.miWidth;
		iScreenHeight = cConfig.miHeight;
	}

	srand((unsigned)time(NULL));

	UnknownsInit();
	gcWindow.Init(hInstance, szTitle);

	bWindowed = cConfig.mbWindowed;

	if (cConfig.meMode == WM_DX_HAL)
		d3dDevType = D3DDEVTYPE_HAL;
	else if (cConfig.meMode == WM_DX_REF)
		d3dDevType = D3DDEVTYPE_REF;
	else if (cConfig.meMode == WM_DX_SW)
		d3dDevType = D3DDEVTYPE_SW;

	if (bWindowed)
	{
		if ((cConfig.miWidth == 0) || (cConfig.miHeight == 0))
		{
			iScreenWidth = (int)(iScreenWidth * 0.9f);
			iScreenHeight = (int)(iScreenHeight * 0.9f);
		}

		gcWindow.Windowed(nCmdShow, 60, 60, iScreenWidth, iScreenHeight);
		bResult = InitMain(0, d3dDevType, gcWindow.hWnd, 0, 0, D3DFMT_UNKNOWN, true, false, gcWindow.hWnd);
	}
	else
	{
		gcWindow.FullScreen(nCmdShow);
		bResult = InitMain(0, d3dDevType, gcWindow.hWnd, iScreenWidth, iScreenHeight, D3DFMT_X8R8G8B8, true, true, gcWindow.hWnd);
	}

	if (!bResult)
	{
		return 1;
	}

	gcWindow.WinLoop();
	gcWindow.Kill();

	KillMain();
	FastFunctionsKill();
	return 0;
}

