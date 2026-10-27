#include "BaseLib/Logger.h"
#include "BaseLib/FastFunctions.h"
#include "BaseLib/TypeNames.h"
#include "BaseLib/TypeConverter.h"
#include "BaseLib/GlobalMemory.h"
#include "BaseLib/GlobalDataTypesIO.h"
#include "StandardLib/Objects.h"
#include "InputLib/Input.h"
#include "WindowLib/Window.h"
#include "WindowLib/ClearCanvasDraw.h"
#include "WindowLib/JewelColours.h"
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
		Ptr<CWindowInputSource>	pInputSource;
		CComponentInput			cComponentInput;

		bResult = cNativeFactory.Init(&gcMemoryAllocator, NULL);
		if (bResult)
		{
			//Validating every object each time one changes is too slow for the text's glyphs.  They are validated once at the end.
			gcObjects.DisableValidation();

			//The window is in the root so that changing pointers into its components doesn't make the objects work out again which
			//of them are still on the stack.  Its tick is allocated, not on the stack, because stack objects can't be under the root.
			pTick = OMalloc<CTesterWindowTick>(&sData);
			if (getenv("HCP65_SNAPSHOT") != NULL)
			{
				pTick->SetSnapshot(getenv("HCP65_SNAPSHOT"), (getenv("HCP65_SNAPSHOT_TICKS") != NULL) ? atoi(getenv("HCP65_SNAPSHOT_TICKS")) : 60);
			}
			pTesterWindow = OMalloc<CWindow>("HCP65 Board Tester", &cNativeFactory, pTick, OMalloc<CClearCanvasDraw>(JEWEL_PANEL), CFT_RGBX, CCO_BGR, CRGB_24bit, ARGB_8bit);
			ORoot()->Add(pTesterWindow);

			if (cContent.Init(pTesterWindow, &sData, "Resources/InterVariable.ttf", "Resources/Icons", "Resources/PropagationTimes.csv", "Resources/Fonts"))
			{
				//The devices are described by the XML so they are added after it is read.
				cInput.Init(NULL);
				cInput.Read("../../commonlibraries/WinInputLib/xml");
				cX11Input.Init();
				cInput.AddNativeInput(&cX11Input);
				cNativeFactory.SetInput(&cX11Input);

				pInputSource = OMalloc<CWindowInputSource>(&cInput);
				cComponentInput.Init(&pInputSource, &pTesterWindow);
				cComponentInput.AddKeyboards();
				cComponentInput.AddMice();
				pTesterWindow->SetComponentInput(&cComponentInput);

				pTesterWindow->SetWindowSize(1280, 800);
				pTesterWindow->Show();

				pTesterWindow->SetComponentInput(NULL);
				cNativeFactory.SetInput(NULL);
				cComponentInput.Kill();
				pInputSource = NULL;
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

