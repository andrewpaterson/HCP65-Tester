#ifndef __TESTER_WINDOW_TICK_H__
#define __TESTER_WINDOW_TICK_H__
#include "WindowLib/WindowTick.h"
#include "TesterWindowData.h"


class CTesterWindowTick : public CWindowTick
{
CONSTRUCTABLE(CTesterWindowTick);
DESTRUCTABLE(CTesterWindowTick);
protected:
	STesterWindowData*	mpcData;
	CChars				mszSnapshot;
	int					miSnapshotTicks;

public:
	void	Init(STesterWindowData* pcData);
	void	Class(void) override;
	void	Free(void) override;
	void	SetSnapshot(const char* szFilename, int iTicks);
	void	Tick(Ptr<CWindow> pWindow, int64 iUpdateTimeInMillieconds, int64 iTotalTimeInMillieconds) override;
};


#endif // __TESTER_WINDOW_TICK_H__

