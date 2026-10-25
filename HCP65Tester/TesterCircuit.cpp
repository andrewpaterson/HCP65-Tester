#include "StandardLib/Objects.h"
#include "TesterCircuit.h"


#define TESTER_MAIN_CIRCUIT		"Main"


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

	mcColours.InitDark();
	mcFamilies.Init();
	mcEditor.Init(&mcFamilies, &mcFonts);

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

	mcEditor.AddCircuit(TESTER_MAIN_CIRCUIT);
	return CreateSimulation();
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
void CTesterCircuit::Kill(void)
{
	mcEditor.Kill();
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
// Simulates the circuit being edited afresh.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterCircuit::CreateSimulation(void)
{
	return mcEditor.Simulate(mcEditor.GetCircuit() != NULL ? mcEditor.GetCircuit()->GetTypeName() : TESTER_MAIN_CIRCUIT);
}


//////////////////////////////////////////////////////////////////////////
//
// Starts the simulation again from nothing.
//
//////////////////////////////////////////////////////////////////////////
bool CTesterCircuit::Reset(void)
{
	return CreateSimulation();
}


//////////////////////////////////////////////////////////////////////////
//
// iTimeForward is in Logicim's time units (1/1024 ns).  Nothing runs
// while an edit is in progress.
//
//////////////////////////////////////////////////////////////////////////
void CTesterCircuit::Run(int64 iTimeForward)
{
	if ((mcEditor.GetSimulation() != NULL) && mcEditor.CanRun())
	{
		mcEditor.GetSimulation()->RunToTime(iTimeForward);
	}
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CTesterCircuit::Save(const char* szFilename)
{
	return mcEditor.Save(szFilename);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
bool CTesterCircuit::Load(const char* szFilename)
{
	return mcEditor.Load(szFilename);
}


//////////////////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////
CCircuitEditor* CTesterCircuit::GetEditor(void) { return &mcEditor; }
CCircuitColours* CTesterCircuit::GetColours(void) { return &mcColours; }
CCircuitFonts* CTesterCircuit::GetFonts(void) { return &mcFonts; }
int64 CTesterCircuit::GetTime(void) { return (mcEditor.GetSimulation() != NULL) ? mcEditor.GetSimulation()->GetTime() : -1; }
