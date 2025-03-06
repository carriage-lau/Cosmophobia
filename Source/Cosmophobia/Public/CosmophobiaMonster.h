#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "MapNode.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../DamageTypes.h"
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

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* SphereComponent;

    // Member vars
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    AMapNode* TargetNode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    AMapNode* CurrentNode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float DetectionRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float VelocityMultiplier = 1.0f;

private:
    // Checks for wall
    bool CheckForWall();
    // Moves to another node at random
    void TraverseGraph();
    // Handles state, replaces former CheckForPlayer
    void HandleState();
    //pathfinding
    TArray<AMapNode*> SelectShortestPath(AMapNode* start, AMapNode* target);
    //movemint
    FVector Move(AMapNode* TargetNode, float DeltaTime, float Offset = 50.0f);
    // States 
    virtual void IdleState(float DeltaTime);
    virtual void NodeChaseState(float DeltaTime);
    virtual void DirectChaseState(float DeltaTime);


    FString state;
    FVector offset;

    FVector dir;
    float velocity = 750.0f; // Example value

    FTimerHandle TraverseTimerHandle;
};