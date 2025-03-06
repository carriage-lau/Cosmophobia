#include "MazeCreator.h"
#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>

MazeCreator::MazeCreator(int N){
    //initializes the maze
    for(int i = 0; i < N; i++){
        std::vector<int> row;
        for(int j = 0; j < N; j++){
            row.push_back(1);
        }
        this-> Maze.push_back(row);
    }

    // adds a central room to prevent the player from suffocating.
    this->Maze[this->Maze.size() / 2][this->Maze.size() / 2] = 0;
}

bool MazeCreator::IsInBounds(int x, int y){
    return x >= 0 && x < this->Maze.size() && y >= 0 && y < this->Maze.size();
}

void MazeCreator::AddFrontier(std::vector<std::vector<int>>& maze, std::vector<MapCell>& frontiersList, int x, int y){
    if(IsInBounds(x, y) && maze[x][y] == 1){
        maze[x][y] = 2;
        frontiersList.push_back({x, y});
    }
}

void MazeCreator::GenerateMaze(){
    // Initializes a random number generator
    std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(0, this->Maze.size() - 1);

    // Initialize the frontier list
    std::vector<MapCell> frontiersList;
    int x = dist(rng);
    int y = dist(rng);

    // fixes the bug of no maze generating.
    this->Maze[x][y] = 0;
    AddFrontier(this->Maze, frontiersList, x+1, y);
    AddFrontier(this->Maze, frontiersList, x-1, y);
    AddFrontier(this->Maze, frontiersList, x, y+1);
    AddFrontier(this->Maze, frontiersList, x, y-1);

    while(!frontiersList.empty()){
        // Chooses a random frontier cell
        int frontierIndex = dist(rng) % frontiersList.size(); // not sure if this is random, but it'll have to do
        MapCell frontier = frontiersList[frontierIndex];
        frontiersList.erase(frontiersList.begin() + frontierIndex);

        // Check if the frontier cell can be opened. If so: marks it as explored and continue.
        int pathCount = 0;
        if(IsInBounds(frontier.x - 1, frontier.y) && this->Maze[frontier.x - 1][frontier.y] != 0){
            pathCount++;
        }
        if(IsInBounds(frontier.x + 1, frontier.y) && this->Maze[frontier.x + 1][frontier.y] == 0){
            pathCount++;
        }
        if(IsInBounds(frontier.x, frontier.y - 1) && this->Maze[frontier.x][frontier.y - 1] == 0){
            pathCount++;
        }
        if(IsInBounds(frontier.x, frontier.y + 1) && this->Maze[frontier.x][frontier.y + 1] == 0){
            pathCount++;
        }

        //if the frontier cell has only one path, open it
        if(pathCount == 1){
            this->Maze[frontier.x][frontier.y] = 0;
            AddFrontier(this->Maze, frontiersList, frontier.x - 1, frontier.y);
            AddFrontier(this->Maze, frontiersList, frontier.x + 1, frontier.y);
            AddFrontier(this->Maze, frontiersList, frontier.x, frontier.y - 1);
            AddFrontier(this->Maze, frontiersList, frontier.x, frontier.y + 1);
        }
    }
}

