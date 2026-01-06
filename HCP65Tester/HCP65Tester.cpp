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
	pcBoard->SetName("Address Decode");
	pcBoard->SetVersion("V1");

	pcBoard->AddSignal( 1, "Register11", PD_Output, PS_Inverted);
	pcBoard->AddSignal( 2, "Register10", PD_Output, PS_Inverted);
	pcBoard->AddPower(  3, PP_5V);
	pcBoard->AddSignal( 4, "Register9", PD_Output, PS_Inverted);
	pcBoard->AddSignal( 5, "Register8", PD_Output, PS_Inverted);
	pcBoard->AddSignal( 6, "Register7", PD_Output, PS_Inverted);
	pcBoard->AddSignal( 7, "Register6", PD_Output, PS_Inverted);
	pcBoard->AddSignal( 8, "Register5", PD_Output, PS_Inverted);
	pcBoard->AddPower(  9, PP_Ground);
	pcBoard->AddSignal(10, "Register4", PD_Output, PS_Inverted);
	pcBoard->AddSignal(11, "Register3", PD_Output, PS_Inverted);
	pcBoard->AddSignal(12, "Register2", PD_Output, PS_Inverted);
	pcBoard->AddSignal(13, "Register1", PD_Output, PS_Inverted);
	pcBoard->AddSignal(14, "Register0", PD_Output, PS_Inverted);
	pcBoard->AddSignal(15, "Device19", PD_Output, PS_Inverted);
	pcBoard->AddSignal(16, "Device18", PD_Output, PS_Inverted);
	pcBoard->AddSignal(17, "Device17", PD_Output, PS_Inverted);
	pcBoard->AddSignal(18, "Device16", PD_Output, PS_Inverted);
	pcBoard->AddSignal(19, "Device7",  PD_Output, PS_Inverted);
	pcBoard->AddPower( 20, PP_Ground);
	pcBoard->AddSignal(21, "Device6", PD_Output, PS_Inverted);
	pcBoard->AddSignal(22, "Device5", PD_Output, PS_Inverted);
	pcBoard->AddSignal(23, "Device4", PD_Output, PS_Inverted);
	pcBoard->AddSignal(24, "Device3", PD_Output, PS_Inverted);
	pcBoard->AddSignal(25, "Device2", PD_Output, PS_Inverted);
	pcBoard->AddSignal(26, "Reset", PD_Input, PS_Inverted);
	pcBoard->AddSignal(27, "Device1", PD_Output, PS_Inverted);
	pcBoard->AddSignal(28, "Registers", PD_Output, PS_Inverted);

	pcBoard->AddSignal(29, "Devices", PD_Output, PS_Inverted);
	pcBoard->AddSignal(30, "RAM Swap", PD_Input, PS_Normal);
	pcBoard->AddPower( 31, PP_5V);
	pcBoard->AddSignal(32, "RAM", PD_Output, PS_Inverted);
	pcBoard->AddSignal(33, "ROM", PD_Output, PS_Inverted);
	pcBoard->AddNoCon( 34);
	pcBoard->AddSignal(35, "Kernal Mode", PD_Input, PS_Normal);
	pcBoard->AddNoCon( 36);
	pcBoard->AddPower( 37, PP_Ground);
	pcBoard->AddSignal(38, "Main Memory", PD_Output, PS_Inverted);
	pcBoard->AddSignal(39, "A23", PD_Input, PS_Normal);
	pcBoard->AddSignal(40, "A22", PD_Input, PS_Normal);
	pcBoard->AddSignal(41, "A21", PD_Input, PS_Normal);
	pcBoard->AddSignal(42, "A20", PD_Input, PS_Normal);
	pcBoard->AddSignal(43, "A19", PD_Input, PS_Normal);
	pcBoard->AddSignal(44, "A18", PD_Input, PS_Normal);
	pcBoard->AddSignal(45, "A17", PD_Input, PS_Normal);
	pcBoard->AddSignal(46, "A16", PD_Input, PS_Normal);
	pcBoard->AddSignal(47, "A15", PD_Input, PS_Normal);
	pcBoard->AddPower( 48, PP_Ground);
	pcBoard->AddSignal(49, "A3", PD_Input, PS_Normal);
	pcBoard->AddSignal(50, "A2", PD_Input, PS_Normal);
	pcBoard->AddSignal(51, "A1", PD_Input, PS_Normal);
	pcBoard->AddSignal(52, "A0", PD_Input, PS_Normal);
	pcBoard->AddSignal(53, "Register12", PD_Output, PS_Inverted);
	pcBoard->AddSignal(54, "Register13", PD_Output, PS_Inverted);
	pcBoard->AddSignal(55, "Register14", PD_Output, PS_Inverted);
	pcBoard->AddSignal(56, "Register15", PD_Output, PS_Inverted);

	pcBoard->GroupBus("Address",  "A0", 0);
	pcBoard->GroupBus("Address",  "A1", 1);
	pcBoard->GroupBus("Address",  "A2", 2);
	pcBoard->GroupBus("Address",  "A3", 3);
	pcBoard->GroupBus("Address", "A15", 15);
	pcBoard->GroupBus("Address", "A16", 16);
	pcBoard->GroupBus("Address", "A17", 17);
	pcBoard->GroupBus("Address", "A18", 18);
	pcBoard->GroupBus("Address", "A19", 19);
	pcBoard->GroupBus("Address", "A20", 20);
	pcBoard->GroupBus("Address", "A21", 21);
	pcBoard->GroupBus("Address", "A22", 22);
	pcBoard->GroupBus("Address", "A23", 23);

	pcBoard->Done();

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
		pcBoard->AddSignal(i, SizeToString(i), PD_Input, PS_Normal);
	}
}


