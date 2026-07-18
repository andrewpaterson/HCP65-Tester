#include <windows.h>
#include "BaseLib/Logger.h"
#include "BaseLib/FastFunctions.h"
#include "BaseLib/FileUtil.h"
#include "BaseLib/TypeNames.h"
#include "BaseLib/TypeConverter.h"
#include "BaseLib/GlobalMemory.h"
#include "BaseLib/GlobalDataTypesIO.h"
#include "BaseLib/DebugOutput.h"
#include "StandardLib/Objects.h"
#include "WindowLib/Window.h"
#include "WinGdiLib/WinGDIWindowFactory.h"
#include "TesterWindowDraw.h"
#include "TesterWindowTick.h"

#include "WindowsCOM.h"
#include "BoardPins.h"
#include "UART.h"
#include "AddressDecodeBoard.h"
#include "TestBoard.h"


#define USend(u, c) 	if (!(u)->Send(c)) \
{ \
	return false; \
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool SetupCommands(CBoardPins* pcBoard, CUART* pcUART)
{
	CChars		szOutputCommand;
	CChars		szWriteCommand;
	CChars		szPowerCommand;
	CChars		szReadCommand;

	szPowerCommand.Init();
	pcBoard->GeneratePower(&szPowerCommand);
	if (!szPowerCommand.Empty())
	{
		szPowerCommand.Dump();
		USend(pcUART, szPowerCommand.Text());
		EngineOutput("\n");
	}
	szPowerCommand.Kill();

	szReadCommand.Init();
	pcBoard->GenerateRead(&szReadCommand);
	szReadCommand.Dump();
	USend(pcUART, szReadCommand.Text());
	EngineOutput("\n");
	szReadCommand.Kill();

	szOutputCommand.Init();
	pcBoard->GenerateOutput(&szOutputCommand);
	szOutputCommand.Dump();
	USend(pcUART, szOutputCommand.Text());
	EngineOutput("\n");
	USend(pcUART, szOutputCommand.Text());
	szOutputCommand.Kill();

	szWriteCommand.Init();
	pcBoard->GenerateWrite(&szWriteCommand);
	szWriteCommand.Dump();
	USend(pcUART, szWriteCommand.Text());
	EngineOutput("\n");
	szWriteCommand.Kill();

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool TestBoardLoop(char* szComPort, size uiBoardPins)
{
	bool		bSuccess;
	CBoardPins  cBoard;
	CUART		cUART;

	cUART.Init(szComPort);
	if (!cUART.Open())
	{
		return false;
	}

	USend(&cUART, "POW");
	USend(&cUART, "PGa1Gb15a1");

	cBoard.Init();
	//SetupAddressDecode(&cBoard);
	SetupTestBoard(&cBoard, uiBoardPins);

	if (!SetupCommands(&cBoard, &cUART))
	{
		return false;
	}

	EngineOutput("\n");

	bSuccess = true;
	while (bSuccess)
	{
		bSuccess = TestTestBoard(&cUART, &cBoard);
	}

	USend(&cUART, "POW");
	cUART.Close();
	cUART.Kill();

	cBoard.Kill();

	return bSuccess;
}


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

	bool bResult;

	bResult = TestBoardLoop("COM3", 64);

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

	if (bResult)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

