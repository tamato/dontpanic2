
namespace collections {

   struct Node {
      int id {INT_MAX};
      int pos {INT_MAX};
      Node(int pos_, int id_): pos(pos_), id(id_) {}
      Node() {}

      std::vector<int> childNodes;

      void addChild(int childId) {
         childNodes.push_back(childId);
      }

      bool operator==(const Node& other) {
         return (this->id == other.id);
      }
   };

   int traverseGraph(std::map<int, std::vector<Node>> nodes, int goal, int dist_traveled)
   {
      int min_dist = INT_MAX;
      const int nodeId = nodes.second.id;
      if (nodeId == goal) {
         std::cout << "Goal found!" << std::endl;
         min_dist = dist_traveled;
         return min_dist;
      }

      const int current_floor = nodes.first;
      auto& childNodes = nodes.second.childNodes;
      for (auto& childNode: childNodes) {
         int dist = dist_traveled + abs(nodes[current_floor].pos - childNode.pos);
         dist = traverseGraph(childNode, 
      }

         const auto next = edges[i].id;
         if (next < g._edges.size())
            traverseGraph(shortest, g, edges[i].id, goal, dist, p, ds, spath);

         if (edges[i].id == goal) {
            if (shortest > dist) {
               shortest = dist;
               spath = p;
            }
         }
      }

      return 0;
   }

   int runGraph(int nb_floors, int exit_floor, int exit_pos, std::map<int, std::vector<int>>& elevators) 
   {
      // find floors with missing elevators
      std::map<int, std::vector<Node>> nodes;
      int nodeID = 0;
      for (int flr = 0; flr < (nb_floors - 1); flr++) {

         int flr_offset = 0;
         if (elevators.find(flr) == elevators.end()) {
            std::cerr << "Missing an elevator on floor: " << flr << std::endl;
            flr_offset = 1;
         }

         for (auto& e: elevators[flr+flr_offset]) {
            int pos = e;
            Node node(pos, nodeID++);
            nodes[flr].push_back(std::move(node));
         }

         // generate graph
         if (flr > 0) {
            for (const auto& n : nodes[flr]) {       // this floors nodes
               for (auto& pn : nodes[flr-1]) { // previous floor nodes
                  pn.addChild(n.id);
               }
            }
         }
      }

      // create a start node
      Node startNode(6, nodeID++);
      for (auto& n: nodes[0]) {
         startNode.addChild(n.id);
      }
      nodes[0].push_back(std::move(startNode));

      // Pop in the exit
      Node exitNode(exit_pos, nodeID);
      for (auto& n : nodes[exit_floor - 1]) {
         n.addChild(exitNode.id);
      }
      nodes[exit_floor].push_back(std::move(exitNode));

      int d = traverseGraph(nodes, nodeID, 0);
      return d;
   }
}
