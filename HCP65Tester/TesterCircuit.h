#ifndef __TESTER_CIRCUIT_H__
#define __TESTER_CIRCUIT_H__
#include "MicroprocessorLib/CircuitSimulation.h"
#include "MicroprocessorLib/FamilyVoltageConfigurationStore.h"
#include "LogicimLib/CircuitView.h"


//The circuit shown in the tester's editor until circuits can be loaded: a ring oscillator (a NAND enabled by a supply
//and two inverters) driving a buffer, and a row of every gate.  It runs in its own simulation.
class CTesterCircuit
{
protected:
	CFamilyVoltageConfigurationStore	mcFamilies;
	CCircuitSimulation					mcSimulation;
	CCircuitView						mcView;
	CCircuitColours						mcColours;
	CTraceView*							mpcEnable;
	bool								mbSimulation;

public:
	bool				Init(const char* szPropagationTimes);
	void				Kill(void);

	bool				Reset(void);
	void				Run(int64 iTimeForward);

	CCircuitView*		GetView(void);
	CCircuitColours*	GetColours(void);
	int64				GetTime(void);

protected:
	void				CreateViews(void);
	bool				CreateSimulation(void);
};


#endif // __TESTER_CIRCUIT_H__
