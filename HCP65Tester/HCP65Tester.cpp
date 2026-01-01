#include <windows.h>
#include "BaseLib/Logger.h"
#include "BaseLib/FastFunctions.h"
#include "BaseLib/FileUtil.h"
#include "BaseLib/TypeNames.h"
#include "BaseLib/TypeConverter.h"
#include "BaseLib/GlobalMemory.h"
#include "BaseLib/GlobalDataTypesIO.h"
#include "StandardLib/Objects.h"
#include "WindowLib/Window.h"
#include "WinGdiLib/WinGDIWindowFactory.h"
#include "TesterWindowDraw.h"
#include "TesterWindowTick.h"

#include "WindowsCOM.h"
#include "BoardPins.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance,	HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	_CrtSetBreakAlloc(0);

	gcLogger.Init();
	FastFunctionsInit();
	TypesInit();
	TypeConverterInit();
	MemoryInit();
	DataIOInit();
	ObjectsInit();

	CBoardPins	cBoard;
	CChars		szOutputCommand;

	cBoard.Init();

	cBoard.Add( 1, "Register11", PD_Output, PS_Inverted);
	cBoard.Add( 2, "Register10", PD_Output, PS_Inverted);
	//5V
	cBoard.Add( 4, "Register9", PD_Output, PS_Inverted);
	cBoard.Add( 5, "Register8", PD_Input, PS_Inverted);
	cBoard.Add( 6, "Register7", PD_Output, PS_Inverted);
	cBoard.Add( 7, "Register6", PD_Output, PS_Inverted);
	cBoard.Add( 8, "Register5", PD_Output, PS_Inverted);
	//GND					  
	cBoard.Add(10, "Register4", PD_Output, PS_Inverted);
	cBoard.Add(11, "Register3", PD_Output, PS_Inverted);
	cBoard.Add(12, "Register2", PD_Output, PS_Inverted);
	cBoard.Add(13, "Register1", PD_Output, PS_Inverted);
	cBoard.Add(14, "Register0", PD_Output, PS_Inverted);
	cBoard.Add(15, "Device19", PD_Output, PS_Inverted);
	cBoard.Add(16, "Device18", PD_Output, PS_Inverted);
	cBoard.Add(17, "Device17", PD_Output, PS_Inverted);
	cBoard.Add(18, "Device16", PD_Output, PS_Inverted);
	cBoard.Add(19, "Device7",  PD_Output, PS_Inverted);
	//GND					  
	cBoard.Add(21, "Device6", PD_Output, PS_Inverted);
	cBoard.Add(22, "Device5", PD_Output, PS_Inverted);
	cBoard.Add(23, "Device4", PD_Output, PS_Inverted);
	cBoard.Add(24, "Device3", PD_Output, PS_Inverted);
	cBoard.Add(25, "Device2", PD_Output, PS_Inverted);
	cBoard.Add(26, "Reset", PD_Input, PS_Inverted);
	cBoard.Add(27, "Device1", PD_Output, PS_Inverted);
	cBoard.Add(28, "Registers", PD_Output, PS_Inverted);

	cBoard.Add(29, "Devices", PD_Output, PS_Inverted);
	cBoard.Add(30, "RAM Swap", PD_Input, PS_Normal);
	//5V
	cBoard.Add(32, "RAM", PD_Output, PS_Inverted);
	cBoard.Add(33, "ROM", PD_Output, PS_Inverted);
	//N.C.
	cBoard.Add(35, "Kernal Mode", PD_Input, PS_Normal);
	//N.C.
	//GND					  
	cBoard.Add(38, "Main Memory", PD_Output, PS_Inverted);
	cBoard.Add(39, "A23", PD_Input, PS_Normal);
	cBoard.Add(40, "A22", PD_Input, PS_Normal);
	cBoard.Add(41, "A21", PD_Input, PS_Normal);
	cBoard.Add(42, "A20", PD_Input, PS_Normal);
	cBoard.Add(43, "A19", PD_Input, PS_Normal);
	cBoard.Add(44, "A18", PD_Input, PS_Normal);
	cBoard.Add(45, "A17", PD_Input, PS_Normal);
	cBoard.Add(46, "A16", PD_Input, PS_Normal);
	cBoard.Add(47, "A15", PD_Input, PS_Normal);
	//GND					  
	cBoard.Add(49, "A3", PD_Input, PS_Normal);
	cBoard.Add(50, "A2", PD_Input, PS_Normal);
	cBoard.Add(51, "A1", PD_Input, PS_Normal);
	cBoard.Add(52, "A0", PD_Input, PS_Normal);
	cBoard.Add(53, "Register12", PD_Output, PS_Inverted);
	cBoard.Add(54, "Register13", PD_Output, PS_Inverted);
	cBoard.Add(55, "Register14", PD_Output, PS_Inverted);
	cBoard.Add(56, "Register15", PD_Output, PS_Inverted);

	szOutputCommand.Init();
	cBoard.GenerateOutput(&szOutputCommand);
	szOutputCommand.DumpKill();

	cBoard.Kill();

	WindowsCOM();

	//{
	//	CWinGDIWindowFactory	cNativeFactory;
	//	CWindow					cTesterWindow;
	//	CTesterWindowDraw		cDraw;
	//	CTesterWindowTick		cTick;
	//	STesterWindowData		sData;

	//	cNativeFactory.Init(&gcMemoryAllocator,
	//		hInstance,
	//		hPrevInstance,
	//		nCmdShow,
	//		"HCP65Tester");

	//	cDraw.Init(&sData);
	//	cTick.Init(&sData);
	//	cTesterWindow.Init("HCP65 Board Tester", &cNativeFactory, &cTick, &cDraw);

	//	cTesterWindow.Show();

	//	cTesterWindow.Kill();

	//	cNativeFactory.Kill();
	//}

	ObjectsKill();
	DataIOKill();
	MemoryKill();
	TypeConverterKill();
	TypesKill();
	FastFunctionsKill();

	gcLogger.Kill();

	_CrtDumpMemoryLeaks();
	return 0;
}

