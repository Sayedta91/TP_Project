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

	AimWalkSpeed = 250.0f;
	SprintWalkSpeed = 900.0f;

	WeaponSocketName = "WeaponSocket";
}

// Called when the game starts or when spawned
void ATPCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;
	HealthComponent->OnHealthChanged.AddDynamic(this, &ATPCharacter::OnHealthChanged);

	// Spawn Weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	EquippedWeapon = GetWorld()->SpawnActor<ATPSWeapon>(StarterWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (EquippedWeapon)
	{
		EquippedWeapon->SetOwner(this);
		EquippedWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
	}
}

void ATPCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
	if (Value < 0.5f && bIsSprinting)
	{
		bIsSprinting = false;
	}
}

void ATPCharacter::MoveRight(float Value)
{
	if (!bIsSprinting)
	{
		AddMovementInput(GetActorRightVector() * Value);
		if (!bIsAiming)
		{
			GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed = 500;
		}
	}
}

void ATPCharacter::BeginCrouch()
{
	bIsSprinting = false;
	Crouch();
}

void ATPCharacter::EndCrouch()
{
	UnCrouch();
}

void ATPCharacter::BeginAiming()
{
	bIsAiming = true;
	StopSprinting();
	GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;
}

void ATPCharacter::StopAiming()
{
	bIsAiming = false;
	GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed = 500;
}

void ATPCharacter::BeginSprinting()
{
	UnCrouch();
	bIsAiming = false;
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintWalkSpeed;
	StopFire();
}

void ATPCharacter::StopSprinting()
{
	bIsSprinting = false;
}

void ATPCharacter::StartFire()
{
	if (EquippedWeapon)
	{
		if (!bIsSprinting && !GetCharacterMovement()->IsFalling())
		{
			EquippedWeapon->StartFire();
		}
	}
}

void ATPCharacter::StopFire()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StopFire();
	}
}

void ATPCharacter::OnHealthChanged(UTPSHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bDied)
	{
		// Can die
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();
		SetLifeSpan(2.0f);

		EquippedWeapon->StopFire();
		EquippedWeapon->SetLifeSpan(2.0f);
	}
}

// Called every frame
void ATPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsSprinting)
	{
		CameraComp->SetFieldOfView(DefaultFOV);
	}
	else
	{
		float TargetFOV = bIsAiming ? AimingFOV : DefaultFOV;
		float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
		CameraComp->SetFieldOfView(NewFOV);
	}
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

	// Player Sprint
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ATPCharacter::BeginSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ATPCharacter::StopSprinting);

	// Player Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATPCharacter::Jump);

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

