#ifndef __TESTER_WINDOW_DRAW_H__
#define __TESTER_WINDOW_DRAW_H__
#include "WindowLib/CanvasDraw.h"
#include "TesterWindowData.h"


class CTesterWindowDraw : public CCanvasDraw
{
CONSTRUCTABLE(CTesterWindowDraw);
DESTRUCTABLE(CTesterWindowDraw);
protected:
	STesterWindowData* mpcData;

public:
	void	Init(STesterWindowData* pcData);
	bool	Draw(Ptr<CCanvas> pCanvas) override;
};


#endif // __TESTER_WINDOW_DRAW_H__


