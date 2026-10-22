#include "BaseLib/Logger.h"
#include "BaseLib/Operators.h"
#include "BaseLib/FastFunctions.h"
#include "BaseLib/TypeNames.h"
#include "BaseLib/TypeConverter.h"
#include "BaseLib/GlobalMemory.h"
#include "BaseLib/GlobalDataTypesIO.h"
#include "StandardLib/Objects.h"
#include "InputLib/Input.h"
#include "WindowLib/Window.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/ComponentInput.h"
#include "X11Lib/X11WindowFactory.h"
#include "X11Lib/X11Input.h"
#include "TesterWindowTick.h"
#include "TesterWindowContent.h"


//////////////////////////////////////////////////////////////////////////
//
// The tester's window on Linux, without the board.  Run from the
// HCP65Tester directory, the resources and WinInputLib's XML are found
// relative to it.
//
//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	gcLogger.Init();
	FastFunctionsInit();
	TypesInit();
	TypeConverterInit();
	OperatorsInit();
	MemoryInit();
	DataIOInit();
	ObjectsInit();

	bool bResult;

	{
		CX11WindowFactory		cNativeFactory;
		Ptr<CWindow>			pTesterWindow;
		Ptr<CTesterWindowTick>	pTick;
		STesterWindowData		sData;
		CTesterWindowContent	cContent;
		CInput					cInput;
		CX11Input				cX11Input;
		CComponentInput			cComponentInput;

		bResult = cNativeFactory.Init(&gcMemoryAllocator, NULL);
		if (bResult)
		{
			//Validating every object each time one changes is too slow for the text's glyphs.  They are validated once at the end.
			gcObjects.DisableValidation();

			//The window is in the root so that changing pointers into its components doesn't make the objects work out again which
			//of them are still on the stack.  Its tick is allocated, not on the stack, because stack objects can't be under the root.
			pTick = OMalloc<CTesterWindowTick>(&sData);
			pTesterWindow = OMalloc<CWindow>("HCP65 Board Tester", &cNativeFactory, pTick, OMalloc<CClearCanvasDraw>(Set32BitColour((uint8)0x1E, (uint8)0x1F, (uint8)0x22)), CFT_RGBX, CCO_BGR, CRGB_24bit, ARGB_8bit);
			ORoot()->Add(pTesterWindow);

			if (cContent.Init(pTesterWindow, "Resources/InterVariable.ttf", "Resources/JetBrainsMonoNL-Regular.ttf", "Resources/PrideAndPrejudice.txt"))
			{
				//The devices are described by the XML so they are added after it is read.
				cInput.Init(NULL);
				cInput.Read("../../commonlibraries/WinInputLib/xml");
				cX11Input.Init();
				cInput.AddNativeInput(&cX11Input);
				cNativeFactory.SetInput(&cX11Input);

				cComponentInput.Init(&cInput, &pTesterWindow);
				cComponentInput.AddKeyboards();
				cComponentInput.AddMice();
				pTesterWindow->SetComponentInput(&cComponentInput);

				pTesterWindow->Show();

				pTesterWindow->SetComponentInput(NULL);
				cNativeFactory.SetInput(NULL);
				cComponentInput.Kill();
				cInput.Kill();
				cX11Input.Kill();
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
		}

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

	if (bResult)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

