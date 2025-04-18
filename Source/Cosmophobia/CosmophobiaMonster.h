#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "MapNode.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DamageTypes.h"
#include "CosmophobiaMonster.generated.h"

//prototype class for the game's monster
class AMapNode;
class ACosmophobiaCharacter;

struct FNodeCost {
    AMapNode* Node;
    float Cost;

    bool operator>(const FNodeCost& value) const {
        return Cost > value.Cost;
    }
};

UCLASS()
class COSMOPHOBIA_API ACosmophobiaMonster : public ACharacter {
    GENERATED_BODY()

public:
    ACosmophobiaMonster();
    virtual void Tick(float DeltaTime) override;
    void PopulateNodesList();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, Category = "AI")
    float MovementSpeed = 300.f;

    // Member vars
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    AMapNode* TargetNode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    AMapNode* CurrentNode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float DetectionRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float VelocityMultiplier = 1.0f;

    /*
    UFUNCTION()
    void OnMonsterHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
    */

    ACosmophobiaCharacter* GetPlayerCharacter() const;
    mutable ACosmophobiaCharacter* CachedPlayer;

    UFUNCTION()
    void OnMonsterOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32    OtherBodyIndex,
        bool     bFromSweep,
        const FHitResult& SweepResult
    );


private:
    FTimerHandle PlayerCheckTimerHandle;

    // Checks for wall
    bool CheckForWall(const FVector& Location);
    
    // Moves to another node at random
    void TraverseGraph();
    
    // Handles state, replaces former CheckForPlayer
    void HandleState();
    
    // Pathfinding
    TArray<AMapNode*> SelectShortestPath(AMapNode* start, AMapNode* target);
    
    // Movemint
    FVector Move(AMapNode* TargetNode, float DeltaTime, float Offset = 50.0f);
    
    // States
    virtual void IdleState(float DeltaTime);
    virtual void NodeChaseState(float DeltaTime);
    virtual void DirectChaseState(float DeltaTime);

    virtual void ValidatePlayerReference();

    // Markers
    FString state;
    FVector offset;

    // Parameters
    FVector dir;
    float velocity = 750.0f; // Example value
    float LastHitTimestamp = 0.0f;

    FTimerHandle TraverseTimerHandle;
};
