// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSHealthComponent.h"
#include "Engine/World.h"
#include "TPSGameMode.h"

// Sets default values for this component's properties
UTPSHealthComponent::UTPSHealthComponent()
{
	DefaultHealth = 100;
	bIsDead = false;

	TeamNum = 255;
}

// Called when the game starts
void UTPSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UTPSHealthComponent::HandleDamage);
	}

	 Health = DefaultHealth;
}

void UTPSHealthComponent::HandleDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.0f || bIsDead) { return; }
	if (DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser)) { return; }

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0.0f;

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		ATPSGameMode* GM = Cast<ATPSGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

float UTPSHealthComponent::GetHealth() const
{
	return Health;
}

void UTPSHealthComponent::Heal(float Amount)
{
	if (Amount <= 0.0f || Health <= 0.0f){ return; }

	Health = FMath::Clamp(Health + Amount, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, -Amount, nullptr, nullptr, nullptr);


}

bool UTPSHealthComponent::IsFriendly(AActor * ActorA, AActor * ActorB)
{
	if (ActorA == nullptr || ActorB == nullptr)
	{
		// Assume Enemy
		return false;
	}
	UTPSHealthComponent* HealthCompA = Cast<UTPSHealthComponent>(ActorA->GetComponentByClass(UTPSHealthComponent::StaticClass()));
	UTPSHealthComponent* HealthCompB = Cast<UTPSHealthComponent>(ActorB->GetComponentByClass(UTPSHealthComponent::StaticClass()));

	if (HealthCompA == nullptr || HealthCompB == nullptr)
	{
		// Assume Friendly
		return true;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}


