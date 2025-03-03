#pragma once

#include "CoreMinimal.h" //ignore this for god's sake
#include "GameFramework/Actor.h"
#include "MapNode.generated.h"

using namespace std;
UCLASS()
class COSMOPHOBIA_API AMapNode : public AActor {
    GENERATED_BODY()

public:
    AMapNode();

    //location
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node")
    FVector position;

    //array of nodes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Node")
    TArray<AMapNode*> ConnectedNodes;

    FString type;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

public:
    // Called every frame

};