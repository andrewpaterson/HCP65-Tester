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


void SetupAddressDecode(CBoardPins* pcBoard)
{
	pcBoard->Add( 1, "Register11", PD_Output, PS_Inverted);
	pcBoard->Add( 2, "Register10", PD_Output, PS_Inverted);
	//5V
	pcBoard->Add( 4, "Register9", PD_Output, PS_Inverted);
	pcBoard->Add( 5, "Register8", PD_Input, PS_Inverted);
	pcBoard->Add( 6, "Register7", PD_Output, PS_Inverted);
	pcBoard->Add( 7, "Register6", PD_Output, PS_Inverted);
	pcBoard->Add( 8, "Register5", PD_Output, PS_Inverted);
	//GND					  
	pcBoard->Add(10, "Register4", PD_Output, PS_Inverted);
	pcBoard->Add(11, "Register3", PD_Output, PS_Inverted);
	pcBoard->Add(12, "Register2", PD_Output, PS_Inverted);
	pcBoard->Add(13, "Register1", PD_Output, PS_Inverted);
	pcBoard->Add(14, "Register0", PD_Output, PS_Inverted);
	pcBoard->Add(15, "Device19", PD_Output, PS_Inverted);
	pcBoard->Add(16, "Device18", PD_Output, PS_Inverted);
	pcBoard->Add(17, "Device17", PD_Output, PS_Inverted);
	pcBoard->Add(18, "Device16", PD_Output, PS_Inverted);
	pcBoard->Add(19, "Device7",  PD_Output, PS_Inverted);
	//GND					  
	pcBoard->Add(21, "Device6", PD_Output, PS_Inverted);
	pcBoard->Add(22, "Device5", PD_Output, PS_Inverted);
	pcBoard->Add(23, "Device4", PD_Output, PS_Inverted);
	pcBoard->Add(24, "Device3", PD_Output, PS_Inverted);
	pcBoard->Add(25, "Device2", PD_Output, PS_Inverted);
	pcBoard->Add(26, "Reset", PD_Input, PS_Inverted);
	pcBoard->Add(27, "Device1", PD_Output, PS_Inverted);
	pcBoard->Add(28, "Registers", PD_Output, PS_Inverted);

	pcBoard->Add(29, "Devices", PD_Output, PS_Inverted);
	pcBoard->Add(30, "RAM Swap", PD_Input, PS_Normal);
	//5V
	pcBoard->Add(32, "RAM", PD_Output, PS_Inverted);
	pcBoard->Add(33, "ROM", PD_Output, PS_Inverted);
	//N.C.
	pcBoard->Add(35, "Kernal Mode", PD_Input, PS_Normal);
	//N.C.
	//GND					  
	pcBoard->Add(38, "Main Memory", PD_Output, PS_Inverted);
	pcBoard->Add(39, "A23", PD_Input, PS_Normal);
	pcBoard->Add(40, "A22", PD_Input, PS_Normal);
	pcBoard->Add(41, "A21", PD_Input, PS_Normal);
	pcBoard->Add(42, "A20", PD_Input, PS_Normal);
	pcBoard->Add(43, "A19", PD_Input, PS_Normal);
	pcBoard->Add(44, "A18", PD_Input, PS_Normal);
	pcBoard->Add(45, "A17", PD_Input, PS_Normal);
	pcBoard->Add(46, "A16", PD_Input, PS_Normal);
	pcBoard->Add(47, "A15", PD_Input, PS_Normal);
	//GND					  
	pcBoard->Add(49, "A3", PD_Input, PS_Normal);
	pcBoard->Add(50, "A2", PD_Input, PS_Normal);
	pcBoard->Add(51, "A1", PD_Input, PS_Normal);
	pcBoard->Add(52, "A0", PD_Input, PS_Normal);
	pcBoard->Add(53, "Register12", PD_Output, PS_Inverted);
	pcBoard->Add(54, "Register13", PD_Output, PS_Inverted);
	pcBoard->Add(55, "Register14", PD_Output, PS_Inverted);
	pcBoard->Add(56, "Register15", PD_Output, PS_Inverted);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void SetupDebugBoard(CBoardPins* pcBoard, size uiNumPins)
{
	size		i;

	uiNumPins = 64;
	for (i = 1; i <= uiNumPins; i++)
	{
		pcBoard->Add(i, SizeToString(i), PD_Input, PS_Normal);
	}
}


#define USend(c) 	if (!cUART.Send(c)) \
{ \
	return 1; \
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

	// Include "POW" command.  Reset Power, output and write
	// Include "RR" command.  Perform last read again.
	// Include "Echo" command.  For testing.
	// Include "Help" command.  For testing.

	CBoardPins	cBoard;
	CChars		szOutputCommand;
	CUART		cUART;
	size		i;

	cBoard.Init();
	SetupDebugBoard(&cBoard, 64);

	cUART.Init("COM4");
	if (!cUART.Open())
	{
		return 1;
	}


	USend("P");
	USend("O");
	USend("W");
	USend("PGb1");
	USend("P5a1");

	szOutputCommand.Init();
	cBoard.GenerateOutput(&szOutputCommand);
	USend(szOutputCommand.Text());
	szOutputCommand.Dump();
	EngineOutput("\n");
	USend("W");

	CChars	szWriteCommand;
	size	uiNumNybbles;
	CChars	szResponse;

	uiNumNybbles = cBoard.NumPins() / 4;
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
		USend(szWriteCommand.Text());
		szWriteCommand.Kill();

		szResponse.Init();
		cUART.Send("R0_7", &szResponse);
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
		USend(szWriteCommand.Text());
		szWriteCommand.Kill();

		szResponse.Init();
		cUART.Send("R0_7", &szResponse);
		szResponse.DumpKill();
	}

	szOutputCommand.Kill();

	cUART.Close();
	cUART.Kill();

	cBoard.Kill();

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

