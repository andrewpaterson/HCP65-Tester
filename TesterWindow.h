#ifndef __GAME_WINDOW_H__
#define __GAME_WINDOW_H__
#include <time.h>
#include "BaseLib/Define.h"
#include "BaseLib/Logger.h"
#include "BaseLib/Numbers.h"
#include "BaseLib/FileUtil.h"
#include "BaseLib/CSVHelper.h"
#include "BaseLib/Version.h"
#include "BaseLib/DiskFile.h"
#include "BaseLib/BufferedFile.h"
#include "BaseLib/Numbers.h"
#include "BaseLib/MD5.h"
#include "BaseLib/TextParser.h"
#include "BaseLib/WinInit.h"
#include "BaseLib/WinConfig.h"
#include "BaseLib/PropertiesFile.h"
#include "WindowLib/Window.h"
#include "WindowLib/ComponentFactory.h"


class CImageCelGroup;
class CTesterWindow : public CWindow
{
CONSTRUCTABLE(CTesterWindow)
public:
	void 				Init(HINSTANCE hInst, char* szName, CInput* pcInput);
	void				Kill(void);
	void 				ActiveTick(void);
	void				InputEvent(CUnknown* pcSource, void* pvContext);
};


#endif // __GAME_WINDOW_H__

