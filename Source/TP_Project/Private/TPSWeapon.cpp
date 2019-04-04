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
#include "TPCharacter.h"
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
	BulletSpread = 2.0f;
}

void ATPSWeapon::BeginPlay()
{
	Super::BeginPlay();
	TimeBetweenShots = 60 / RateOfFire;
}

ATPCharacter * ATPSWeapon::GetPawnOwner() const
{
	return MyPawn;
}

void ATPSWeapon::Fire()
{
	// Trace the world from pawn eyes to location of crosshair (center of screen)

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector TraceFrom;
		FRotator EyeRotation;
		// Because theyre being passed as a reference, they will be filled with data from the owning actor
		MyOwner->GetActorEyesViewPoint(TraceFrom, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// Bullet Spread
		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);

		FVector TraceTo = TraceFrom + (ShotDirection * 10000);

		FCollisionQueryParams TraceParams;
		// Ignore both the actor and the weapon itself
		TraceParams.AddIgnoredActor(MyOwner);
		TraceParams.AddIgnoredActor(this);
		// more expensive but gives an exact value of what we hit
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = true;

		// Particle "Target" parameter
		FVector TracerEndPoint = TraceTo;
		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams)) {
			// Blocking hit

			AActor* HitActor = Hit.GetActor();

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 5.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), MyOwner, DamageType);
			
			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;
		}

		// Weapon Debug Line
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), TraceFrom, TraceTo, FColor::Blue, false, 1.0f, 0, 1.0f);
		}

		FireEffects(TracerEndPoint);

		LastTimeFired = GetWorld()->TimeSeconds;

	}
}

float ATPSWeapon::PlayReloadAnimation(UAnimMontage * Animation, float InPlayRate, FName StartSectionName)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		if (Animation)
		{
			Duration = MyPawn->PlayAnimMontage(Animation, InPlayRate, StartSectionName);
		}
	}

	return Duration;
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
		UGameplayStatics::SpawnSoundAttached(MuzzleFlashSound, MeshComponent, MuzzleSocketName);
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

void ATPSWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
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
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);

		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ImpactSound, ImpactPoint, ShotDirection.Rotation());
	}
}

