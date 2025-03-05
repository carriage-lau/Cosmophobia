#ifndef MAZE_CREATOR_H
#define MAZE_CREATOR_H

#include <vector>
#include <iostream>
#include <random>

class MazeCreator{
    public:
        MazeCreator(int N);

        /** Uses Prim's Algorithm. Note: N must be odd. The algorithm does NOT adjust for this.*/
        void GenerateMaze(std::vector<std::vector<int>>& maze);

    private:
        struct MapCell;

        std::vector<std::vector<int>> Maze;

        /** Checks if a cell is in bounds. */
        bool IsInBounds(int x, int ys);

        /** Adds a frontier cell. */
        void AddFrontier(std::vector<std::vector<int>>& maze, std::vector<MapCell>& frontiersList, int x, int y);
};

#endif