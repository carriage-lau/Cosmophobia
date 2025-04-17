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
	GetCapsuleComponent()->InitCapsuleSize(16.f, 60.0f);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block); // Changed from Ignore
	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // ORDERING IS IMPORTANT!@!!
	GetCapsuleComponent()->SetGenerateOverlapEvents(false); // Disable overlap for main capsule

	// Create a CameraComponent
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 80.f)); // Position the camera
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

	HeadCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HeadCollision"));
	HeadCollision->SetupAttachment(GetMesh());
	HeadCollision->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

	HeadCollision->SetCollisionObjectType(ECC_Pawn); // Treat as pawn
	HeadCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HeadCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HeadCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HeadCollision->SetGenerateOverlapEvents(true);

	TorsoCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TorsoCollision"));
	TorsoCollision->SetupAttachment(GetMesh());
	TorsoCollision->SetRelativeLocation(FVector(0.f, 0.f, 40.f));

	TorsoCollision->SetCollisionObjectType(ECC_Pawn); // Treat as pawn
	TorsoCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TorsoCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	TorsoCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TorsoCollision->SetGenerateOverlapEvents(true);

	ArmCollisionL = CreateDefaultSubobject<UBoxComponent>(TEXT("ArmCollisionL"));
	ArmCollisionL->SetupAttachment(GetMesh());
	ArmCollisionL->SetRelativeLocation(FVector(-10.f, 0.f, 20.f));

	ArmCollisionL->SetCollisionObjectType(ECC_Pawn); // Treat as pawn
	ArmCollisionL->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ArmCollisionL->SetCollisionResponseToAllChannels(ECR_Ignore);
	ArmCollisionL->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ArmCollisionL->SetGenerateOverlapEvents(true);

	ArmCollisionR = CreateDefaultSubobject<UBoxComponent>(TEXT("ArmCollisionR"));
	ArmCollisionR->SetupAttachment(GetMesh());
	ArmCollisionR->SetRelativeLocation(FVector(10.f, 0.f, 20.f));

	ArmCollisionR->SetCollisionObjectType(ECC_Pawn); // Treat as pawn
	ArmCollisionR->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ArmCollisionR->SetCollisionResponseToAllChannels(ECR_Ignore);
	ArmCollisionR->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	ArmCollisionR->SetGenerateOverlapEvents(true);

	LegCollisionL = CreateDefaultSubobject<UBoxComponent>(TEXT("LegCollisionL"));
	LegCollisionL->SetupAttachment(GetMesh());
	LegCollisionL->SetRelativeLocation(FVector(-10.f, 0.f, 0.f));

	LegCollisionL->SetCollisionObjectType(ECC_Pawn); // Treat as pawn
	LegCollisionL->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LegCollisionL->SetCollisionResponseToAllChannels(ECR_Ignore);
	LegCollisionL->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	LegCollisionL->SetGenerateOverlapEvents(true);

	LegCollisionR = CreateDefaultSubobject<UBoxComponent>(TEXT("LegCollisionR"));
	LegCollisionR->SetupAttachment(GetMesh());
	LegCollisionR->SetRelativeLocation(FVector(10.f, 0.f, 0.f));

	LegCollisionR->SetCollisionObjectType(ECC_Pawn); // Treat as pawn
	LegCollisionR->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LegCollisionR->SetCollisionResponseToAllChannels(ECR_Ignore);
	LegCollisionR->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	LegCollisionR->SetGenerateOverlapEvents(true);

	HeadCollision->ComponentTags.Add("Head");
	TorsoCollision->ComponentTags.Add("Torso");
	ArmCollisionL->ComponentTags.Add("Arm");
	ArmCollisionR->ComponentTags.Add("Arm");
	LegCollisionL->ComponentTags.Add("Leg");
	LegCollisionR->ComponentTags.Add("Leg");
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
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, static_cast<void (ACosmophobiaCharacter::*)(const FInputActionValue&)>(&ACosmophobiaCharacter::Move));
                        

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, static_cast<void (ACosmophobiaCharacter::*)(const FInputActionValue&)>(&ACosmophobiaCharacter::Look));

		// Flashlight
		EnhancedInputComponent->BindAction(ToggleFlashlightAction, ETriggerEvent::Triggered, this, &ACosmophobiaCharacter::ToggleFlashlight);
		// triggered by F key
		
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACosmophobiaCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACosmophobiaCharacter::StopSprint);
		// triggered by shift key

		EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Started, this, &ACosmophobiaCharacter::StartSneak);
		EnhancedInputComponent->BindAction(SneakAction, ETriggerEvent::Completed, this, &ACosmophobiaCharacter::StopSneak);
		// triggered by control key

		EnhancedInputComponent->BindAction(PauseGameAction, ETriggerEvent::Started, this, &ACosmophobiaCharacter::PauseGame);
		EnhancedInputComponent->BindAction(PauseGameAction, ETriggerEvent::Completed, this, &ACosmophobiaCharacter::ResumeGame);
		// triggered by escape key

	
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

void ACosmophobiaCharacter::ModifyFearLevel(float NewFearLevel) {
	FearLevel = NewFearLevel;
}

