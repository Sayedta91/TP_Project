// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSWeapon.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

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
}

void ATPSWeapon::Fire()
{
	// Trace the world from pawn eyes to location of crosshair (center of screen)

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
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

		// Particle "Target" parameter
		FVector TracerEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams)) {
			// Blocking hit

			AActor* HitActor = Hit.GetActor();
			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			// Projectile Impact Effect
			if (ImpactEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			TracerEndPoint = Hit.ImpactPoint;
		}

		// Weapon Debug Line
		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Blue, false, 1.0f, 0, 1.0f);
		}
		
		FireEffects(TracerEndPoint);
		
	}
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
}

