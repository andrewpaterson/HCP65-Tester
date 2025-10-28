#ifndef __TESTER_WINDOW_H__
#define __TESTER_WINDOW_H__
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


class CImageCelGroup;
class CTesterWindow : public CWindow
{
CONSTRUCTABLE(CTesterWindow)
protected:
	int64	miTime;
	int32	miX;

public:
	void	Init(char* szWindowTitle, CNativeWindowFactory* pcWindowFactory);
	void	Kill(void);

	void	Tick(int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds);
	void	CanvasChanged(CCanvas* pcNewCanvas);
};


#endif // __TESTER_WINDOW_H__

