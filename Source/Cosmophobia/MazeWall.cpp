#include "MazeWall.h"
#include "Components/StaticMeshComponent.h"

AMazeWall::AMazeWall()
{
    PrimaryActorTick.bCanEverTick = false;

    UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> WallMesh(TEXT("StaticMesh'/Game/Meshes/WallMesh.WallMesh'"));
    if (WallMesh.Succeeded())
    {
        MeshComponent->SetStaticMesh(WallMesh.Object);
    }
}

void AMazeWall::BeginPlay()
{
	Super::BeginPlay();
}
