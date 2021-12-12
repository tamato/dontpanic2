#include <iostream>
#include <map>
#include <vector>
#include <climits>

static const int CostToTurn {1};

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
