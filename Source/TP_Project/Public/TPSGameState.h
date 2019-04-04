// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TPSGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8 {
	WaitingToStart,

	WaveInProgress,

	// No longer spawning new bots
	WaitingToComplete,

	WaveComplete,

	GameOver,
};

UCLASS()
class TP_PROJECT_API ATPSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:

	UFUNCTION()
	void OnWaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, Category = "GameState")
	EWaveState WaveState;

public:

	void SetWaveState(EWaveState NewState);
};
