// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"
#include "TP_Project.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("TP.DebugWeapons"), 
	DebugWeaponDrawing, 
	TEXT("Draw Debug Lines for Weapons"), 
	ECVF_Cheat);

// Sets default values
ATPSWeapon::ATPSWeapon()
{
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComponent;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";

	BaseDamage = 20.0f;

	RateOfFire = 600;
	CurrentChamberAmmo = 30;
	MaxChamberAmmo = 30;
	TotalAmmo = 120;
}

void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateOfFire;
}

void ATPSWeapon::Fire()
{
	// Trace the world from pawn eyes to location of crosshair (center of screen)

	
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		if (!CheckAmmo()){return;}
		FVector EyeLocation;
		FRotator EyeRotation;
		// Because theyre being passed as a reference, they will be filled with data from the owning actor
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		// Ignore both the actor and the weapon itself
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		// more expensive but gives an exact value of what we hit
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;


		// Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams)) {
			// Blocking hit

			AActor* HitActor = Hit.GetActor();
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			UParticleSystem* SelectedEffect = nullptr;
			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				SelectedEffect = FleshImpactEffect;
				break;
			default:
				SelectedEffect = DefaultImpactEffect;
				break;
			}

			// Projectile Impact Effect
			if (SelectedEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DefaultImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		// Weapon Debug Line
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Blue, false, 1.0f, 0, 1.0f);
		}
		
		FireEffects(TracerEndPoint);
		
		LastTimeFired = GetWorld()->TimeSeconds;
		CurrentChamberAmmo--;
		UE_LOG(LogTemp, Warning, TEXT("Current Ammo: %d"), CurrentChamberAmmo);
	}
}

bool ATPSWeapon::CheckAmmo()
{
	if (bIsReloading)
	{
		return false;
	}

	if (!HasAmmo())
	{
		if (CanReloadAmmo())
		{
			ReloadWeapon();
			return false;
		}
		else {
			StopFire();
			return false;
		}
	}
	return true;
}

bool ATPSWeapon::HasAmmo() const
{
	return CurrentChamberAmmo > 0 && TotalAmmo > 0;
}

bool ATPSWeapon::CanReloadAmmo() const
{
	return TotalAmmo > 0 && CurrentChamberAmmo < MaxChamberAmmo;
}

void ATPSWeapon::ReloadWeapon()
{
	if (bIsReloading) return;
	bIsReloading = true;
	int32 oldammo = CurrentChamberAmmo;
	int32 newammo = MaxChamberAmmo - oldammo;
	CurrentChamberAmmo = TotalAmmo >= newammo ? newammo : TotalAmmo;
	TotalAmmo -= newammo;

	GetWorldTimerManager().SetTimer(
		TimerHandle_Reloading, [this]() {
		bIsReloading = false; },
		TimeToReload, false, TimeToReload);
}

void ATPSWeapon::StartFire()
{
	float FirstDelay = FMath::Max(LastTimeFired + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TH_TimeBetweenShots, this, &ATPSWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void ATPSWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TH_TimeBetweenShots);
}

void ATPSWeapon::FireEffects(FVector TraceEnd)
{
	// Muzzle flash effect
	if (MuzzleFlashEffect) {
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlashEffect, MeshComponent, MuzzleSocketName);
	}

	// Projectile Tracer Effect
	if (ProjectileEffect)
	{
		// Get the location of the muzzle socket
		FVector MuzzleSocketLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		// Spawn the particle tracer effect at the muzzle socket location
		UParticleSystemComponent* ProjectileComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileEffect, MuzzleSocketLocation);

		if (ProjectileComponent) {
			ProjectileComponent->SetVectorParameter(TracerTargetName, TraceEnd);
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		if (PlayerController)
		{
			PlayerController->ClientPlayCameraShake(FireCamShake);
		}
	}
}

