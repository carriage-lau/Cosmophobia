#ifndef MAZE_CREATOR_H
#define MAZE_CREATOR_H

#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <ctime>
#include <chrono>

class MazeCreator{
    public:
        MazeCreator(int N);

        /** Uses Prim's Algorithm. Note: N must be odd for the initial room to spawn in the center. The algorithm does NOT adjust for this.*/
        void GenerateMaze();
    
        /** Returns the maze. */
        std::vector<std::vector<int>> GetMaze();

    private:
        struct MapCell{
            int x, y;
        };

        std::vector<std::vector<int>> Maze;

        /** Checks if a cell is in bounds. */
        bool IsInBounds(int x, int ys);

        /** Adds a frontier cell. */
        void AddFrontier(std::vector<std::vector<int>>& maze, std::vector<MapCell>& frontiersList, int x, int y);
};

#endif
