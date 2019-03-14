// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;

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

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* MuzzleFlashEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ProjectileEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShake> FireCamShake;

	void Fire();

	FTimerHandle TH_TimeBetweenShots;

	float LastTimeFired;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BaseDamage;

	// RPM 
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float RateOfFire;

	// Bullet Spread in Degrees
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.0f))
	float BulletSpread;

	float TimeBetweenShots;

	FTimerHandle TH_Reloading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 TotalAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 MaxChamberAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentChamberAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsReloading;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float TimeToReload;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FString WeaponName;

public:	

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual bool CheckAmmo();

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	virtual bool HasAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	virtual bool CanReloadAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	virtual void Reload();

	void StartFire();

	void StopFire();
};
