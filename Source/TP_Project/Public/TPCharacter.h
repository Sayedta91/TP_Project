// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPCharacter.generated.h"

// Forward Declaring Components
class UCameraComponent;
class USpringArmComponent;
class ATPSWeapon;
class UTPSHealthComponent;

UCLASS()
class TP_PROJECT_API ATPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UTPSHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player")
	bool bIsAiming;

	float AimWalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Player")
	bool bIsSprinting;

	void BeginSprinting();

	float SprintWalkSpeed;

	void StopSprinting();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player")
	float AimingFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	float DefaultFOV;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Player")
	ATPSWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ATPSWeapon> StarterWeapon;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponSocketName;

	UFUNCTION()
	void OnHealthChanged(UTPSHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void BeginAiming();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopAiming();
};
