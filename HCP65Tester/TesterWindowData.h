#ifndef __TESTER_WINDOW_DATA_H__
#define __TESTER_WINDOW_DATA_H__
#include "BaseLib/PrimitiveTypes.h"


//Dialogs are shown from the window's tick, not from the listener of the button or menu item that asked for them,
//because a modal dialog reads the input itself.
enum ETesterDialog
{
	TD_None,
	TD_Properties,
	TD_NewSubcircuit,
	TD_NewSimulation,
};


class CTesterWindowContent;
struct STesterWindowData
{
	CTesterWindowContent*	pcContent;
	ETesterDialog			eDialog;  //The dialog to show next tick.
};


#endif // __TESTER_WINDOW_DATA_H__

