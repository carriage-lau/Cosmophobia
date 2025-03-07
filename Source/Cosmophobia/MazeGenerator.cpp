#include "MazeGenerator.h"
#include "Engine/World.h"
#include <vector>

AMazeGenerator::AMazeGenerator(){
    
    PrimaryActorTick.bCanEverTick = false;
}

void AMazeGenerator::BeginPlay(){
    Super::BeginPlay();
    GenerateMaze();
}

void AMazeGenerator::GenerateMaze(){
	MazeCreator MazeInstance = MazeCreator(21);
    MazeInstance.GenerateMaze();
    std::vector<std::vector<int>> MazeVector = MazeInstance.GetMaze();
    TArray<TArray<int32>> MazeGrid;
    MazeGrid.SetNum(MazeVector.size());
    
    for (int32 i = 0; i < MazeVector.size(); i++)
        {
            MazeGrid[i].SetNum(MazeVector[i].size());

            for (int32 j = 0; j < MazeVector[i].size(); j++)
            {
                MazeGrid[i][j] = MazeVector[i][j]; // Copy data
            }
        }
    
    float CellSize = 1000.f;  // Each cell is 1000x1000 Unreal units

        for (int32 y = 0; y < MazeGrid.Num(); y++)
        {
            for (int32 x = 0; x < MazeGrid[y].Num(); x++)
            {
                FVector SpawnLocation = FVector(x * CellSize, y * CellSize, 0);

                if (MazeGrid[y][x] != 0) // 1 or 2 → Wall
                {
                    GetWorld()->SpawnActor<AMazeWall>(AMazeWall::StaticClass(), SpawnLocation, FRotator::ZeroRotator);
                }
            }
        }
}
