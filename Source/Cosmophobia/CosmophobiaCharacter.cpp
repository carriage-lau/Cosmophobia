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

	// hitbox setup
	HeadCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HeadCollision"));
	HeadCollision->SetupAttachment(GetMesh()); // Attach to the mesh or a specific bone if needed
	HeadCollision->SetRelativeLocation(FVector(0.f, 0.f, 80.f)); // Adjust position as required

	TorsoCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TorsoCollision"));
	TorsoCollision->SetupAttachment(GetMesh());
	TorsoCollision->SetRelativeLocation(FVector(0.f, 0.f, 40.f)); // Adjust as required

	ArmCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ArmCollision"));
	ArmCollision->SetupAttachment(GetMesh());
	ArmCollision->SetRelativeLocation(FVector(0.f, 0.f, 20.f)); // Adjust as required

	LegCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LegCollision"));
	LegCollision->SetupAttachment(GetMesh());
	LegCollision->SetRelativeLocation(FVector(0.f, 0.f, 0.f)); // Adjust as required

	HeadCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	HeadCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	HeadCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	TorsoCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TorsoCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	TorsoCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	TorsoCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);

	ArmCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ArmCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	ArmCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	ArmCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);

	LegCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LegCollision->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	LegCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	LegCollision->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);

	HeadCollision->OnComponentHit.AddDynamic(this, &ACosmophobiaCharacter::OnHeadHit);
	TorsoCollision->OnComponentHit.AddDynamic(this, &ACosmophobiaCharacter::OnTorsoHit);
	ArmCollision->OnComponentHit.AddDynamic(this, &ACosmophobiaCharacter::OnArmHit);
	LegCollision->OnComponentHit.AddDynamic(this, &ACosmophobiaCharacter::OnLegHit);
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
	}
	else if(HitsLeft == 2){
		// trigger a red UI effect here
	}
	else if(HitsLeft == 1){
		// trigger a bigger red UI effect here
	}
	else{
		// handle death here
	}
	// Trigger any additional effects (e.g. UI update) if health changes.
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
	if (LegDisabled) {
		SetVelocityMultiplierLevel(this->VelocityMultiplier * 0.5); // Reduce speed by 50%
	}
	else if(TorsoDisabled){
		SetVelocityMultiplierLevel(this->VelocityMultiplier * 0.75); // Reduce speed by 25%
	}
	else{
		SetVelocityMultiplierLevel(this->VelocityMultiplier); // Reset speed to default, redundant logic?
	}
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * VelocityMultiplier;
}

void ACosmophobiaCharacter::SetLegDisabled(bool bDisabled) {
	LegDisabled = bDisabled;
	UpdateMovement();
}

void ACosmophobiaCharacter::SetTorsoDisabled(bool bDisabled) {
	TorsoDisabled = bDisabled;
	UpdateMovement();
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
		ModifyHealth(-1);
		if(DamageType == EDamageType::Torso){
			SetTorsoDisabled(true);
		}
		else if(DamageType == EDamageType::Arm){
			SetArmDisabled(true);
		}
		else if(DamageType == EDamageType::Leg){
			SetLegDisabled(true);
		}
	}
	else{
		ModifyHealth(-3);
	}
	// Handle damage based on DamageType
}

void ACosmophobiaCharacter::OnHeadHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this) {
		DamageHandler(EDamageType::Head);
	}
}

void ACosmophobiaCharacter::OnTorsoHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this) {
		DamageHandler(EDamageType::Torso);
	}
}

void ACosmophobiaCharacter::OnArmHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this) {
		DamageHandler(EDamageType::Arm);
	}
}

void ACosmophobiaCharacter::OnLegHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this) {
		DamageHandler(EDamageType::Leg);
	}
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
	// Ensure the character�s movement component uses the updated speed:
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed * VelocityMultiplier;
}
