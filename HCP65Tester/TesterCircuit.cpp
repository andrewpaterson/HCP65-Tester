#include "BaseLib/ConstructorCall.h"
#include "BaseLib/Logger.h"
#include "MicroprocessorLib/PowerSource.h"
#include "MicroprocessorLib/PowerOutPort.h"
#include "MicroprocessorLib/Timeline.h"
#include "LogicimLib/AndGateView.h"
#include "LogicimLib/OrGateView.h"
#include "LogicimLib/BufferView.h"
#include "LogicimLib/ValueView.h"
#include "StandardLib/Objects.h"
#include "TesterCircuit.h"


#define TESTER_FAMILY	"LVC"


//////////////////////////////////////////////////////////////////////////
//
// Kill must be called even when this fails.  The circuit's text is
// coloured in eColourOrder, the canvas's.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterCircuit::Init(const char* szPropagationTimes, const char* szFontDirectory, EColourOrder eColourOrder)
{
	Ptr<CFontFamily>	pFamily;
	bool				bFonts;

	mbSimulation = false;
	mpcEnable = NULL;
	mcColours.InitDark();
	mcFamilies.Init();
	mcView.Init();

	//Nothing else holds the font families, so they are rooted.
	bFonts = LoadCircuitFonts(&mcFonts, szFontDirectory, eColourOrder);
	pFamily = mcFonts.GetSansSerif();
	if (pFamily.IsNotNull())
	{
		ORoot()->Add(pFamily);
	}
	pFamily = mcFonts.GetMonospaced();
	if (pFamily.IsNotNull())
	{
		ORoot()->Add(pFamily);
	}
	if (!bFonts || !mcFamilies.Load(szPropagationTimes))
	{
		return false;
	}

	CreateViews();
	return CreateSimulation();
}


