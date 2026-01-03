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


class CBoardPins
{
protected:
	CMapIntString	mmPinNumbers;
	CMapStringInt	mmPinNames;
	CArrayBit		msOutputs;
	CArrayBit		msInverted;
	size			miNumPins;

	CArrayBit		msWriteValues;
	CArrayBit		msReadValues;

public:
	void	Init(void);
	void	Kill(void);

	bool	Add(size iPinNumber, char* szPinName, EPinDirection eDirection, EPinSignal eSignal);

	bool	Set(char* szPinName, bool bValue);
	bool	Set(size iPinNumber, bool bValue);

	void	GenerateOutput(CChars* psz);
	void	GenerateLeft(CArrayBit* psArray, CChars* psz);
	void	GenerateRight(CArrayBit* psArray, CChars* psz);
	void	GenerateWrite(CChars* psz);
	size	NumPins(void);
};


#endif // __BOARD_PINS_H__

