#include <iostream>
#include <map>
#include <vector>
#include <climits>

static const int CostToTurn {1};

struct Node {
   int id;
   int pos;
   Node(int pos_): id(Node::node_counter++), pos(pos_) {}
   static int node_counter;
};
int Node::node_counter = 0;

struct Edge {
   Node _start;
   std::vector<Node> _ends;
};

struct Graph 
{
   std::vector<Edge> _edgeLs;
   void addEdge(Edge edge) {
      _edgeLs.push_back(edge);
   }
};

int traverseGraph2(int& shortest, Graph& g, int start, int goal, int dist_traveled, const std::string& path_followed, const std::string& sum, std::string& spath)
{
   const auto edges = g._edgeLs[start]._ends;
   for (int i=0; i < (int)edges.size(); ++i) {
      
      int dist = dist_traveled + abs(g._edgeLs[start]._start.pos - edges[i].pos);
      std::string p = path_followed + std::to_string(start) + std::string(" -> ") + std::to_string(edges[i].id) + ", ";
      std::string ds = sum + std::to_string(g._edgeLs[start]._start.pos) + std::string(" - ") + std::to_string(edges[i].pos) + std::string(" = ") + std::to_string(abs(g._edgeLs[start]._start.pos - edges[i].pos)) + ", ";

      const auto next = edges[i].id;
      if (next < g._edgeLs.size()) 
         traverseGraph2(shortest, g, edges[i].id, goal, dist, p, ds, spath);

      if (edges[i].id == goal) {
         // std::cout << "Found goal, distance: " << dist << std::endl;
         // std::cout << "Path: " + p << std::endl;
         // std::cout << "Dists: " + ds << std::endl;
         // std::cout << std::endl;
         if (shortest > dist) {
            shortest = dist;
            spath = p;
         }
      }
   }

   return 0;
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
   Node startNode(6);
   Node n0(2);
   Node n1(3);
   Node n2(8);
   Node n3(1);
   Node n4(6);
   Node n5(2);
   Node n6(1);
   Node exitNode(3);
   Node n8(5);

   Edge start = {startNode, {n0}};
   Edge e0 = {n0, {n1, n2}};
   Edge e1 = {n1, {n3, n4}};
   Edge e2 = {n2, {n3, n4}};
   Edge e3 = {n3, {n5}};
   Edge e4 = {n4, {n5}};
   Edge e5 = {n5, {n6, exitNode, n8}};

   Graph g;
   g.addEdge(start);
   g.addEdge(e0);
   g.addEdge(e1);
   g.addEdge(e2);
   g.addEdge(e3);
   g.addEdge(e4);
   g.addEdge(e5);

   std::string mypath;
   std::string spath;
   std::string sum;
   int shortest {INT_MAX};
   traverseGraph2(shortest, g, 0, 7, 0, mypath, sum, spath);
   std::cout << "Shortest path found: " << shortest << std::endl;
   std::cout << "Shortest path found: " << spath << std::endl;
   return 0;
}
