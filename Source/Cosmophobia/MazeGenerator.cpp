#include "MazeGenerator.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include <vector>

AMazeGenerator::AMazeGenerator(){
    PrimaryActorTick.bCanEverTick = false;
}

void AMazeGenerator::BeginPlay(){
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("BeginPlay Triggered!"));
    GenerateMaze();
}

void AMazeGenerator::Tick(float DeltaTime){
    // A timer. Nothing else.
    if(FMath::Abs(GetWorld()->GetTimeSeconds() - 10) < 0.01){
        GenerateMaze();
    }
}

/** Generates a maze inside the actual map. For the purposes of L1, maze is 20 x 20.*/
void AMazeGenerator::GenerateMaze(){
	MazeCreator MazeInstance = MazeCreator(this -> MazeSize);
    MazeInstance.GenerateMaze();
    std::vector<std::vector<int>> MazeVector = MazeInstance.GetMaze();
    TArray<TArray<int32>> MazeGrid;
    MazeGrid.SetNum(MazeVector.size()); // This sets the size of the array.
    
    // Copies the data from a 2d vector into Unreal's TArray structure
    for (int32 i = 0; i < MazeVector.size(); i++) {
        MazeGrid[i].SetNum(MazeVector[i].size()); // Sets stuff!

        for (int32 j = 0; j < MazeVector[i].size(); j++) {
            MazeGrid[i][j] = MazeVector[i][j];
        }
    }
    
    // This deletes any old cube meshes
    for(TActorIterator<AStaticMeshActor> It(GetWorld()); It; ++It){
        AStaticMeshActor* Wall = *It;
        if (Wall->Tags.Contains(FName("MazeWallCube"))) {
            Wall->Destroy();
        }
    }
    
    // And this generates the maze inside the map
    float CellSize = 300.f;  // spawn room is in (11, 11)
    UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    
        for (int32 y = 0; y < MazeGrid.Num(); y++)
        {
            for (int32 x = 0; x < MazeGrid[y].Num(); x++)
            {
                FVector SpawnLocation = FVector(x * CellSize, y * CellSize, 0);

                AStaticMeshActor* Wall = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
                if (Wall && CubeMesh) {
                    Wall->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
                    Wall->GetStaticMeshComponent()->SetWorldScale3D(FVector(3.0f));
                    Wall->SetMobility(EComponentMobility::Static);
                    Wall->Tags.Add(FName("MazeWallCube"));
                }
            }
        }
}
