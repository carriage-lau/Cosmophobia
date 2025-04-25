//#include "MazeWall.h"
//#include "Components/StaticMeshComponent.h"
//#include "UObject/ConstructorHelpers.h"
//
//
//// A custom maze wall, for dynamic generating.
//// Update: this is a stub class. Current method utilizes cube spawning directly in MazeGenerator
//AMazeWall::AMazeWall() {
//    UE_LOG(LogTemp, Warning, TEXT("Loading wall..."));
//    PrimaryActorTick.bCanEverTick = false;
//
//    UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
//    RootComponent = MeshComponent;
//
//    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
//    
//    if (CubeMesh.Succeeded())
//    {
//        MeshComponent->SetStaticMesh(CubeMesh.Object);
//    }
//    else{
//        UE_LOG(LogTemp, Warning, TEXT("Failed to load WallMesh."));
//    }
//    
//    // Post application scaling
//    MeshComponent->SetWorldScale3D(FVector(3.0f, 3.0f, 3.0f));
//    
//    MeshComponent -> SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//    MeshComponent -> SetCollisionResponseToAllChannels(ECR_Block);
//}
//
//void AMazeWall::BeginPlay()
//{
//    UE_LOG(LogTemp, Warning, TEXT("Trying to load wall."));
//	Super::BeginPlay();
//}
