#ifndef __TESTER_BUTTON_LISTENER_H__
#define __TESTER_BUTTON_LISTENER_H__
#include "WindowLib/ButtonListener.h"
#include "WindowLib/MenuListener.h"
#include "WindowLib/ListListener.h"


//The window content can't be every kind of listener itself, each is a CListener, so these pass what they hear on.
class CTesterWindowContent;


class CTesterButtonListener : public CButtonListener
{
CONSTRUCTABLE(CTesterButtonListener);
public:
	CTesterWindowContent*	mpcContent;

	void	Init(CTesterWindowContent* pcContent);
	void	ButtonClicked(CUnknown* pcSource, void* pvContext) override;
};


class CTesterMenuListener : public CMenuListener
{
CONSTRUCTABLE(CTesterMenuListener);
public:
	CTesterWindowContent*	mpcContent;

	void	Init(CTesterWindowContent* pcContent);
	void	MenuItemClicked(CUnknown* pcSource, void* pvContext) override;
};


class CTesterListListener : public CListListener
{
CONSTRUCTABLE(CTesterListListener);
public:
	CTesterWindowContent*	mpcContent;

	void	Init(CTesterWindowContent* pcContent);
	void	SelectionChanged(CUnknown* pcSource, void* pvContext) override;
};


#endif // __TESTER_BUTTON_LISTENER_H__