void ACosmophobiaCharacter::ModifyHealthLevel(int Delta)
{
	HitsLeft = FMath::Clamp(HitsLeft + Delta, 0, 3);
	if(HitsLeft == 3){
		// resets all UI effects
		UE_LOG(LogTemp, Warning, TEXT("3 Hits Left!"));
	}
	else if(HitsLeft == 2){
		// trigger a red UI effect here
		UE_LOG(LogTemp, Warning, TEXT("2 Hits Left!"));
	}
	else if(HitsLeft == 1){
		// trigger a bigger red UI effect here
		UE_LOG(LogTemp, Warning, TEXT("1 Hit Left!"));
	}
	else{
//		HandleDeath();
		UE_LOG(LogTemp, Warning, TEXT("Dead Player"));
	}
}

void ACosmophobiaCharacter::HandleDeath(){
	// Handle death here
	
}

void ACosmophobiaCharacter::SetVelocityMultiplierLevel(float NewVelocityMultiplier) {
	VelocityMultiplier = NewVelocityMultiplier;
}

void ACosmophobiaCharacter::ToggleFlashlight() {
	if (Flashlight) {
		Flashlight->ToggleVisibility();
	}
}

void ACosmophobiaCharacter::SetLegDisabled(bool bDisabled) {
	LegDisabled = bDisabled;
	SetVelocityMultiplierLevel(this->VelocityMultiplier * 0.5); // Reduce speed by 50%
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * VelocityMultiplier;
}

void ACosmophobiaCharacter::SetTorsoDisabled(bool bDisabled) {
	TorsoDisabled = bDisabled;
	SetVelocityMultiplierLevel(this->VelocityMultiplier * 0.75); // Reduce speed by 25%
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * VelocityMultiplier;
}

void ACosmophobiaCharacter::SetArmDisabled(bool bDisabled){
	ArmDisabled = bDisabled;
}

void ACosmophobiaCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	// Custom tick logic
}

void ACosmophobiaCharacter::DamageHandler(EDamageType DamageType) {
	if(DamageType != EDamageType::Head){
		ModifyHealthLevel(-1);
		if(DamageType == EDamageType::Torso){
			if (!TorsoDisabled) SetTorsoDisabled(true);
			UE_LOG(LogTemp, Warning, TEXT("hit torso"));
		}
		else if(DamageType == EDamageType::Arm){
			if (!ArmDisabled) SetArmDisabled(true);
			UE_LOG(LogTemp, Warning, TEXT("hit arm"));
		}
		else if(DamageType == EDamageType::Leg){
			if (!LegDisabled) SetLegDisabled(true);
			UE_LOG(LogTemp, Warning, TEXT("hit leg"));
		}
	}
	else{
		ModifyHealthLevel(-3);
		UE_LOG(LogTemp, Warning, TEXT("hit head"));
	}
	// Handle damage based on DamageType
}

void ACosmophobiaCharacter::BeginPlay()
{
	Super::BeginPlay();
	// Custom BeginPlay logic
	BaseWalkSpeed = StartSpeed;
	UpdateMovementSpeed();
	UE_LOG(LogTemp, Warning, TEXT("HeadCollision tags: %d"), HeadCollision->ComponentTags.Num());
	for (const FName& Tag : HeadCollision->ComponentTags) {
		UE_LOG(LogTemp, Warning, TEXT("- Tag: %s"), *Tag.ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("TorsoCollision tags: %d"), TorsoCollision->ComponentTags.Num());
	for (const FName& Tag : TorsoCollision->ComponentTags) {
		UE_LOG(LogTemp, Warning, TEXT("- Tag: %s"), *Tag.ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("ArmCollisionL tags: %d"), ArmCollisionL->ComponentTags.Num());
	for (const FName& Tag : ArmCollisionL->ComponentTags) {
		UE_LOG(LogTemp, Warning, TEXT("- Tag: %s"), *Tag.ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("ArmCollisionR tags: %d"), ArmCollisionR->ComponentTags.Num());
	for (const FName& Tag : ArmCollisionR->ComponentTags) {
		UE_LOG(LogTemp, Warning, TEXT("- Tag: %s"), *Tag.ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("LegCollisionL tags: %d"), LegCollisionL->ComponentTags.Num());
	for (const FName& Tag : LegCollisionL->ComponentTags) {
		UE_LOG(LogTemp, Warning, TEXT("- Tag: %s"), *Tag.ToString());
	}
	UE_LOG(LogTemp, Warning, TEXT("LegCollisionR tags: %d"), LegCollisionR->ComponentTags.Num());
	for (const FName& Tag : LegCollisionR->ComponentTags) {
		UE_LOG(LogTemp, Warning, TEXT("- Tag: %s"), *Tag.ToString());
	}
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

void ACosmophobiaCharacter::PauseGame()
{
	// Pause the game
}

void ACosmophobiaCharacter::ResumeGame()
{
	// Resume the game
}	

void ACosmophobiaCharacter::UpdateMovementSpeed()
{
	if (bIsSprinting)
	{
		SetVelocityMultiplierLevel(VelocityMultiplier * SprintMultiplier);
	}
	if (bIsSneaking)
	{
		SetVelocityMultiplierLevel(VelocityMultiplier * SneakMultiplier);
	}
	// Ensure the character's movement component uses the updated speed:
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * VelocityMultiplier;
}
