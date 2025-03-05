#include "MazeCreator.h"
#include <vector>
#include <iostream>
#include <random>

MazeCreator::MazeCreator(int N){
    //initializes the maze
    for(int i = 0; i < N; i++){
        std::vector<int> row;
        for(int j = 0; j < N; j++){
            row.push_back(1);
        }
        this-> Maze.push_back(row);
    }
}

bool MazeCreator::IsInBounds(int x, int y){
    return x >= 0 && x < this->Maze.size() && y >= 0 && y < this->Maze.size();
}

// FIXME: This function is not used in the codebase
// void MazeCreator::AddFrontier(std::vector<std::vector<int>>& maze, std::vector<MapCell>& frontiersList, int x, int y){
//     if(IsInBounds(x, y) && maze[x][y] == 1){
//         maze[x][y] = 2;
//         frontiersList.push_back(MapCell(x, y));
//     }
// }

