// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TP_PROJECT_API ATPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	FTimerHandle TH_BotSpawn;

	// Number of bots to spawn in current wave
	int32 BotsToSpawn;

	int32 WaveCount;

protected:

	// Allow BP to spawn a single bot
	UFUNCTION(BlueprintImplmentableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	// Start spawning bots for wave
	void StartWave();

	// Stop spawning bots and end wave
	void EndWave();

	// set timer for next wave
	void PrepareNextWave();
};
