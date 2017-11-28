#include "stdafx.h"
#include <vector>
#include <list>
#include <map>

using namespace std;

bool IsIn(vector<int> List, int outNId){

  int size = List.size();
  // printf("In thread %d --- %d\n", omp_get_thread_num(), size);
  for(int i = 0 ; i < size ; i++){
    if(List[i] == outNId){
      return true;
    }
  }
  return false;
}

int BFS(map<int, int*> NodeEdgeMap, int root, int goal, int* nodes, int nodesCount ){  
  map<int, bool> CheckedNodes;
  
  for(int i = 0; i < nodesCount; i++){
    CheckedNodes.insert(make_pair(nodes[i], false));
  }

  // vector<int> Checked;
  list<int> Queue;

  Queue.push_back(root);
  while(Queue.size() > 0)
  {
    int currentId = Queue.front();
    
    Queue.pop_front();
    if(currentId == goal){
      return goal;
    }
    int *childs = NodeEdgeMap[currentId];
    int childsCount = childs[0];
    for (int e = 1; e < childsCount + 1; e++) {
      int outNId = childs[e];
      // bool isin = IsIn(Checked,outNId);
      bool isin = CheckedNodes[outNId];
      if(!isin){
        // Checked.push_back(outNId);
        CheckedNodes[outNId] = true;
        Queue.push_back(outNId);
      }
    }
    // printf("- Node: %d - childsCount: %d - Queue Size: %d\n", currentId, childsCount, Queue.size());
    
  }

  return -1;
}

int main(int argc, char* argv[]) 
{
  //// what type of graph do you want to use?
  //typedef PUNGraph PGraph; // undirected graph
  typedef PNGraph PGraph;  //   directed graph
  
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  
  PGraph G = TSnap::LoadEdgeList<PNGraph>("twitter/twitter_combined.txt", 0,1);

  int nodesCount = G->GetNodes();
  int edgesCount = G->GetEdges();
  map<int, int*> NodeEdgeMap;
  int *nodes = new int[nodesCount];
  // node.insert(make_pair(2, new int[5]));
  // int * childs = node[2];
  // childs[0] = 4;
  // for (int i = 1 ; i < childs[0]+1 ; i++){
  //   childs[i] = 2;
  //   printf("Value %d -- %d\n\n", i,  childs[i]);
  // }
  
  int i = 0;
  for (TNGraph::TNodeI NI = G->BegNI(); NI < G->EndNI(); NI++) {
    int childsCount = NI.GetOutDeg();
    int currentNodeId = NI.GetId();
    nodes[i++] = currentNodeId; 
    NodeEdgeMap.insert(make_pair(currentNodeId, new int[childsCount + 1]));
    NodeEdgeMap[currentNodeId][0] = childsCount;    
    for (int e = 0; e < NI.GetOutDeg(); e++) {
      NodeEdgeMap[currentNodeId][e+1] = NI.GetOutNId(e);
      // printf("edge (%d %d)\n", currentNodeId, node[currentNodeId][e+1]);
    }
  
  }
  printf("Node Count = %d\n\n", NodeEdgeMap.size());

  /**
   * Daqui em diante não usar mais  a biblioteca do SNAP
   * 
  */

  srand (545612); // Seed the Random

  printf("Graph (%d, %d)\n", nodesCount, edgesCount);
  for (int e = 0; e < 100; e++) {
    const int NId1 = nodes[rand() % nodesCount];
    const int NId2 = nodes[rand() % nodesCount];
    printf("Search Path %d -- %d\n", NId1,  NId2); 
    TExeTm ExeBfsTm;
    printf("run time: %s (%s)\n", ExeBfsTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr()); 
    int val = BFS(NodeEdgeMap, NId1,NId2, nodes, nodesCount);
    printf("run time: %s (%s)\n", ExeBfsTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr()); 
    if(val == -1){
      printf("Not Found Path %d -- %d\n\n", NId1,  NId2); 
    }else{
      printf("Found Path %d -- %d\n\n", NId1,  NId2); 
    }
    //}
    // else {
    //   printf("  Edge %d -- %d already exists\n", NId1, NId2); }
  }
  
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
