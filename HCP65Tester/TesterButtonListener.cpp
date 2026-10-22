#include "BaseLib/Logger.h"
#include "WindowLib/Button.h"
#include "TesterButtonListener.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterButtonListener::ButtonClicked(CUnknown* pcSource, void* pvContext)
{
	CButton*	pcButton;

	pcButton = (CButton*)pcSource;
	gcLogger.Info2("Button [", pcButton->GetCaption(), "] clicked.", NULL);
}

