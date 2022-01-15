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

   std::string dir;

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
   std::string cmd;
};

struct Clone {
   std::string dir;
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

const std::map<unsigned int, Record> generateTable(const Node& startNode, std::map<int, std::vector<Node>>& elevators)
{
   std::deque<Node> frontier;
   frontier.push_back(startNode);

   std::map<unsigned int, Record> table;
   table[startNode.id] = Record();
   table[startNode.id].fromId = startNode.id;
   table[startNode.id].id = startNode.id;
   table[startNode.id].cost = 0;

   while ( !frontier.empty() ){
      auto& node = frontier.front();
      frontier.pop_front();
      // std::cout << "Popping node: " << node.id << std::endl;

      const auto& flr = node.floor; // floor is the *next* floor
      // std::cout << "Visiting nodes on floor " << node.floor << std::endl;

      // queue in frontier nodes
      if (elevators.find(flr) != elevators.end()) {
         for (auto& elevator : elevators[flr]) {
            frontier.push_back(elevator);
            // std::cout << "Pushing node: " << elevator.id << std::endl;

            // update table
            if (table.find(elevator.id) == table.end()) {
               table[elevator.id] = Record();
            }

            auto cost = elevator.pos - node.pos;
            // check if we need to turn
            // bool turn = false;
            // if (cost < 0 && node.dir == Direction::Right) {
            //    turn = true;
            // } else if (cost > 0 && node.dir == Direction::Left) {
            //    turn = true;
            // }

            cost = abs(cost);
            // if (turn) {
            //    // This entry could still be something already
            //    // if there is a double block
            //    // or if there are 2 elevators that need to be created.
            //    table[node.id].cmd = Command::BLOCK;
            //    cost += CostToTurn;
            // }

            if (cost < table[elevator.id].cost) {
               table[elevator.id].cost = cost;

               // update 'closest node'/'fromId'?
               table[elevator.id].fromId = node.id;
               table[elevator.id].id = elevator.id;
               table[elevator.id].pos = node.pos;
               table[elevator.id].floor = node.floor;

               // make an elevator?
               if (elevator.createElevator) {
                  table[elevator.id].cmd = "ELEVATOR";
                  // std::cout << "Creating an elevator at : " << elevator.pos << ", " << elevator.floor - 1 << std::endl;
               }
            }

         }
      }
   }

   return table;
}

const std::vector<Record> sortTable(std::map<unsigned int, Record>& table) {
   std::vector<Record> sorted;

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

const std::map<unsigned int, Record> runGraph(int nb_floors, std::map<int, std::vector<Node>>& elevators) 
{
   // find floors with missing elevators
   for (int flr = 0; flr < (nb_floors - 1); flr++) {
      if (elevators.find(flr) == elevators.end()) {
         int next_floor = flr + 1;
         //cerr << "Missing an elevator" << endl;

         // TODO - pos could be a list and we don't want to add an elevator for every single one.
         for (auto& ele: elevators[next_floor]) {
            Node n;
            n.pos = ele.pos;
            n.floor = next_floor;
            n.createElevator = true;
            elevators[flr].push_back(n);
            //cerr << "Added missing elevator. " << flr << " " << n.pos << std::endl;
         }
      }
   }

   // Push in the exit
   Node startNode;
   startNode.pos = 2;
   startNode.floor = 0;
   startNode.dir = "RIGHT";

   cerr << "about to gen" << endl;
   auto table = generateTable(startNode, elevators);
   //auto mypath = sortTable(table);
   cerr << "sorted" << endl;

   return table;
}

int main()
{
    int nb_floors; // number of floors
    int width; // width of the area
    int nb_rounds; // maximum number of rounds
    int exit_floor; // floor on which the exit is found
    int exit_pos; // position of the exit on its floor
    int nb_total_clones; // number of generated clones
    int nb_additional_elevators; // number of additional elevators that you can build
    int nb_elevators; // number of elevators
    
    cin >> nb_floors >> width 
        >> nb_rounds >> exit_floor 
        >> exit_pos >> nb_total_clones 
        >> nb_additional_elevators 
        >> nb_elevators; 
    cin.ignore();

    cerr << " floors: " << nb_floors 
        << " width: " << width 
        << " rounds: " << nb_rounds
        << " exit flr: " << exit_floor 
        << " exit pos: " << exit_pos 
        << " # clones: " << nb_total_clones 
        << " # extra ele: " << nb_additional_elevators 
        << " eles: " << nb_elevators
        << endl;

   std::map<int, std::vector<Node>> elevators;
   for (int i = 0; i < nb_elevators; i++) {
      int elevator_floor; // floor on which this elevator is found
      int elevator_pos; // position of the elevator on its floor
      cin >> elevator_floor >> elevator_pos; cin.ignore();

      Node node;
      node.pos = elevator_pos;
      node.floor = elevator_floor + 1; // points to the next floor

      elevators[elevator_floor].push_back(node);
   }
  
   Node node;
   node.pos = exit_pos;
   node.floor = exit_floor;
   elevators[exit_floor].push_back(node);

    // game loop
    while (1) {
        int clone_floor; // floor of the leading clone
        int clone_pos; // position of the leading clone on its floor
        string direction; // direction of the leading clone: LEFT or RIGHT
        cin >> clone_floor >> clone_pos >> direction; cin.ignore();

        Clone c { direction, clone_floor, clone_pos };
        cerr << "Has leader: " << boolalpha << c.hasLeader() << endl;

        std::string cmd = "WAIT";

        // check for leader
        if (c.hasLeader()) {
            auto mypath = runGraph(nb_floors, elevators);
            
            // keep in bounds
            if ((c.pos == (width - 1)) || (c.pos == 0)) {
                cmd = "BLOCK";
            }   

            // Node* startNode = nullptr;
            // int min_dist = INT_MAX;
            // for (auto& n: nodes[c.floor]) {
            //     int dist_to_elevator = n.pos - c.pos;
            //
            //     if (c.direction == Direction::Left) {
            //         if (dist_to_elevator < 0) {
            //         dist_to_elevator -= 1;
            //         }
            //     }
            //     else if (c.direction == Direction::Right) {
            //         if (dist_to_elevator > 0) {
            //         dist_to_elevator += 1;
            //         }
            //     }
            //
            //     dist_to_elevator = abs(dist_to_elevator);
            //     if (dist_to_elevator < min_dist) {
            //         min_dist = dist_to_elevator;
            //         startNode = &n;
            //     }
            // }
        }

        cout << cmd << endl; // action: WAIT or BLOCK or ELEVATOR
    }
}
