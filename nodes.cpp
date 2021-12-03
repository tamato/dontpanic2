#include <iostream>
#include <map>
#include <vector>
#include <climits>

static const int CostToTurn {1};

struct Node {
   std::string name;
   int pos;
   Node(int pos_, const std::string&& name_): pos(pos_), name((name_)) {}

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
   // std::cout << "Node: " << nodes.name << std::endl;
   int sdist = INT_MAX;
   if (nodes.childNodes.empty()) {
      if (nodes == goal) {
         std::cout << "Found Goal" << std::endl;
         sdist = dist_traveled;
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

/**************
  -= Map =-

.......... 
.e.x.e.... 
..e....... 
.e....e... 
...e....e. 
..e...s... 

**************/

int main()
{
   // Create nodes
   Node startNode(6, "start");
   Node n0(2, "e00");
   Node n1(3, "e10");
   Node n2(8, "e11");
   Node n3(1, "e20");
   Node n4(6, "e21");
   Node n5(2, "e30");
   Node n6(1, "e40");
   Node exitNode(3, "exit");
   Node n8(5, "e41");

   // Form graph
   startNode.addChild(&n0);

   n0.addChild(&n1);
   n0.addChild(&n2);

   n1.addChild(&n3);
   n1.addChild(&n4);

   n2.addChild(&n3);
   n2.addChild(&n4);

   n3.addChild(&n5);

   n4.addChild(&n5);

   n5.addChild(&n6);
   n5.addChild(&exitNode);
   n5.addChild(&n8);

   int d = traverseGraph(startNode, exitNode, 0);
   std::cout << "Distance: " << d << std::endl;
   return 0;
}
