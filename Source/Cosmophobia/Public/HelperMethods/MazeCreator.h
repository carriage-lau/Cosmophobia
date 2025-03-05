#include <vector>
#include <iostream>
#include <random>

class MazeCreator{
    public:
        MazeCreator();

        /** Uses Prim's Algorithm. N must be odd. */
        std::vector<std::vector<int>> GenerateMaze(std::vector<std::vector<int>>& maze, int N);

    private:
        struct MapCell;

        /** Checks if a cell is in bounds */
        bool IsInBounds(int x, int ys, int N);

        /** Adds a frontier cell */
        void AddFrontier(std::vector<std::vector<int>>& maze, std::vector<MapCell>& frontiersList, int x, int y, int N);
        

}