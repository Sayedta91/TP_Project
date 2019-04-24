// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TPSGameMode.generated.h"

enum class EWaveState : uint8;

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

	void SetWaveState(EWaveState NewState);

private:

	UPROPERTY(EditAnywhere, Category = "GameMode")
	float TimeBetweenWaves;

	UPROPERTY(EditAnywhere, Category = "GameMode")
	int32 WaveCount;

public:

	ATPSGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds);

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
};