#define USend(u, c) 	if (!(u)->Send(c)) \
{ \
	return 1; \
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
int TestDebugBoard(CBoardPins* pcBoard, CUART* pcUART)
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

	return 0;
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

	size		uiLoop;

	for (uiLoop = 0;; uiLoop++)
	{
		gcLogger.Info2("--------------------- Loop [", SizeToString(uiLoop), "] ----------------------\n", NULL);

		CBoardPins	cBoard;
		CChars		szOutputCommand;
		CChars		szWriteCommand;
		CChars		szPowerCommand;
		CChars		szReadCommand;
		CChars		szReadResult;
		CUART		cUART;
		CChars		szPrevRead;
		CChars		szRead;
		size		uiFlags;
		bool		bSuccess;

		cBoard.Init();
		SetupAddressDecode(&cBoard);

		cUART.Init("COM4");
		if (!cUART.Open())
		{
			return 1;
		}

		USend(&cUART, "POW");
		USend(&cUART, "PGb1");
		USend(&cUART, "P5a1");

		szPowerCommand.Init();
		cBoard.GeneratePower(&szPowerCommand);
		USend(&cUART, szPowerCommand.Text());
		szPowerCommand.Dump();
		EngineOutput("\n");
		szPowerCommand.Kill();

		szReadCommand.Init();
		cBoard.GenerateRead(&szReadCommand);
		USend(&cUART, szReadCommand.Text());
		szReadCommand.Dump();
		EngineOutput("\n");
		szReadCommand.Kill();

		szOutputCommand.Init();
		cBoard.GenerateOutput(&szOutputCommand);
		USend(&cUART, szOutputCommand.Text());
		szOutputCommand.Dump();
		EngineOutput("\n");
		USend(&cUART, "W");
		USend(&cUART, szOutputCommand.Text());

		szWriteCommand.Init();
		cBoard.GenerateWrite(&szWriteCommand);
		USend(&cUART, szWriteCommand.Text());
		szWriteCommand.Dump();
		EngineOutput("\n");
		szWriteCommand.Kill();

		EngineOutput("\n");

		szPrevRead.Init();
		size uiAddress;
		for (uiFlags = 0; uiFlags < 8; uiFlags++)
		{
			cBoard.Set("Reset", uiFlags & 4);
			cBoard.Set("Kernal Mode", uiFlags & 2);
			cBoard.Set("RAM Swap", uiFlags & 1);

			uiAddress = 0x10'0000;
			for (;;)
			{
				cBoard.SetBus("Address", uiAddress);
				szWriteCommand.Init();
				cBoard.GenerateWrite(&szWriteCommand);
				USend(&cUART, szWriteCommand.Text());
				szWriteCommand.Kill();

				szReadResult.Init();
				if (!cUART.Send("RR", &szReadResult))
				{
					return 1;
				}
				cBoard.UpdateRead(&szReadResult);
				szRead.Init();
				bSuccess = cBoard.PrintRead(&szRead, true);
				if (!szRead.Equals(&szPrevRead))
				{
					EngineOutput("===========================\n");
					szPrevRead.Set(&szRead);
					cBoard.DumpWrite(false);
					EngineOutput("\n");
					szRead.Dump();
					EngineOutput("===========================\n");
				}
				szRead.Kill();
				szReadResult.Kill();

				if (!bSuccess)
				{
					break;
				}

				if (uiAddress < 0x10'0010)
				{
					uiAddress += 0x0001;
				}
				else
				{
					if (uiAddress == 0x10'0010)
					{
						uiAddress = 0x10'8000;
					}
					else
					{
						uiAddress += 0x8000;
					}
				}
				if (uiAddress == 0x20'0000)
				{
					break;
				}
			}
			if (!bSuccess)
			{
				break;
			}
		}

		szOutputCommand.Kill();
		szPrevRead.Kill();

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
	}

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

