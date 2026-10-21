#ifndef __TESTER_BUTTON_LISTENER_H__
#define __TESTER_BUTTON_LISTENER_H__
#include "WindowLib/ButtonListener.h"


//Logs the caption of each button clicked.
class CTesterButtonListener : public CButtonListener
{
CONSTRUCTABLE(CTesterButtonListener);
public:
	void	ButtonClicked(CUnknown* pcSource, void* pvContext) override;
};


#endif // __TESTER_BUTTON_LISTENER_H__

