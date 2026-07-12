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



#define USend(u, c) 	if (!(u)->Send(c)) \
{ \
	return false; \
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void SetupDebugBoard(CBoardPins* pcBoard, size uiNumPins)
{
	size		i;

	for (i = 1; i <= uiNumPins; i++)
	{
		pcBoard->AddSignal(i, SizeToString(i), PD_Input, PS_Normal);
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool TestDebugBoard(CBoardPins* pcBoard, CUART* pcUART)
{
	CChars	szWriteCommand;
	size	uiNumNybbles;
	CChars	szResponse;
	size	i;

	uiNumNybbles = pcBoard->NumPins() / 4;
	for (i = 0; i < uiNumNybbles; i++)
	{
		if (i % 4 == 0)
		{
			EngineOutput("-----------------\n");
		}

		szWriteCommand.Init("W");
		szWriteCommand.Append('0', i);
		szWriteCommand.Append(ToUpper(IntToString(i % 16, 16)));
		szWriteCommand.Append('0', uiNumNybbles - i - 1);
		USend(pcUART, szWriteCommand.Text());
		szWriteCommand.Kill();

		szResponse.Init();
		pcUART->Send("R0_7", &szResponse);
		szResponse.DumpKill();
	}

	for (i = 0; i < uiNumNybbles; i++)
	{
		if (i % 4 == 0)
		{
			EngineOutput("-----------------\n");
		}

		szWriteCommand.Init("W");
		szWriteCommand.Append('F', i);
		szWriteCommand.Append(ToUpper(IntToString(i % 16, 16)));
		szWriteCommand.Append('F', uiNumNybbles - i - 1);
		USend(pcUART, szWriteCommand.Text());
		szWriteCommand.Kill();

		szResponse.Init();
		pcUART->Send("R0_7", &szResponse);
		szResponse.DumpKill();
	}

	return true;
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
	USend(pcUART, szPowerCommand.Text());
	szPowerCommand.Dump();
	EngineOutput("\n");
	szPowerCommand.Kill();

	szReadCommand.Init();
	pcBoard->GenerateRead(&szReadCommand);
	USend(pcUART, szReadCommand.Text());
	szReadCommand.Dump();
	EngineOutput("\n");
	szReadCommand.Kill();

	szOutputCommand.Init();
	pcBoard->GenerateOutput(&szOutputCommand);
	USend(pcUART, szOutputCommand.Text());
	szOutputCommand.Dump();
	EngineOutput("\n");
	USend(pcUART, "W");
	USend(pcUART, szOutputCommand.Text());

	szWriteCommand.Init();
	pcBoard->GenerateWrite(&szWriteCommand);
	USend(pcUART, szWriteCommand.Text());
	szWriteCommand.Dump();
	EngineOutput("\n");
	szWriteCommand.Kill();

	szOutputCommand.Kill();

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool TestBoardAddressShit(void)
{
	CBoardPins	cBoard;
	CChars		szReadResult;
	CUART		cUART;
	CChars		szPrevRead;

	cBoard.Init();
	SetupAddressDecode(&cBoard);

	cUART.Init("COM3");
	if (!cUART.Open())
	{
		return false;
	}

	USend(&cUART, "POW");
	USend(&cUART, "PGb15a1");
	USend(&cUART, "OFFFFFFFFFFFFFF");
	USend(&cUART, "R0_6");
	EngineOutput("\n");

	for (;;)
	{
		if (!cUART.Send("W"))
		{
			return false;
		}
		szReadResult.Init();
		if (!cUART.Send("RR", &szReadResult))
		{
			return false;
		}
		szReadResult.Dump();
		szReadResult.Kill();
		szReadResult.Init();
		if (!cUART.Send("RR", &szReadResult))
		{
			return false;
		}
		szReadResult.Dump();
		szReadResult.Kill();
		szReadResult.Init();
		if (!cUART.Send("RR", &szReadResult))
		{
			return false;
		}
		szReadResult.Dump();
		szReadResult.Kill();
		if (!cUART.Send("WFFFFFFFFFFFFFF"))
		{
			return false;
		}
		szReadResult.Init();
		if (!cUART.Send("RR", &szReadResult))
		{
			return false;
		}
		szReadResult.Dump();
		szReadResult.Kill();
		szReadResult.Init();
		if (!cUART.Send("RR", &szReadResult))
		{
			return false;
		}
		szReadResult.Dump();
		szReadResult.Kill();
		szReadResult.Init();
		if (!cUART.Send("RR", &szReadResult))
		{
			return false;
		}
		szReadResult.Dump();
		szReadResult.Kill();
	}

	szPrevRead.Kill();

	cUART.Close();
	cUART.Kill();

	cBoard.Kill();

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool TestBoardLoop(char* szComPort)
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
	SetupAddressDecode(&cBoard);

	if (!SetupCommands(&cBoard, &cUART))
	{
		return false;
	}

	EngineOutput("\n");

	bSuccess = true;
	while (bSuccess)
	{
		bSuccess = TestAddressDecode(&cUART, &cBoard);
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

	bResult = TestBoardLoop("COM3");

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

