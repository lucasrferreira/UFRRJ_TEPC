#include "stdafx.h"
#include <vector>
#include <list>
#include <map>
#include <iostream>

#define NUM_THREADS 2
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

void PrintAllQueue(list<int> List){
    std::cout <<"\n\n\n---- LISTA DE TAMANHO:" <<  List.size() << "\n";
    // for (int v : List)
    //     std::cout <<"---- " <<  v << "\n";
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
    
    int nthreads = NUM_THREADS;
    if(childsCount < nthreads){
      nthreads = childsCount;
    }
    int* visitedCounts = new int[nthreads];
    int** visited = new int*[nthreads];
    // #pragma omp parallel for num_threads(nthreads) shared(visited, visitedCounts)
    for (int i = 0; i < nthreads; ++i)
    {
      visitedCounts[i] = 0;
      visited[i] = new int[childsCount];
    }
    #pragma omp parallel for num_threads(nthreads) shared(CheckedNodes)
    for (int i = 1; i < childsCount + 1; i++) {
      int outNId = childs[i];
      int threadNum = omp_get_thread_num();
      bool isin = CheckedNodes[outNId];
      if(!isin){
        CheckedNodes[outNId] = true;
        visited[threadNum][visitedCounts[threadNum]] = outNId;
        visitedCounts[threadNum] += 1;
      }
    }
    for (int i = 0 ; i < nthreads ; i++){
      for(int j = 0; j < visitedCounts[i]; j++){
        Queue.push_back(visited[i][j]);
      }
      delete visited[i];
    }
    delete visited;
    delete visitedCounts;

    // printf("- Node: %d - childsCount: %d - Queue Size: %d - %d\n", currentId, childsCount, Queue.size(), Queue.empty());
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
