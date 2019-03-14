// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

UCLASS()
class TP_PROJECT_API ATPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	FTimerHandle TH_BotSpawn;

	FTimerHandle TH_NextWaveStart;

	// Number of bots to spawn in current wave
	int32 BotsToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

protected:

	// Allow BP to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	// Start spawning bots for wave
	void StartWave();

	// Stop spawning bots and end wave
	void EndWave();

	// set timer for next wave
	void PrepareNextWave();

	void CheckWaveState();

	void CheckPlayerState();

	void GameOver();

public:

	ATPSGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds);

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
};
