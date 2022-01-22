#include <iostream>
#include <map>
#include <deque>
#include <vector>
#include <climits>
#include <fstream>
#include <stdexcept>

using namespace std;

static const int CostToTurn {1};
static const int CostForElevator {1};

struct Node {
   unsigned int id {INT_MAX};

   int pos {INT_MAX};
   int floor {INT_MAX}; // the *next* floor

   bool createElevator {false};

   string dir;

   Node() : id(NodeID++) {}

   private:
      static int NodeID;
};
int Node::NodeID = 0;

struct Record {
   unsigned int cost {INT_MAX};
   unsigned int fromId {INT_MAX};
   unsigned int id {INT_MAX};
   unsigned int pos {0};
   unsigned int floor {0};
   string cmd {"WAIT"};
};

struct Clone {
   string dir;
   int floor {INT_MAX};
   int pos {INT_MAX};
   int goalFloor {INT_MAX};
   int goalPos {INT_MAX};

   bool testGoal() const {
      return (goalFloor == floor && goalPos == pos);
   }

   bool hasLeader() const {
      return (floor > -1 && pos > -1);
   }
};

const map<unsigned int, Record> generateTable(const Node& startNode, map<int, std::vector<Node>>& elevators)
{
   deque<Node> frontier;
   frontier.push_back(startNode);

   // startNode is the last one added and will have the highest value
   // Generated elevators will be prioor to startNode
   map<unsigned int, Record> table;
   while ( !frontier.empty() ){
      const auto& node = frontier.front();
      frontier.pop_front();

      const auto& flr = node.floor + 1; 
      const auto& dir = node.dir;

      // queue in frontier nodes
      if (elevators.find(flr) != elevators.end()) { // don't seach beyond the exit floor
         for (auto& elevator : elevators[flr]) {
            elevator.dir = dir;

            // update table
            if (table.find(elevator.floor) == table.end()) {
               table[elevator.floor] = Record();
            }

            auto cost = elevator.pos - node.pos;
            if (node.dir == "LEFT" && cost > 0) {
               cost += 1;
               elevator.dir = "RIGHT";
            }
            else if (node.dir == "RIGHT" && cost < 0) {
               cost -= 1;
               elevator.dir = "LEFT";
            }
            frontier.push_back(elevator);

            cost = abs(cost);
            if (cost < table[elevator.floor].cost) {
               table[elevator.floor].cost = cost;

               // update 'closest node'/'fromId'?
               table[elevator.floor].fromId = node.id;
               table[elevator.floor].id = elevator.id;
               table[elevator.floor].pos = elevator.pos;
               table[elevator.floor].floor = elevator.floor;

               // make an elevator?
               if (elevator.createElevator) {
                  table[elevator.floor].cmd = "ELEVATOR";
               }
            }
         }
      }
   }

   return table;
}

const vector<Record> sortTable(map<unsigned int, Record>& table) {
   vector<Record> sorted;

   const int count = table.size();
   const int startNodeId = count - 1;
   int nextNodeId = startNodeId - 1;
   while (nextNodeId != startNodeId) {
      auto nextNode = table[nextNodeId];
      sorted.push_back(nextNode);
      nextNodeId = nextNode.fromId;
   }
   return sorted;
}

const map<unsigned int, Record> runGraph(int clone_pos, int nb_floors, map<int, std::vector<Node>>& elevators) 
{
   // find floors with missing elevators
   for (int flr = 0; flr < (nb_floors - 1); flr++) {
      if (elevators.find(flr) == elevators.end()) {

         // find the next floor with an elevator
         for (int next_floor = (flr+1); next_floor < nb_floors; ++next_floor) {
            if (elevators.find(next_floor) != elevators.end()) {

               // TODO - pos could be a list and we don't want to add an elevator for every single one.
               auto& ele= elevators[next_floor][0];
               Node n;
               n.pos = ele.pos;
               n.floor = flr;
               n.createElevator = true;
               elevators[flr].push_back(n);
               break;
            }
         }
      }
   }

   // Push in the exit
   Node startNode;
   startNode.pos = clone_pos;
   startNode.floor = -1;
   startNode.dir = "RIGHT";

   auto table = generateTable(startNode, elevators);
   //auto mypath = sortTable(table);
   return table;
}


/**
Compile commands:
g++ -O0 -g2 -o dontPanic main.cpp && ./dontPanic

stage "Best Path" is the first with multiple elevators on 1 floor
  */

void printTable(map<unsigned int, Record> table)
{
   for (auto kp : table) {
      cerr << "ID: " << kp.first
         << " id: " << kp.second.id
         << " pos " << kp.second.pos
         << " floor " << kp.second.floor
         << " cmd <" << kp.second.cmd << ">"
         << " fromId " << kp.second.fromId 
         << endl;
   }
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

   ifstream level;
   // level.open("elevator.txt");
   // level.open("one_elevator_per_floor.txt");
   // level.open("2missing.txt");
   level.open("3missing.txt");
   // level.open("bestpath.txt");
   if (!level.is_open())
      throw runtime_error("File not found");

   level >> nb_floors
      >> width >> nb_rounds
      >> exit_floor >> exit_pos
      >> nb_total_clones
      >> nb_additional_elevators
      >> nb_elevators;

   map<int, vector<Node>> elevators;
   for (int i = 0; i < nb_elevators; i++) {
      int elevator_floor; // floor on which this elevator is found
      int elevator_pos; // position of the elevator on its floor
      level >> elevator_floor >> elevator_pos;

      Node node;
      node.pos = elevator_pos;
      node.floor = elevator_floor;
      elevators[elevator_floor].push_back(node);
      // cerr << node.pos << " " << node.floor << endl;
   }

   Node node;
   node.pos = exit_pos;
   node.floor = exit_floor;
   elevators[exit_floor].push_back(node);

   // TODO from a starting floor/pos get a path
   auto mypath = runGraph(0, nb_floors, elevators);
   printTable(mypath);

  Clone c { "LEFT", 1, 3 };
  std::string cmd = "WAIT";

  // get command
  auto& next = mypath[c.floor];
  if (c.pos == next.pos) {
     cmd = next.cmd;
     next.cmd = "WAIT";
     cerr << "Next: " << next.pos << "," << next.floor << ", " << next.cmd << endl;
  }
   
   cerr << "Dir: " << c.dir << " clone: " << c.pos << "," << c.floor << " next: " << next.pos << ","<< next.floor << endl;
  // check if we need to turn
  int dist_to_elevator = next.pos - c.pos;
  if (c.dir== "LEFT" && dist_to_elevator > 0) {
     cmd = "BLOCK";
     cerr << "blocked left" << endl;
  }
  else if (c.dir== "RIGHT" && dist_to_elevator < 0) {
     cmd = "BLOCK";
     cerr << "blocked right" << endl;
  }
   return 0;
}