//////////////////////////////////////////////////////////////////////////
//
// The views go before the simulation they point into.
//
//////////////////////////////////////////////////////////////////////////
void CTesterCircuit::Kill(void)
{
	mcView.Kill();
	if (mbSimulation)
	{
		mcSimulation.Kill();
		mbSimulation = false;
	}
	mcFamilies.Kill();
	mcColours.Kill();
	RemoveFontFamily(mcFonts.GetSansSerif());
	RemoveFontFamily(mcFonts.GetMonospaced());
	mcFonts.Kill();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterCircuit::RemoveFontFamily(Ptr<CFontFamily> pFamily)
{
	if (pFamily.IsNotNull())
	{
		ORoot()->Remove(pFamily);
	}
}


//////////////////////////////////////////////////////////////////////////
//
// The NAND's output runs through two inverters and back to its lower
// input, so it oscillates while its upper input is high.  A probe shows
// the buffered output.  Below are the other gates, unconnected.
//
//////////////////////////////////////////////////////////////////////////
void CTesterCircuit::CreateViews(void)
{
	CNandGateView*		pcNand;
	CInverterView*		pcInverter;
	CBufferView*		pcBuffer;
	CAndGateView*		pcAnd;
	COrGateView*		pcOr;
	CNorGateView*		pcNor;
	CXorGateView*		pcXor;
	CXnorGateView*		pcXnor;
	CProbeView*			pcProbe;

	pcNand = NewMalloc<CNandGateView>();
	pcNand->Init(-17, 0, ROT_West, "U1", TESTER_FAMILY, false, 2, 1);
	mcView.Add(pcNand);
	pcInverter = NewMalloc<CInverterView>();
	pcInverter->Init(-11, 0, ROT_West, "U2", TESTER_FAMILY, false, 1, 1);
	mcView.Add(pcInverter);
	pcInverter = NewMalloc<CInverterView>();
	pcInverter->Init(-6, 0, ROT_West, "U3", TESTER_FAMILY, false, 1, 1);
	mcView.Add(pcInverter);
	pcBuffer = NewMalloc<CBufferView>();
	pcBuffer->Init(2, 0, ROT_West, "U4", TESTER_FAMILY, false, 1, 1);
	mcView.Add(pcBuffer);

	mpcEnable = mcView.AddTrace(-25, -1, -18, -1);
	mcView.AddTrace(-14, 0, -12, 0);
	mcView.AddTrace(-9, 0, -7, 0);
	mcView.AddTrace(-4, 0, 1, 0);
	mcView.AddTrace(-1, 0, -1, 4);
	mcView.AddTrace(-1, 4, -20, 4);
	mcView.AddTrace(-20, 4, -20, 1);
	mcView.AddTrace(-20, 1, -18, 1);
	mcView.AddTrace(3, 0, 9, 0);
	pcProbe = NewMalloc<CProbeView>();
	pcProbe->Init(9, 0, ROT_East, "", TESTER_FAMILY, false, 1, RADIX_Binary, &mcFonts);
	mcView.Add(pcProbe);

	pcAnd = NewMalloc<CAndGateView>();
	pcAnd->Init(-24, 12, ROT_West, "", TESTER_FAMILY, false, 2, 1);
	mcView.Add(pcAnd);
	pcOr = NewMalloc<COrGateView>();
	pcOr->Init(-16, 12, ROT_West, "", TESTER_FAMILY, false, 3, 1);
	mcView.Add(pcOr);
	pcNor = NewMalloc<CNorGateView>();
	pcNor->Init(-8, 12, ROT_West, "", TESTER_FAMILY, false, 2, 1);
	mcView.Add(pcNor);
	pcXor = NewMalloc<CXorGateView>();
	pcXor->Init(0, 12, ROT_West, "", TESTER_FAMILY, false, 2, 1);
	mcView.Add(pcXor);
	pcXnor = NewMalloc<CXnorGateView>();
	pcXnor->Init(8, 12, ROT_West, "", TESTER_FAMILY, false, 2, 1);
	mcView.Add(pcXnor);
	pcBuffer = NewMalloc<CBufferView>();
	pcBuffer->Init(16, 12, ROT_North, "", TESTER_FAMILY, false, 2, 1);
	mcView.Add(pcBuffer);
}


//////////////////////////////////////////////////////////////////////////
//
// Gates only act on transitions and every wire of the ring starts
// undriven, which a NAND can't decide on with its enable high.  So the
// enable is wired low first, which drives the ring to a known level, and
// then switched high, which starts it oscillating.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterCircuit::CreateSimulation(void)
{
	CPowerSource*			pcLow;
	CPowerSource*			pcHigh;
	CVoltageConfiguration*	pcVoltage;
	CTimeline*				pcTimeline;

	mcSimulation.Init("Top Top 1");
	mbSimulation = true;
	if (!mcView.CreateSimulation(&mcSimulation, &mcFamilies))
	{
		return false;
	}
	pcTimeline = mcSimulation.GetTimeline();

	pcLow = NewMalloc<CPowerSource>();
	pcLow->Init(mcSimulation.GetSubcircuitTopSimulation(), "Disable", 0.0f);
	if (!pcLow->GetPowerOutPort()->ConnectLive(mpcEnable->GetTrace(0), pcTimeline))
	{
		return false;
	}
	pcTimeline->Run();
	pcLow->GetPowerOutPort()->Disconnect(pcTimeline);

	pcVoltage = mcFamilies.Get(TESTER_FAMILY)->GetDefaultVoltageConfiguration(DEFAULT_LOGIC_LEVEL);
	pcHigh = NewMalloc<CPowerSource>();
	pcHigh->Init(mcSimulation.GetSubcircuitTopSimulation(), "Enable", pcVoltage->GetVcc());
	return pcHigh->GetPowerOutPort()->ConnectLive(mpcEnable->GetTrace(0), pcTimeline);
}


//////////////////////////////////////////////////////////////////////////
//
// Starts the simulation again from nothing.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterCircuit::Reset(void)
{
	mcView.DisconnectSimulation();
	if (mbSimulation)
	{
		mcSimulation.Kill();
		mbSimulation = false;
	}
	return CreateSimulation();
}


//////////////////////////////////////////////////////////////////////////
//
// iTimeForward is in Logicim's time units (1/1024 ns).
//
//////////////////////////////////////////////////////////////////////////
void CTesterCircuit::Run(int64 iTimeForward)
{
	if (mbSimulation)
	{
		mcSimulation.RunToTime(iTimeForward);
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
CCircuitView* CTesterCircuit::GetView(void) { return &mcView; }
CCircuitColours* CTesterCircuit::GetColours(void) { return &mcColours; }
CCircuitFonts* CTesterCircuit::GetFonts(void) { return &mcFonts; }
int64 CTesterCircuit::GetTime(void) { return mbSimulation ? mcSimulation.GetTime() : -1; }

