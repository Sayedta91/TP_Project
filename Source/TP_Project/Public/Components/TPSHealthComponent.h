// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSHealthComponent.generated.h"

// On Health Changed Event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UTPSHealthComponent*, OwningHealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TP_PROJECT_API UTPSHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPSHealthComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health Component")
	uint8 TeamNum;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Component")
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Component")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Component")
	float DefaultHealth;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:

	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	void Heal(float Amount);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);
};
