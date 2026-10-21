#include <windows.h>
#include "BaseLib/Logger.h"
#include "BaseLib/Operators.h"
#include "BaseLib/FastFunctions.h"
#include "BaseLib/FileUtil.h"
#include "BaseLib/TypeNames.h"
#include "BaseLib/TypeConverter.h"
#include "BaseLib/GlobalMemory.h"
#include "BaseLib/GlobalDataTypesIO.h"
#include "BaseLib/DebugOutput.h"
#include "StandardLib/Objects.h"
#include "InputLib/Input.h"
#include "WindowLib/Window.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/ComponentInput.h"
#include "WinGdiLib/WinGDIWindowFactory.h"
#include "WinInputLib/WinInput.h"
#include "TesterWindowTick.h"
#include "TesterWindowContent.h"

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
	OperatorsInit();
	MemoryInit();
	DataIOInit();
	ObjectsInit();

	bool bResult;

	bResult = true;
	//bResult = TestBoardLoop("COM3", 64);

	{
		CWinGDIWindowFactory	cNativeFactory;
		Ptr<CWindow>			pTesterWindow;
		Ptr<CTesterWindowTick>	pTick;
		STesterWindowData		sData;
		CTesterWindowContent	cContent;
		CInput					cInput;
		CWinInput				cWinInput;
		CComponentInput			cComponentInput;

		cNativeFactory.Init(&gcMemoryAllocator,
			hInstance,
			hPrevInstance,
			nCmdShow,
			"HCP65Tester");

		//Validating every object each time one changes is too slow for the text's glyphs.  They are validated once at the end.
		gcObjects.DisableValidation();

		//The window is in the root so that changing pointers into its components doesn't make the objects work out again which
		//of them are still on the stack.  Its tick is allocated, not on the stack, because stack objects can't be under the root.
		pTick = OMalloc<CTesterWindowTick>(&sData);
		pTesterWindow = OMalloc<CWindow>("HCP65 Board Tester", &cNativeFactory, pTick, OMalloc<CClearCanvasDraw>(Set32BitColour((uint8)0x1E, (uint8)0x1F, (uint8)0x22)), CFT_RGBX, CCO_BGR, CRGB_24bit, ARGB_8bit);
		ORoot()->Add(pTesterWindow);

		if (cContent.Init(pTesterWindow, "Resources/JetBrainsMonoNL-Regular.ttf", "Resources/PrideAndPrejudice.txt"))
		{
			//The devices are described by the XML so they are added after it is read.
			cInput.Init(NULL);
			cInput.Read("../../CommonLibraries/WinInputLib/xml");
			cWinInput.Init(NULL, false);
			cInput.AddNativeInput(&cWinInput);
			cNativeFactory.SetRawInput(WinRawInputCallback, cWinInput.GetRaw());

			cComponentInput.Init(&cInput, &pTesterWindow);
			cComponentInput.AddKeyboards();
			cComponentInput.AddMice();
			pTesterWindow->SetComponentInput(&cComponentInput);

			pTesterWindow->Show();

			pTesterWindow->SetComponentInput(NULL);
			cNativeFactory.SetRawInput(NULL, NULL);
			cComponentInput.Kill();
			cInput.Kill();
			cWinInput.Kill();
		}
		else
		{
			bResult = false;
		}

		cContent.Kill();
		ORoot()->Remove(pTesterWindow);
		pTesterWindow = NULL;
		pTick = NULL;
		ORoot()->RemoveAll();

		gcObjects.EnableValidation();
		gcObjects.ValidateObjectsConsistency();

		cNativeFactory.Kill();
	}


	ObjectsFlush();
	ObjectsKill();
	DataIOKill();
	MemoryKill();
	OperatorsKill();
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

