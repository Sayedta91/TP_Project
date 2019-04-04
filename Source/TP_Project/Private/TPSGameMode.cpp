// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "TPSHealthComponent.h"
#include "TPSPlayerState.h"
#include "TPSGameState.h"


ATPSGameMode::ATPSGameMode()
{
	TimeBetweenWaves = 2.0f;
	GameStateClass = ATPSGameState::StaticClass();
	PlayerStateClass = ATPSGameMode::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void ATPSGameMode::StartWave()
{
	WaveCount++;

	BotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TH_BotSpawn, this, &ATPSGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}

void ATPSGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TH_BotSpawn);
	SetWaveState(EWaveState::WaitingToComplete);
}

void ATPSGameMode::PrepareNextWave()
{
	GetWorldTimerManager().SetTimer(TH_NextWaveStart, this, &ATPSGameMode::StartWave, TimeBetweenWaves, false);
	SetWaveState(EWaveState::WaitingToStart);

}

void ATPSGameMode::CheckWaveState()
{
	bool bIsPrepForWave = GetWorldTimerManager().IsTimerActive(TH_NextWaveStart);

	if (BotsToSpawn > 0 || bIsPrepForWave)
	{
		return; 
	}

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator ItPawns = GetWorld()->GetPawnIterator(); ItPawns; ++ItPawns)
	{
		APawn* TestPawn = ItPawns->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		UTPSHealthComponent* HealthComponent = Cast<UTPSHealthComponent>(TestPawn->GetComponentByClass(UTPSHealthComponent::StaticClass()));
		if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	} 

	if (!bIsAnyBotAlive)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareNextWave();
	}
}

void ATPSGameMode::CheckPlayerState()
{
	for (FConstPlayerControllerIterator ItPlayer = GetWorld()->GetPlayerControllerIterator(); ItPlayer; ++ItPlayer)
	{
		APlayerController* PC = ItPlayer->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UTPSHealthComponent* HealthComponent = Cast<UTPSHealthComponent>(MyPawn->GetComponentByClass(UTPSHealthComponent::StaticClass()));
			if (ensure(HealthComponent) && HealthComponent->GetHealth() > 0.0f)
			{
				// Player is alive
				return;
			}
		}
	}
	
	// Player is dead
	GameOver();
}

void ATPSGameMode::GameOver()
{
	EndWave();
	SetWaveState(EWaveState::GameOver);
}

void ATPSGameMode::SetWaveState(EWaveState NewState)
{
	ATPSGameState* GS = GetGameState<ATPSGameState>();
	if (ensure(GS))
	{
		GS->SetWaveState(NewState);
	}
}

void ATPSGameMode::StartPlay()
{
	Super::StartPlay();
	PrepareNextWave();
}

void ATPSGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
	CheckPlayerState();
}

void ATPSGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	BotsToSpawn--;

	if (BotsToSpawn <=0)
	{
		EndWave();
	}
}
