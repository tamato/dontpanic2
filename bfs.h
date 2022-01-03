namespace bfs {

   struct Node {
      int id {INT_MAX};
      bool visited {false};

      int pos {INT_MAX};
      int floor {INT_MAX};

      bool createElevator {false};

      std::vector<int> children;
      std::vector<int> siblings;
   };

   struct Clone {
      Direction dir;
      int floor {INT_MAX};
      int pos {INT_MAX};
      bool testGoal() const {
         return (goalFloor == floor && goalPos == pos);
      }
   };


   std::string traverseGraph(const Node& node, const Node& goal, const std::map<int, std::vector<Node>>& elevators)
   {
      std::string cmd = "WAIT";
      
      std::queue<Node> frontier;
      frontier.push_back(node);

      for (auto& neighbor: frontier) {
         const auto& flr = neighbor.floor;

         // re-queue in frontier nodes
         if (elevators.find(flr+
         for (auto& elevator : elevators[flr]) {
            frontier.push_back(elevator);
         }
      }

      return cmd;
   }

   std::string runGraph(int nb_floors, int exit_floor, int exit_pos, std::map<int, std::vector<Node>>& elevators) 
   {
      // find floors with missing elevators
      for (int flr = 0; flr < (nb_floors - 1); flr++) {
         if (elevators.find(flr) == elevators.end()) {
            int next_floor = flr + 1;
            // TODO - pos could be a list and we don't want to add an elevator for every single one.
            for (auto& pos: elevators[next_floor]) {
               Node n(pos);
               n.createElevator = true;
               elevators[flr].push_back(n);
               // std::cout << "Added missing elevator. " << flr << " " << n.pos << std::endl;
            }
         }
      }

      // Pop in the exit
      Clone clone = {Direction::Left, 0, 6, exit_floor, exit_pos};

      auto start = high_resolution_clock::now();
      auto cmd = traverseGraph(clone, elevators, 0);
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(stop - start);
      std::cout << "Traverse Duration: " << duration.count() << " in milliseconds." << std::endl;

      return cmd;
   }
}

