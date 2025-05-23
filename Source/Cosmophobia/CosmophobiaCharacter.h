#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SpotLightComponent.h"
#include <Camera/CameraComponent.h>
#include "DamageTypes.h"
#include "CosmophobiaCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class COSMOPHOBIA_API ACosmophobiaCharacter : public ACharacter {
    GENERATED_BODY()

public:
    ACosmophobiaCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void NotifyControllerChanged() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    // Damage Handling
    void DamageHandler(EDamageType DamageType);

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void ToggleFlashlight();
    void UpdateMovementSpeed();
    void SetLegDisabled(bool bDisabled);
    void StartSprint();
    void StopSprint();
    void StartSneak();
    void StopSneak();
    void ModifyHealth(float Delta);

    USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

protected:
    virtual void BeginPlay() override;

    // hitbox?
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UCapsuleComponent* HeadCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UBoxComponent* TorsoCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UBoxComponent* ArmCollisionL;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UBoxComponent* ArmCollisionR;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UBoxComponent* LegCollisionL;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
    UBoxComponent* LegCollisionR;

    // Player Attributes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
    float VelocityMultiplier = 1.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
    float FearLevel = 0.0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
    int HitsLeft = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
    bool ArmDisabled = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
    bool LegDisabled = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats", meta = (AllowPrivateAccess = "true"))
    bool TorsoDisabled = false;

    UPROPERTY(VisibleANywhere, BlueprintReadOnly, Category = "Lighting")
    USpotLightComponent* Flashlight;

    // Movemint Related
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float BaseWalkSpeed = 600.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* JumpAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* LookAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* ToggleFlashlightAction;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* FirstPersonCameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    USkeletalMeshComponent* Mesh1P;
    
    UPROPERTY(EditAnywhere, Category = "PostProcess")
    UMaterialInterface* BasePostProcessingMaterial;
    
    UPROPERTY()
    UMaterialInstanceDynamic* DynMat;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* SprintAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* SneakAction;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* PauseGameAction;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UDeathScreenWidget> DeathScreenWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    float SprintMultiplier = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
    float SneakMultiplier = 0.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attributes")
    float StartSpeed = 600.0f;


    // For storing the current state, you can use an enum or simple booleans:
    bool bIsSprinting = false;
    bool bIsSneaking = false;


private:
    void ModifyFearLevel(float NewFearLevel);
    void ModifyHealthLevel(int delta);
    void SetVelocityMultiplierLevel(float NewVelocityMultiplier);

    void DamageHandler();


    void SetTorsoDisabled(bool bDisabled);
    void SetArmDisabled(bool bDisabled);

    void PauseGame();
    void ResumeGame();

    void Tick();
    void HandleDeath();
};
