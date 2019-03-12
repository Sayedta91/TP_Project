// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSPowerupActor.generated.h"

UCLASS()
class TP_PROJECT_API ATPSPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSPowerupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// time between ticks
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	float PowerupInterval;

	// total number of times we apply power up effect
	UPROPERTY(EditDefaultsOnly, Category = "Powerup")
	int32 NumberOfTicks;

	// total number of ticks applied
	int32 TicksProcessed;

	FTimerHandle TH_PowerUpTick;

	UFUNCTION()
	void OnTickPowerup();

public:	

	void ActivatePowerup();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerup")
	void OnExpired();

};
