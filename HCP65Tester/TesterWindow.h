#ifndef __TESTER_WINDOW_H__
#define __TESTER_WINDOW_H__
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

