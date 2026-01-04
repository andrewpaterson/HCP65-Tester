#ifndef __BOARD_PINS_H__
#define __BOARD_PINS_H__
#include "BaseLib/MapIntString.h"
#include "BaseLib/MapStringInt.h"
#include "BaseLib/MapIntInt.h"
#include "BaseLib/ArrayBit.h"


enum EPinDirection
{
	PD_Unknown,
	PD_Input,
	PD_Output,
};


enum EPinSignal
{
	PS_Unknown,
	PS_Normal,
	PS_Inverted,
};

enum EPowerPin
{
	PP_Unknown,
	PP_Ground,
	PP_5V,
};


class CBusPins
{
protected:
	CMapIntInt	mmBusOffsets;

public:
	void	Init(void);
	void	Kill(void);

	int		Get(int iPinNumber);
	void	Set(int iPinNumber, int iBusOffset);
};


typedef CMapStringTemplate<CBusPins> CMapStringBusses;


class CBoardPins
{
protected:
	CChars				mszName;
	CChars				mszVersion;

	CMapIntString		mmPinSignals;
	CMapStringInt		mmPinNames;

	CArrayBit			msOutputs;
	CArrayBit			msInverted;

	CMapIntInt			mmPinPowers;
	CMapIntInt			mmPinNoConnects;

	CMapStringBusses	mmBusses;

	size				miNumPins;
	bool				mbDone;

	CArrayBit			msWriteValues;
	CArrayBit			msReadValues;

public:
	void	Init(void);
	void	Kill(void);

	bool	Done(void);

	void	SetName(char* szName);
	void	SetVersion(char* szVersion);

	bool	ValidateCanAdd(size iPinNumber, char* szPinName = NULL);
	bool	AddSignal(size iPinNumber, char* szPinName, EPinDirection eDirection, EPinSignal eSignal);
	bool	AddPower(size iPinNumber, EPowerPin ePower);
	bool	AddNoCon(size iPinNumber);

	bool	GroupBus(char* szBusName, char* szPinName, int iBusOffset);

	bool	Set(char* szPinName, bool bValue);
	bool	Set(size iPinNumber, bool bValue);

	void	GenerateOutput(CChars* psz);
	void	GenerateLeft(CArrayBit* psArray, CChars* psz);
	void	GenerateRight(CArrayBit* psArray, CChars* psz);
	void	GenerateWrite(CChars* psz);
	size	NumPins(void);
};


#endif // __BOARD_PINS_H__

