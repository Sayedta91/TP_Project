// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSPowerupActor.h"
#include "TimerManager.h"


// Sets default values
ATPSPowerupActor::ATPSPowerupActor()
{
	PowerupInterval = 0.0f;
	NumberOfTicks = 0;
}

// Called when the game starts or when spawned
void ATPSPowerupActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATPSPowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= NumberOfTicks)
	{
		OnExpired();
		// Clear timer
		GetWorldTimerManager().ClearTimer(TH_PowerUpTick);
	}
}

void ATPSPowerupActor::ActivatePowerup()
{
	OnActivated();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TH_PowerUpTick, this, &ATPSPowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else
	{
		OnTickPowerup();
	}
}
