// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "TPSHealthComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "TPCharacter.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"

// Sets default values
ATPSTrackerBot::ATPSTrackerBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetCanEverAffectNavigation(false);
	MeshComponent->SetSimulatePhysics(true);
	RootComponent = MeshComponent;

	HealthComponent = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("Health Component"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &ATPSTrackerBot::HandleTakeDamage);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetSphereRadius(200);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComponent->SetupAttachment(RootComponent);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredTargetDistance = 100;

	ExplosionDamage = 50;
	ExplosionRadius = 200;
	SelfDamageInterval = 0.25f;
}

// Called when the game starts or when spawned
void ATPSTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	// Initial move to
	FVector NextPathPoint = GetNextPathPoint();

}

void ATPSTrackerBot::HandleTakeDamage(UTPSHealthComponent * OwningHealthComp, float Health, float HealthDelta, const UDamageType * DamageType,
	AController * InstigatedBy, AActor * DamageCauser)
{

	// pulse material on damage taken
	if (MattInstance == nullptr)
	{
		MattInstance = MeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComponent->GetMaterial(0));
	}

	if (MattInstance)
	{
		MattInstance->SetScalarParameterValue("LastTImeDamageTaken", GetWorld()->TimeSeconds);
	}

	// Explode on health == 0

	if (Health <= 0.0f)
	{
		SelfDestruct();
	}
}

FVector ATPSTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath && NavPath->PathPoints.Num() > 1)
	{
		// Returns next point in path
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ATPSTrackerBot::SelfDestruct()
{
	if (bHasExploded) { return; }

	bHasExploded = true;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 12, FColor::Yellow, false, 2.0f, 0, 1.0f);

	UGameplayStatics::SpawnSoundAtLocation(this, ExplosionSound, GetActorLocation());

	Destroy();
}

void ATPSTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ATPSTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredTargetDistance)
	{
		NextPathPoint = GetNextPathPoint();
		DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!");
	}
	else
	{
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		// Keep moving towards target
		MeshComponent->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}

	DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 4.0f, 1.0f);
}

void ATPSTrackerBot::NotifyActorBeginOverlap(AActor * OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartedDestructionSeq)
	{
		ATPCharacter* PlayerPawn = Cast<ATPCharacter>(OtherActor);
		if (PlayerPawn)
		{
			// Overlapped with player
			// start self destruct sequence
			GetWorldTimerManager().SetTimer(TH_SelfDamage, this, &ATPSTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);

			bStartedDestructionSeq = true;
			UGameplayStatics::SpawnSoundAttached(SelfDestructSequence, RootComponent);
		}
	}
}
