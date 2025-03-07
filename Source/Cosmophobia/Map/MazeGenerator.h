#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeWall.h"
#include "MazeCreator.h"
#include "MazeGenerator.generated.h"

UCLASS()
class COSMOPHOBIA_API AMazeGenerator : public AActor
{
    GENERATED_BODY()
public:
    AMazeGenerator();

protected:
    virtual void BeginPlay() override;
    
private:
    
    int MazeSize = 21;

    
    void GenerateMaze();
};
