// Fill out your copyright notice in the Description page of Project Settings.

#include "TPCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "TPSHealthComponent.h"
#include "TPSWeapon.h"
#include "TP_Project.h"

// Sets default values
ATPCharacter::ATPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateAbstractDefaultSubobject<USpringArmComponent>(TEXT("SpringArm Component"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);
	
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComponent = CreateDefaultSubobject<UTPSHealthComponent>(TEXT("Health Component"));

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComp->SetupAttachment(SpringArmComponent);

	AimingFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;

	WeaponSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ATPCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComp->FieldOfView;

	// Spawn Weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	EquippedWeapon = GetWorld()->SpawnActor<ATPSWeapon>(StarterWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (EquippedWeapon)
	{
		EquippedWeapon->SetOwner(this);
		EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
	}
	
	HealthComponent->OnHealthChanged.AddDynamic(this, &ATPCharacter::OnHealthChanged);
	
}

void ATPCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ATPCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ATPCharacter::BeginCrouch()
{
	Crouch();
}

void ATPCharacter::EndCrouch()
{
	UnCrouch();
}

void ATPCharacter::BeginAiming()
{
	bIsAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed * 0.5f;
}

void ATPCharacter::StopAiming()
{
	bIsAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed / 0.5f;
}

void ATPCharacter::StartFire()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StartFire();
	}
}

void ATPCharacter::StopFire()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StopFire();
	}
}

void ATPCharacter::OnHealthChanged(UTPSHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		// Can die
		bDied = true;
		
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(10.0f);
	}
}

// Called every frame
void ATPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bIsAiming ? AimingFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);
}

// Called to bind functionality to input
void ATPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Player Movement(WASD) Input
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPCharacter::MoveRight);

	// Player Mouse Input
	PlayerInputComponent->BindAxis("LookUp", this, &ATPCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &ATPCharacter::AddControllerYawInput);

	// Player Crouch
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATPCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ATPCharacter::EndCrouch);

	// Player Weapon Actions
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATPCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ATPCharacter::StopFire);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ATPCharacter::BeginAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ATPCharacter::StopAiming);

}

FVector ATPCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

