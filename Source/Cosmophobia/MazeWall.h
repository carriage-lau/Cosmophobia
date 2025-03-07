#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeWall.generated.h"

UCLASS()
class COSMOPHOBIA_API AMazeWall : public AActor
{
    GENERATED_BODY()

public:
    AMazeWall();

protected:
    virtual void BeginPlay() override;
};
