
#include "TPSGameState.h"

void ATPSGameState::OnWaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void ATPSGameState::SetWaveState(EWaveState NewState)
{
	EWaveState OldState = WaveState;
	WaveState = NewState;
	OnWaveState(OldState);
}
