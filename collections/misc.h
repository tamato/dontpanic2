
namespace collections {

   struct Command {
      enum Commands {
         WAIT = 0,
         BLOCK,
         ELEVATOR
      };

      Commands cmd {Commands::WAIT};
      std::array<std::string, 3> cmdStrings;

      Command() 
         : cmdStrings {"WAIT", "BLOCK", "ELEVATOR"}
      { }

      Command operator=(const Commands command) {
         cmd = command;
         return *this;
      }

      const std::string& toString() const {
         return cmdStrings[cmd];
      }
   };

   struct Node {
      unsigned int id {INT_MAX};

      int pos {INT_MAX};
      int floor {INT_MAX}; // the *next* floor

      bool createElevator {false};

      Direction dir;

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
      Command cmd;
   };

   struct Clone {
      Direction dir;
      int floor {INT_MAX};
      int pos {INT_MAX};
      int goalFloor {INT_MAX};
      int goalPos {INT_MAX};

      bool testGoal() const {
         return (goalFloor == floor && goalPos == pos);
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
                     table[elevator.id].cmd = Command::ELEVATOR;
                     // std::cout << "Creating an elevator at : " << elevator.pos << ", " << elevator.floor - 1 << std::endl;
                  }
               }

            }
         }
      }

      return table;
   }

   void viewTable(const std::map<unsigned int, Record>& table) {
      std::cout << "Viewing UnSorted table" << std::endl;
      std::cout << "ID\t|From\t|Cost\t|Cmd" << std::endl;
      for (auto& r : table) {
         std::cout << r.first << "\t" 
            << r.second.fromId << "\t" 
            << r.second.cost << "\t" 
            << r.second.cmd.cmd << std::endl;
      }
   }

   void viewSorted(const std::vector<Record>& sorted) {
      std::cout << "Sorted table" << std::endl;
      std::cout << "ID\t| From\t| Cost\t| Cmd" << std::endl;

      int cost = 0;
      for (auto& r : sorted) {
         std::cout << r.id << "\t| "
            << r.fromId << "\t| " 
            << r.cost << "\t| " 
            << r.cmd.toString() << std::endl;

         cost += r.cost;
      }

      std::cout << "total cost: " << cost << std::endl;
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

   void runGraph(int nb_floors, std::map<int, std::vector<Node>>& elevators) 
   {
      // find floors with missing elevators
      for (int flr = 0; flr < (nb_floors - 1); flr++) {
         if (elevators.find(flr) == elevators.end()) {
            int next_floor = flr + 1;

            // TODO - pos could be a list and we don't want to add an elevator for every single one.
            for (auto& ele: elevators[next_floor]) {
               Node n;
               n.pos = ele.pos;
               n.floor = next_floor;
               n.createElevator = true;
               elevators[flr].push_back(n);
               // std::cout << "Added missing elevator. " << flr << " " << n.pos << std::endl;
            }
         }
      }

      // Pop in the exit
      Node startNode;
      startNode.pos = 2;
      startNode.floor = 0;
      startNode.dir = Direction::Right;

      auto start = high_resolution_clock::now();
      auto table = generateTable(startNode, elevators);
      auto mypath = sortTable(table);
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(stop - start);
      std::cout << "Traverse Duration: " << duration.count() << " in milliseconds." << std::endl;

      // viewTable(table);
      viewSorted(mypath);
   }
}
