#include <iostream>
#include <map>
#include <vector>
#include <climits>

#include <fstream>
#include <stdexcept>

/**
Compile commands:
g++ -o nodes nodes.cpp && ./nodes
  */



/**
Stage = 2 missing elevators
---------------------------

floors: 6
width: 13
rounds: 100
exit flr: 5
exit pos: 1# clones: 10#
extra ele: 2
eles: 3
e floor4 e pos: 1
e floor0 e pos: 4
e floor2 e pos: 7

start= Pos: 10 Floor: 0


.x...........
.e...........
.............
.......e.....
.............
....e.....S..


stage "Best Path" is the first with multiple elevators on 1 floor
*/

struct Elevator {
   int floor {0};
   int pos {0};
   Elevator(int flr, int p) : floor(flr), pos(p) {}
   Elevator() {}
};

static const int CostToTurn {1};

struct Node {
   std::string name {""};
   int pos {0};
   Node(int pos_, const std::string&& name_): pos(pos_), name((name_)) {}
   Node() {}

   std::vector<Node*> childNodes;

   void addChild(Node* child) {
      childNodes.push_back(child);
   }

   bool operator==(const Node& other) {
      return (this->name == other.name);
   }
};

int traverseGraph(Node& nodes, const Node& goal, int dist_traveled)
{
   int sdist = INT_MAX;
   if (nodes.childNodes.empty()) {
      if (nodes == goal) {
         sdist = dist_traveled;
         std::cout << "Goal found!" << std::endl;
      }

      return sdist;
   }

   for (auto n : nodes.childNodes) {
      int dist = dist_traveled + abs(nodes.pos - n->pos);
      dist = traverseGraph(*n, goal, dist);
      if (dist < sdist) sdist = dist;
   }
   return sdist;
}

int main(int argc, char** argv)
{
    int nb_floors; // number of floors
    int width; // width of the area
    int nb_rounds; // maximum number of rounds
    int exit_floor; // floor on which the exit is found
    int exit_pos; // position of the exit on its floor
    int nb_total_clones; // number of generated clones
    int nb_additional_elevators; // number of additional elevators that you can build
    int nb_elevators; // number of elevators

    std::ifstream level;
    level.open("2missing.txt");
    if (!level.is_open())
       throw std::runtime_error("File not found");
    
    level >> nb_floors >> width 
        >> nb_rounds >> exit_floor 
        >> exit_pos >> nb_total_clones 
        >> nb_additional_elevators 
        >> nb_elevators; 

    std::cerr << "floors: " << nb_floors 
        << " width: " << width 
        << " rounds: " << nb_rounds
        << " exit flr: " << exit_floor 
        << " exit pos: " << exit_pos 
        << " # clones: " << nb_total_clones 
        << " #extra ele: " << nb_additional_elevators 
        << " eles: " << nb_elevators
        << std::endl;

    std::map<int, std::vector<int>> elevators;
    for (int i = 0; i < nb_elevators; i++) {
        int elevator_floor; // floor on which this elevator is found
        int elevator_pos; // position of the elevator on its floor
        level >> elevator_floor >> elevator_pos;
        std::cerr << "e floor: " << elevator_floor << ", e pos: " << elevator_pos << std::endl;

        elevators[elevator_floor].push_back(elevator_pos);
    }

   // find floors with missing elevators
   std::map<int, std::vector<Node>> nodes;
   for (int i = 0; i < (nb_floors - 1); i++) {

      int flr_offset = 0;
      if (elevators.find(i) == elevators.end()) {
         std::cerr << "Missing an elevator on floor: " << i << std::endl;
         flr_offset = 1;
      }

      int pos = elevators[i+flr_offset][0];
      Node node(pos, std::to_string(i) + " " + std::to_string(pos));
      nodes[i].push_back(node);

      // generate graph
      if (i > 0) {
         for (auto& n : nodes[i]) {       // this floors nodes
            for (auto& pn : nodes[i-1]) { // previous floor nodes
               pn.addChild(&n);
            }
         }
      }
   }

   // Pop in the exit
   Node exitNode(exit_pos, "exit");
   for (auto& n : nodes[exit_floor - 1]) {
      n.addChild(&exitNode);
   }

   Node& startNode = nodes[0][0];
   int d = traverseGraph(startNode, exitNode, 0);
   std::cout << "Distance: " << d << std::endl;
   return 0;
}
