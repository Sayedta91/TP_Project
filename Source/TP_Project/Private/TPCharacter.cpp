// Fill out your copyright notice in the Description page of Project Settings.

#include "TPCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ATPCharacter::ATPCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateAbstractDefaultSubobject<USpringArmComponent>(TEXT("SpringArm Component"));
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	CameraComponent->SetupAttachment(SpringArmComponent);

}

// Called when the game starts or when spawned
void ATPCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATPCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ATPCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

// Called every frame
void ATPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Player Movement(WASD) Input
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ATPCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &ATPCharacter::AddControllerYawInput);
}

