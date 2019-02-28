// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

UCLASS()
class TP_PROJECT_API ATPSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATPSWeapon();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;

	void FireEffects(FVector TraceEnd);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleFlashEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ProjectileEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCamShake;

	void Fire();

	FTimerHandle TH_TimeBetweenShots;

	float LastTimeFired;

	/* RPM */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	float TimeBetweenShots;

	FTimerHandle TimerHandle_Reloading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ToolTip = "Number of bullets available to shoot"))
	int32 TotalAmmo = 90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ToolTip = "Max number of bullets in the chamber"))
	int32 MaxChamberAmmo = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ToolTip = "Current number of bullets in the chamber"))
	int32 CurrentChamberAmmo = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (ToolTip = "Flag to know when the weapon is in reloading time"))
	bool IsReloading = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (ToolTip = "Time in seconds to reload the weapon"))
	float TimeToReload = 0.9f;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool CheckAmmo();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool HasAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool CanReloadAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	virtual void Reload();

public:	

	void StartFire();

	void StopFire();
};
