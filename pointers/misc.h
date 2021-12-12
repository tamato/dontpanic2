
namespace pointers {

   struct Node {
      std::string name {""};
      int pos {INT_MAX};
      Node(int pos_, const std::string&& name_): pos(pos_), name((name_)) {}
      Node() {}

      std::vector<Node*> childNodes;

      void addChild(Node* child) {
         childNodes.push_back(child);
      }

      bool operator==(const Node& other) const {
         return (this->name == other.name);
      }
   };

   int traverseGraph(const Node* start, const Node& goal, int dist_traveled, Direction dir)
   {
      int min_dist = INT_MAX;
      if (*start == goal) {
         min_dist = dist_traveled;
         // std::cout << "Goal found!" << " " << dist_traveled << std::endl;
         return min_dist;
      }

      for (auto n : start->childNodes) {
         int dist_to_elevator = start->pos - n->pos;
         bool block_needed = false;
         if (dir == Direction::Left) {
            if (dist_to_elevator < 0) {
               block_needed = true;
            }
         }
         else if (dir == Direction::Right) {
            if (dist_to_elevator > 0) {
               block_needed = true;
            }
         }

         dist_to_elevator = abs(dist_to_elevator);
         if (block_needed) {
            dist_to_elevator += CostToTurn;
         }

         int dist = dist_traveled + CostForElevator + dist_to_elevator;
         dist = traverseGraph(n, goal, dist, dir);

         if (dist >= min_dist) continue;

         if (dist < min_dist) {
            min_dist = dist;
         }
      }

      return min_dist;
   }

   int runGraph(int nb_floors, int exit_floor, int exit_pos, std::map<int, std::vector<int>>& elevators)
   {
      // find floors with missing elevators
      std::map<int, std::vector<Node>> nodes;
      for (int i = 0; i < (nb_floors - 1); i++) {

         int flr_offset = 0;
         if (elevators.find(i) == elevators.end()) {
            std::cerr << "Missing an elevator on floor: " << i << std::endl;
            flr_offset = 1;
         }

         for (auto& e: elevators[i+flr_offset]) {
            int pos = e;
            Node node(pos, std::to_string(i) + " " + std::to_string(pos));
            const int size = (int)nodes[i].size();
            nodes[i].push_back(std::move(node));
            node = nodes[i].back();
         }

         // generate graph
         if (i > 0) {
            for (auto& n : nodes[i]) {       // this floors nodes

               // connect children
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

      // create a start node
      // using the lead clones current floor/position
      Clone clone = {Direction::Left, 0, 6};

      // get closest
      Node* startNode = nullptr;
      int min_dist = INT_MAX;
      for (auto& n: nodes[clone.floor]) {
         int dist_to_elevator = n.pos - clone.pos;

         if (clone.dir == Direction::Left) {
            if (dist_to_elevator < 0) {
               dist_to_elevator -= 1;
            }
         }
         else if (clone.dir == Direction::Right) {
            if (dist_to_elevator > 0) {
               dist_to_elevator += 1;
            }
         }

         dist_to_elevator = abs(dist_to_elevator);
         if (dist_to_elevator < min_dist) {
            min_dist = dist_to_elevator;
            startNode = &n;
         }
      }
      int d = traverseGraph(startNode, exitNode, 0, Direction::Left);
      return d;
   }
}
