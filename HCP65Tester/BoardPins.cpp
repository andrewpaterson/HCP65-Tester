#include "BaseLib/Logger.h"
#include "BaseLib/LogString.h"
#include "BoardPins.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::Init(void)
{
	mszName.Init();
	mszVersion.Init();

	mmPinSignals.Init();
	mmPinNames.Init();
	mmPinPowers.Init();
	mmPinNoConnects.Init();
	msOutputs.Init();
	msInverted.Init();

	miNumPins = 0;

	msWriteValues.Init();
	msReadValues.Init();

	mmBusses.Init();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::Kill(void)
{
	SMapIterator	sIter;
	bool			bHasNext;
	CBusPins*		pcBus;

	bHasNext = mmBusses.StartIteration(&sIter, NULL, NULL, (void**)&pcBus, NULL);
	while (bHasNext)
	{
		pcBus->Kill();
		bHasNext = mmBusses.Iterate(&sIter, NULL, NULL, (void**)&pcBus, NULL);
	}
	mmBusses.Kill();
		
	msReadValues.Kill();
	msWriteValues.Kill();

	msInverted.Kill();
	msOutputs.Kill();
	mmPinNoConnects.Kill();
	mmPinPowers.Kill();
	mmPinNames.Kill();
	mmPinSignals.Kill();

	mszVersion.Kill();
	mszName.Kill();

	miNumPins = 0;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CBoardPins::ValidateCanAdd(size iPinNumber, char* szPinName)
{
	if (iPinNumber < 1)
	{
		return gcLogger.Error2(__METHOD__, " Cannot add Pin numbered [", SizeToString(iPinNumber), "].  It must be greater than zero.", NULL);
	}
	else if (iPinNumber > 128)
	{
		return gcLogger.Error2(__METHOD__, " Cannot add Pin numbered [", SizeToString(iPinNumber), "].  It must be less than 129.", NULL);
	}

	if (szPinName)
	{
		if (StrEmpty(szPinName))
		{
			return gcLogger.Error2(__METHOD__, " Cannot add an unamed Pin.", NULL);
		}
		if (mmPinNames.HasKey(szPinName))
		{
			return gcLogger.Error2(__METHOD__, " Cannot add Pin named [", szPinName, "].  It already exists.", NULL);
		}
	}

	if (mmPinSignals.HasKey(iPinNumber))
	{
		return gcLogger.Error2(__METHOD__, " Cannot add Pin numbered [", SizeToString(iPinNumber), "].  It already exists as a signal.", NULL);
	}
	if (mmPinPowers.HasKey(iPinNumber))
	{
		return gcLogger.Error2(__METHOD__, " Cannot add Pin numbered [", SizeToString(iPinNumber), "].  It already exists as power.", NULL);
	}
	if (mmPinNoConnects.HasKey(iPinNumber))
	{
		return gcLogger.Error2(__METHOD__, " Cannot add Pin numbered [", SizeToString(iPinNumber), "].  It already exists as a no connect.", NULL);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CBoardPins::AddSignal(size iPinNumber, char* szPinName, EPinDirection eDirection, EPinSignal eSignal)
{
	if (!ValidateCanAdd(iPinNumber, szPinName))
	{
		return false;
	}

	mmPinNames.Put(szPinName, iPinNumber);
	mmPinSignals.Put(iPinNumber, szPinName);

	msOutputs.GrowTo(iPinNumber);
	msOutputs.Set(iPinNumber - 1, eDirection == PD_Output);

	msInverted.GrowTo(iPinNumber);
	msInverted.Set(iPinNumber - 1, eSignal == PS_Inverted);

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CBoardPins::AddPower(size iPinNumber, EPowerPin ePower)
{
	if (!ValidateCanAdd(iPinNumber))
	{
		return false;
	}

	mmPinPowers.Put(iPinNumber, (int)ePower);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CBoardPins::AddNoCon(size iPinNumber)
{
	if (!ValidateCanAdd(iPinNumber))
	{
		return false;
	}

	mmPinNoConnects.Put(iPinNumber, 0);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CBoardPins::GroupBus(char* szBusName, char* szPinName, int iBusOffset)
{
	CBusPins*	pcBusPins;
	int			iPinNumber;
	int			iExistingOffset;

	pcBusPins = mmBusses.Get(szBusName);
	if (!pcBusPins)
	{
		pcBusPins = mmBusses.Put(szBusName);
		pcBusPins->Init();
	}

	iPinNumber = (int)mmPinNames.Get(szPinName);
	if (iPinNumber == -1)
	{
		return gcLogger.Error2(__METHOD__, " Cannot add Pin named [", szPinName, "] into Bus [", szBusName, "].  Pin does not exist.", NULL);
	}

	iExistingOffset = pcBusPins->Get(iPinNumber);
	if (iExistingOffset != -1)
	{
		return gcLogger.Error2(__METHOD__, " Cannot add Pin named [", szPinName, "] into Bus [", szBusName, "].  It already has an offset.", NULL);
	}

	pcBusPins->Set(iPinNumber, iBusOffset);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CBoardPins::Done(void)
{
	SMapIterator	sIter;
	bool			bHasNext;
	int*			piPinNumber;	

	miNumPins = 0;

	bHasNext = mmPinSignals.StartIteration(&sIter, (void**)&piPinNumber, NULL, NULL, NULL);
	while (bHasNext)
	{
		if (miNumPins < (size)(*piPinNumber))
		{
			miNumPins = (size)(*piPinNumber);
		}
		bHasNext = mmPinSignals.Iterate(&sIter, (void**)&piPinNumber, NULL, NULL, NULL);
	}

	bHasNext = mmPinPowers.StartIteration(&sIter, (void**)&piPinNumber, NULL, NULL, NULL);
	while (bHasNext)
	{
		if (miNumPins < (size)(*piPinNumber))
		{
			miNumPins = (size)(*piPinNumber);
		}
		bHasNext = mmPinPowers.Iterate(&sIter, (void**)&piPinNumber, NULL, NULL, NULL);
	}

	bHasNext = mmPinNoConnects.StartIteration(&sIter, (void**)&piPinNumber, NULL, NULL, NULL);
	while (bHasNext)
	{
		if (miNumPins < (size)(*piPinNumber))
		{
			miNumPins = (size)(*piPinNumber);
		}
		bHasNext = mmPinNoConnects.Iterate(&sIter, (void**)&piPinNumber, NULL, NULL, NULL);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::SetName(char* szName)
{
	mszName.Set(szName);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::SetVersion(char* szVersion)
{
	mszVersion.Set(szVersion);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CBoardPins::Set(char* szPinName, bool bValue)
{
	int32	iPin;

	iPin = mmPinNames.Get(szPinName);
	if (iPin == -1)
	{
		return false;
	}

	return Set(iPin, bValue);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CBoardPins::Set(size iPinNumber, bool bValue)
{
	if ((iPinNumber < 1) ||
		(iPinNumber > miNumPins))
	{
		return false;
	}

	msWriteValues.GrowTo(iPinNumber);
	msWriteValues.Set(iPinNumber - 1, bValue);
	return true;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::GenerateLeft(CArrayBit* psArray, CChars* psz)
{
	size	ui;
	bool	bValue;
	char	c;
	uint8	uiValue;
	size	iCount;
	size	uiHalfPins;

	uiHalfPins = miNumPins / 2;
	iCount = 0;
	for (ui = uiHalfPins-1; ui != SIZE_MAX ; ui--)
	{
		bValue = !psArray->Get(ui);

		if (iCount == 0)
		{
			uiValue = 0;
		}
		if (iCount <= 3)
		{
			if (bValue)
			{
				uiValue |= 1 << (3 - iCount);
			}
		}
		if (iCount == 3)
		{
			c = GetHexChar(uiValue);
			c = ToUpper(c);
			psz->Append(c);
			iCount = 0;
		}
		else
		{
			iCount++;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::GenerateRight(CArrayBit* psArray, CChars* psz)
{
	size	ui;
	bool	bValue;
	char	c;
	uint8	uiValue;
	int		iCount;
	size	uiHalfPins;

	uiHalfPins = miNumPins / 2;
	iCount = 0;
	for (ui = uiHalfPins; ui < miNumPins; ui++)
	{
		bValue = !psArray->Get(ui);

		if (iCount == 0)
		{
			uiValue = 0;
		}
		if (iCount <= 3)
		{
			if (bValue)
			{
				uiValue |= 1 << (3 - iCount);
			}
		}
		if (iCount == 3)
		{
			c = GetHexChar(uiValue);
			c = ToUpper(c);
			psz->Append(c);
			iCount = 0;
		}
		else
		{
			iCount++;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::GenerateOutput(CChars* psz)
{
	CChars	szLeft;
	CChars	szRight;

	szRight.Init();
	szLeft.Init();

	GenerateLeft(&msOutputs, &szLeft);
	GenerateRight(&msOutputs, &szRight);

	psz->Append('O');
	psz->Append(&szRight);
	psz->Append(&szLeft);

	szLeft.Kill();
	szRight.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::GenerateWrite(CChars* psz)
{
	CChars	szLeft;
	CChars	szRight;

	szRight.Init();
	szLeft.Init();

	GenerateLeft(&msWriteValues, &szLeft);
	GenerateRight(&msWriteValues, &szRight);

	psz->Append('W');
	psz->Append(&szRight);
	psz->Append(&szLeft);

	szLeft.Kill();
	szRight.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
size CBoardPins::NumPins(void)
{
	return miNumPins;
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBusPins::Init(void)
{
	mmBusOffsets.Init();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBusPins::Kill(void)
{
	mmBusOffsets.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
int CBusPins::Get(int iPinNumber)
{
	int* piBusOffset;

	piBusOffset = mmBusOffsets.Get(iPinNumber);
	if (piBusOffset)
	{
		return *piBusOffset;
	}
	else
	{
		return -1;
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBusPins::Set(int iPinNumber, int iBusOffset)
{
	mmBusOffsets.Put(iPinNumber, iBusOffset);
}

