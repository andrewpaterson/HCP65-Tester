#include <windows.h>
#include "BaseLib/FastFunctions.h"
#include "BaseLib/Logger.h"
#include "WinGdiLib/WinGDIWindow.h"
#include "TesterWindow.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	gcLogger.Init();
	FastFunctionsInit();

	CWinGDIWindow	cNativeWindow;
	CTesterWindow	cTesterWindow;

	cNativeWindow.Init(hInstance, 
						hPrevInstance, 
						nCmdShow,
						"HCP65Tester", 
						"HCP65 Board Tester");
	cTesterWindow.Init(&cNativeWindow);

	cTesterWindow.Show();

	cTesterWindow.Kill();
	cNativeWindow.Kill();


	FastFunctionsKill();
	gcLogger.Kill();
	return 0;
}

