// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSPickup.generated.h"

class USphereComponent;
class UDecalComponent;
class ATPSPowerupActor;

UCLASS()
class TP_PROJECT_API ATPSPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComponent;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<ATPSPowerupActor> PowerupClass;

	void Respawn();

	ATPSPowerupActor* PowerupInstance;

	UPROPERTY(EditDefaultsOnly, Category = "PickupActor")
	float CooldownDuration;

	FTimerHandle TH_RespawnTimer;

public:

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
