#include "BaseLib/Logger.h"
#include "BaseLib/TextFile.h"
#include "StandardLib/Objects.h"
#include "SupportLib/ColourARGB32.h"
#include "SupportLib/ImageColour.h"
#include "SupportLib/ImageColourConverter.h"
#include "SupportLib/TrueTypeFont.h"
#include "SupportLib/TrueTypeFontFactory.h"
#include "WindowLib/JewelButtonImages.h"
#include "WindowLib/ScrollBarImages.h"
#include "TesterWindowContent.h"


//The glyphs generated: printable ASCII and the curly quotes the text uses.
#define TESTER_FONT_CHARACTERS	" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\xE2\x80\x98\xE2\x80\x99\xE2\x80\x9C\xE2\x80\x9D"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CTesterWindowContent::Init(Ptr<CWindow> pWindow, const char* szFontFilename, const char* szTextFilename)
{
	CChars					szText;
	CColourFormatHelper		cFormat;
	Ptr<CButtonImages>		pImages;
	Ptr<CButtonImages>		pDefaultImages;

	pWindow->GetColourFormat(&cFormat);
	mpFont = GenerateFont(szFontFilename, 14.0f, cFormat.meOrder);
	if (mpFont.IsNull())
	{
		return false;
	}

	if (!ReadText(szTextFilename, &szText))
	{
		mpFont = NULL;
		return false;
	}

	mpGrid = OMalloc<CGridContainer>(pWindow);
	pWindow->SetContainer(mpGrid);

	//A light translucent thumb like IntelliJ IDEA's dark theme.
	mpScrollPane = OMalloc<CScrollPane>(pWindow, OMalloc<CScrollBarImages>(8, Set32BitColour((uint8)200, (uint8)200, (uint8)200, (uint8)128), &cFormat));
	mpTextArea = OMalloc<CTextArea>(pWindow, mpFont, "");
	mpTextArea->SetWordWrap(false);
	mpScrollPane->SetContent(mpTextArea);
	mpGrid->AddComponent(mpScrollPane, 0, 0, 1, 1, 1, 1);

	//Each glyph is a sprite object.  The text is only laid out once the text area can be reached from the root through
	//the window, otherwise every sprite placed would make the objects work out again which of them are still on the stack.
	mpTextArea->SetText(szText.Text());
	szText.Kill();

	pImages = CJewelButtonImages::CreateButtonImages(&cFormat);
	pDefaultImages = CJewelButtonImages::CreateDefaultButtonImages(&cFormat);
	mpOK = OMalloc<CButton>(pWindow, pImages, pDefaultImages, mpFont, "OK");
	mpCancel = OMalloc<CButton>(pWindow, pImages, pDefaultImages, mpFont, "Cancel");
	mpOK->AddListener(&mcListener);
	mpCancel->AddListener(&mcListener);
	pWindow->SetDefaultButton(mpOK);

	//The bar is stretched across the window so that its buttons can be aligned to the right.
	mpButtonBar = OMalloc<CButtonBar>(pWindow);
	mpButtonBar->SetAlignment(CSH_Stretch, CSV_Center);
	mpButtonBar->AddComponent(mpOK);
	mpButtonBar->AddComponent(mpCancel);
	mpGrid->AddComponent(mpButtonBar, 0, 1);

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterWindowContent::Kill(void)
{
	if (mpOK.IsNotNull())
	{
		mpOK->RemoveListener(&mcListener);
		mpCancel->RemoveListener(&mcListener);
	}

	mpOK = NULL;
	mpCancel = NULL;
	mpButtonBar = NULL;
	mpTextArea = NULL;
	mpScrollPane = NULL;
	mpGrid = NULL;
	mpFont = NULL;
}


//////////////////////////////////////////////////////////////////////////
//
// Rasterises the glyphs from the TrueType font into an image in the
// window's colour order and makes a font from them.
//
//////////////////////////////////////////////////////////////////////////
Ptr<CFont> CTesterWindowContent::GenerateFont(const char* szFontFilename, float32 fPixelSize, EColourOrder eOrder)
{
	CTrueTypeFont			cTrueTypeFont;
	CTrueTypeFontFactory	cFactory;
	CImageColourRGB			cColour;
	CChars					szCharacters;
	Ptr<CFontDescription>	pDescription;
	Ptr<CImage>				pImage;
	Ptr<CFont>				pFont;

	cTrueTypeFont.Init();
	if (!cTrueTypeFont.Load((char*)szFontFilename))
	{
		cTrueTypeFont.Kill();
		gcLogger.Error2(__METHOD__, " Could not load font [", szFontFilename, "].", NULL);
		return NULL;
	}

	//The light grey of IntelliJ IDEA's dark theme text.
	cColour.Init(0xDF / 255.0f, 0xE1 / 255.0f, 0xE5 / 255.0f);
	szCharacters.Init(TESTER_FONT_CHARACTERS);
	cFactory.Init(&cTrueTypeFont, fPixelSize);
	pDescription = cFactory.Describe((char*)"JetBrains Mono 10px", szCharacters.Text());
	pImage = cFactory.Rasterise(pDescription, &cColour);
	if (pImage.IsNotNull())
	{
		if (pImage->GetColourOrder() != eOrder)
		{
			pImage = CImageColourConverter::ConvertColour(pImage, eOrder);
		}
		pFont = OMalloc<CFont>(pDescription, pImage);
	}
	cFactory.Kill();
	szCharacters.Kill();
	cTrueTypeFont.Kill();

	if (pFont.IsNull())
	{
		gcLogger.Error2(__METHOD__, " Could not make a font from [", szFontFilename, "].", NULL);
	}
	return pFont;
}


//////////////////////////////////////////////////////////////////////////
//
// Line feeds start new lines so carriage returns are removed.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterWindowContent::ReadText(const char* szTextFilename, CChars* pszText)
{
	CTextFile	cTextFile;

	cTextFile.Init();
	if (!cTextFile.Read(szTextFilename))
	{
		cTextFile.Kill();
		return gcLogger.Error2(__METHOD__, " Could not read text [", szTextFilename, "].", NULL);
	}

	pszText->Init(cTextFile.Text());
	pszText->Replace("\r", "");
	cTextFile.Kill();
	return true;
}

