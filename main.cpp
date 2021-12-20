#include <iostream>
#include <map>
#include <vector>
#include <climits>

#include <chrono>
using namespace std::chrono;

static const int CostToTurn {1};
static const int CostForElevator {1};
enum class Direction {
   Left = 0,
   Right = 1
};

// #define PTRS 1
#define COLL 1

#ifdef PTRS
#include "pointers/misc.h"
using namespace pointers;
#endif // PTRS

#ifdef COLL
#include "collections/misc.h"
using namespace collections;
#endif // COLL

#include <fstream>
#include <stdexcept>

/**
Compile commands:
g++ -O0 -g2 -o nodes nodes.cpp && ./nodes

stage "Best Path" is the first with multiple elevators on 1 floor
  */

int main(int argc, char** argv)
{
   // auto start = high_resolution_clock::now();

    int nb_floors; // number of floors
    int width; // width of the area
    int nb_rounds; // maximum number of rounds
    int exit_floor; // floor on which the exit is found
    int exit_pos; // position of the exit on its floor
    int nb_total_clones; // number of generated clones
    int nb_additional_elevators; // number of additional elevators that you can build
    int nb_elevators; // number of elevators

    std::ifstream level;
    // level.open("2missing.txt");
    // level.open("bestpath.txt");
    level.open("elevator.txt");
    if (!level.is_open())
       throw std::runtime_error("File not found");

    level >> nb_floors
        >> width >> nb_rounds
        >> exit_floor >> exit_pos
        >> nb_total_clones
        >> nb_additional_elevators
        >> nb_elevators;

    std::map<int, std::vector<Node>> elevators;
    for (int i = 0; i < nb_elevators; i++) {
        int elevator_floor; // floor on which this elevator is found
        int elevator_pos; // position of the elevator on its floor
        level >> elevator_floor >> elevator_pos;
        // std::cerr << "e floor: " << elevator_floor << ", e pos: " << elevator_pos << std::endl;

        elevators[elevator_floor].push_back(elevator_pos);
    }
    elevators[exit_floor].push_back(exit_pos);

   // TODO from a starting floor/pos get a path
   auto cmd = runGraph(nb_floors, exit_floor, exit_pos, elevators);

   // auto stop = high_resolution_clock::now();
   // auto duration = duration_cast<microseconds>(stop - start);
   // std::cout << "Total Duration: " << duration.count() << " in milliseconds." << std::endl;
   std::cout << "Next cmd: " << cmd << std::endl;
   return 0;
}
