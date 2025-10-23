#include <windows.h>
#include "BaseLib/FastFunctions.h"
#include "BaseLib/GlobalMemory.h"
#include "BaseLib/Logger.h"
#include "WinGdiLib/WinGDIWindowFactory.h"
#include "TesterWindow.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	gcLogger.Init();
	FastFunctionsInit();
	MemoryInit();

	CWinGDIWindowFactory	cNativeFactory;
	CTesterWindow			cTesterWindow;

	cNativeFactory.Init(&gcMemoryAllocator,
					    hInstance, 
					    hPrevInstance, 
					    nCmdShow,
					    "HCP65Tester");
	cTesterWindow.Init("HCP65 Board Tester", &cNativeFactory);

	cTesterWindow.Show();

	cTesterWindow.Kill();


	MemoryKill();
	FastFunctionsKill();
	gcLogger.Kill();
	return 0;
}

