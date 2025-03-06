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

        /** Uses Prim's Algorithm. Note: N must be odd. The algorithm does NOT adjust for this. You must input an NxN array of 1s.*/
        void GenerateMaze();

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