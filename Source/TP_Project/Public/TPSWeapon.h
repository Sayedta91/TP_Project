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

	/* Get pawn owner */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	class ATPCharacter* GetPawnOwner() const;

	UPROPERTY(Transient)
	class ATPCharacter* MyPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;

	/************************************************************************/
	/* Simulation & FX                                                      */
	/************************************************************************/

	void FireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	USoundBase* MuzzleFlashSound;

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


	/************************************************************************/
	/* Fire & Damage Handling                                               */
	/************************************************************************/

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

	/************************************************************************/
	/* Ammo & Reloading                                                     */
	/************************************************************************/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FString WeaponName;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* PlayerReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* WeaponReloadAnim;

	float PlayReloadAnimation(UAnimMontage* Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	FTimerHandle TimerHandle_Reloading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 TotalAmmo = 1000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 MaxChamberAmmo = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentChamberAmmo = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool IsReloading = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float TimeToReload = 0.9f;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	virtual bool HasAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	virtual bool CanReloadAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	virtual void Reload();

public:	

	UFUNCTION(BlueprintCallable, Category = "Weapons")
	virtual bool CheckAmmo();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();
		
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();
};
