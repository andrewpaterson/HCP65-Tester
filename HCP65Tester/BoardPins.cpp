#include "BoardPins.h"


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::Init(void)
{
	mmPinNumbers.Init();
	mmPinNames.Init();
	msOutputs.Init();
	msInverted.Init();
	miNumPins = 0;

	msWriteValues.Init();
	msReadValues.Init();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CBoardPins::Kill(void)
{
	msReadValues.Kill();
	msWriteValues.Kill();
	msInverted.Kill();
	msOutputs.Kill();
	miNumPins = 0;
	mmPinNames.Kill();
	mmPinNumbers.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CBoardPins::Add(size iPinNumber, char* szPinName, EPinDirection eDirection, EPinSignal eSignal)
{
	if ((iPinNumber < 1) ||
		(iPinNumber > 72))
	{
		return false;
	}

	if (mmPinNames.HasKey(szPinName))
	{
		return false;
	}

	if (mmPinNumbers.HasKey(iPinNumber))
	{
		return false;
	}

	mmPinNames.Put(szPinName, iPinNumber);
	mmPinNumbers.Put(iPinNumber, szPinName);

	msOutputs.GrowTo(iPinNumber);
	msOutputs.Set(iPinNumber - 1, eDirection == PD_Output);

	msInverted.GrowTo(iPinNumber);
	msInverted.Set(iPinNumber - 1, eSignal== PS_Inverted);

	if (miNumPins < iPinNumber)
	{
		miNumPins = iPinNumber;
	}
	return true;
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

