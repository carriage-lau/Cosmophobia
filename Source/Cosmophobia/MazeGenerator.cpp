#include "MazeGenerator.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "MapNode.h"
#include <vector>

AMazeGenerator::AMazeGenerator(){
    PrimaryActorTick.bCanEverTick = false;
}

void AMazeGenerator::BeginPlay(){
    Super::BeginPlay();
    GenerateMaze();
}

void AMazeGenerator::Tick(float DeltaTime){
    // A timer. Nothing else.
    if(!HasRegenerated && FMath::Abs(GetWorld()->GetTimeSeconds() - 10) < 1){
        GenerateMaze();
        HasRegenerated = true;
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
    float CellSize = 300.f;
    UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    
        for (int32 y = 0; y < MazeGrid.Num(); y++)
        {
            for (int32 x = 0; x < MazeGrid[y].Num(); x++)
            {
                FVector SpawnLocation = FVector(x * CellSize, y * CellSize, 140);
                if(MazeGrid[x][y] != 0){
                    UE_LOG(LogTemp, Warning, TEXT("Generated a maze cell."));
                    AStaticMeshActor* Wall = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
                    if (Wall && CubeMesh) {
                        Wall->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
                        Wall->GetStaticMeshComponent()->SetWorldScale3D(FVector(3.0f));
                        Wall->SetMobility(EComponentMobility::Static);
                        Wall->Tags.Add(FName("MazeWallCube"));
                    }
                }
                else{
                    FVector NodeLocation = FVector(x * CellSize + 150.0f, y * CellSize + 150.0f, 140);
                    AMapNode* Node = GetWorld()->SpawnActor<AMapNode>(AMapNode::StaticClass(), NodeLocation, FRotator::ZeroRotator);
                    
                    // Adds lighting
                    FVector LightLocation = NodeLocation + FVector(0, 0, 200); // Offset up for visibility
                    APointLight* Light = GetWorld()->SpawnActor<APointLight>(LightLocation, FRotator::ZeroRotator);
                    
                    if (Light) {
                        Light->SetMobility(EComponentMobility::Movable); // Or Static for baked lighting
                        Light->SetLightColor(FLinearColor(1.0f, 0.9f, 0.7f)); // Warm color, tweak if needed
                        Light->SetIntensity(3000.f); // Tweak to taste
                        Light->SetAttenuationRadius(400.f); // Size of the light’s influence
                    }
                }
            }
        }
}
