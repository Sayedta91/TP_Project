// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSPickup.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "TPSPowerupActor.h"
#include "TimerManager.h"

// Sets default values
ATPSPickup::ATPSPickup()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetSphereRadius(75.0f);
	RootComponent = SphereComponent;

	DecalComponent = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal Component"));
	DecalComponent->SetRelativeRotation(FRotator(90, 0.0f, 0.0f));
	DecalComponent->DecalSize = FVector(64, 75, 75);
	DecalComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATPSPickup::BeginPlay()
{
	Super::BeginPlay();
	
	Respawn();
}

void ATPSPickup::Respawn()
{
	if (PowerupClass == nullptr){ return; }

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance = GetWorld()->SpawnActor<ATPSPowerupActor>(PowerupClass, GetTransform(), SpawnParams);
}

void ATPSPickup::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Grant Powerup to Player
	if (PowerupInstance)
	{
		PowerupInstance->ActivatePowerup();
		PowerupInstance = nullptr;

		GetWorldTimerManager().SetTimer(TH_RespawnTimer, this, &ATPSPickup::Respawn, CooldownDuration);
	}

}


