// Copyright Epic Games, Inc. All Rights Reserved.

#include "CosmophobiaCharacter.h"
#include "CosmophobiaProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACosmophobiaCharacter

ACosmophobiaCharacter::ACosmophobiaCharacter() {
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component for the 1st person view
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	// Create and attach the Flashlight
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(FirstPersonCameraComponent);
	Flashlight->SetRelativeLocation(FVector(100.f, 0.f, 0.f)); // Adjust as needed
	Flashlight->SetIntensity(5000.f); // Adjust as needed
	Flashlight->SetOuterConeAngle(45.f); // Adjust as needed
	Flashlight->SetInnerConeAngle(30.f); // Adjust as needed
}

//////////////////////////////////////////////////////////////////////////// Input

void ACosmophobiaCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ACosmophobiaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACosmophobiaCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACosmophobiaCharacter::Look);

		// Flashlight
		EnhancedInputComponent->BindAction(ToggleFlashlightAction, ETriggerEvent::Triggered, this, &ACosmophobiaCharacter::ToggleFlashlight);
		// triggered by F key
		
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACosmophobiaCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACosmophobiaCharacter::StopSprint);
		// triggered by shift key

		EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Started, this, &ACosmophobiaCharacter::StartSneak);
		EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Completed, this, &ACosmophobiaCharacter::StopSneak);
		// triggered by control key

	
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ACosmophobiaCharacter::Move(const FInputActionValue& Value) {
	if (Controller != nullptr) {
		FVector2D MovementVector = Value.Get<FVector2D>();
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACosmophobiaCharacter::Look(const FInputActionValue& Value) {
	if (Controller != nullptr) {
		FVector2D LookAxisVector = Value.Get<FVector2D>();
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACosmophobiaCharacter::SetFearLevel(float NewFearLevel) {
	FearLevel = NewFearLevel;
}

void ACosmophobiaCharacter::SetHealthLevel(float NewHealthLevel) {
	HealthLevel = NewHealthLevel;
}

void ACosmophobiaCharacter::SetVelocityMultiplierLevel(float NewVelocityMultiplier) {
	VelocityMultiplier = NewVelocityMultiplier;
}

void ACosmophobiaCharacter::ToggleFlashlight() {
	if (Flashlight) {
		Flashlight->ToggleVisibility();
	}
}

void ACosmophobiaCharacter::UpdateMovement() {
	float SpeedMultiplier = 1.0f;
	if (LegDisabled) {
		SpeedMultiplier *= 0.5f; // Reduce speed by 50%
	}
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SpeedMultiplier;
}

void ACosmophobiaCharacter::SetLegDisabled(bool bDisabled) {
	LegDisabled = bDisabled;
	UpdateMovement();
}

void ACosmophobiaCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	// Custom tick logic
}

void ACosmophobiaCharacter::DamageHandler(EDamageType DamageType) {
	// Handle damage based on DamageType
}

void ACosmophobiaCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Custom BeginPlay logic
	BaseWalkSpeed = StartSpeed;
	UpdateMovementSpeed();
}

void ACosmophobiaCharacter::StartSprint()
{
	bIsSprinting = true;
	UpdateMovementSpeed();
}

void ACosmophobiaCharacter::StopSprint()
{
	bIsSprinting = false;
	UpdateMovementSpeed();
}

void ACosmophobiaCharacter::StartSneak()
{
	bIsSneaking = true;
	UpdateMovementSpeed();
}

void ACosmophobiaCharacter::StopSneak()
{
	bIsSneaking = false;
	UpdateMovementSpeed();
}

void ACosmophobiaCharacter::UpdateMovementSpeed()
{
	float SpeedMultiplier = 1.0f;
	if (bIsSprinting)
	{
		SpeedMultiplier *= SprintMultiplier;
	}
	if (bIsSneaking)
	{
		SpeedMultiplier *= SneakMultiplier;
	}
	// Ensure the character�s movement component uses the updated speed:
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * SpeedMultiplier;
}

void ACosmophobiaCharacter::ModifyHealth(float Delta)
{
	Health = FMath::Clamp(Health + Delta, 0.0f, 100.0f);
	if(Delta < 0){
		// trigger a red UI effect here
	}
	else{
		// trigger a green healing UI effect here
	}
	// Trigger any additional effects (e.g. UI update) if health changes.
}
