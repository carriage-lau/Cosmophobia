#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeCreator.h"
#include "MazeGenerator.generated.h"

UCLASS()
class COSMOPHOBIA_API AMazeGenerator : public AActor
{
    GENERATED_BODY()
public:
    AMazeGenerator();

    UFUNCTION(BlueprintCallable, Category = "Maze")
    void GenerateMaze();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
private:
    int MazeSize = 20;
    
    bool HasRegenerated = false;
};
