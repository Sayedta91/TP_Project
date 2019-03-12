// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSGameMode.h"
#include "TimerManager.h"


void ATPSGameMode::StartWave()
{
	WaveCount++;

	BotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TH_BotSpawn, this, &ATPSGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
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
