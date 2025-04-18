#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "MapNode.generated.h"

UCLASS()
class COSMOPHOBIA_API AMapNode : public AActor {
    GENERATED_BODY()

public:
    AMapNode();

    // Position -- this is so that we can edit inside the editor.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node")
    FVector Position;

    // An array of connected nodes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node")
    TArray<AMapNode*> ConnectedNodes;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* NodeMesh;

protected:
    // Begin play
    virtual void BeginPlay() override;
    
    // Tick (this should do nothing)
    virtual void Tick(float DeltaTime) override;

public:
    // Called every frame
    void FillConnectedNodes(TArray<AMapNode*> ConnectedNodesList);

};
