#include "stdafx.h"
#include <vector>
#include <list>
#include <iostream>
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

int BFS(const PNGraph& G, int root, int goal) {

  // we can traverse the edges also like this
  vector<int> Checked;
  list<int> Queue;
  // TVec<int> Checked;    
  // TVec<int> Queue;

  // Queue.Add(root);
  Queue.push_back(root);
  // while(!Queue.Empty())
  while(Queue.size() > 0)
  {
    // int currentId = Queue[0];
    // Queue.Del(0);

    int currentId = Queue.front();
    Queue.pop_front();
    PNGraph::TObj::TNodeI currentNI = G->GetNI(currentId);
    if(currentId == goal){
        return goal;
    }

    int outDegCount = currentNI.GetOutDeg();
    int nthreads = 2;
    if(outDegCount < nthreads){
      nthreads = outDegCount;
    }
    if(nthreads == 0){
      continue;
    }
    int i = 0;
    printf("Node %d  Num of threads %d  outDegCount = %d  ExecThreads = %d\n",currentId ,2 , outDegCount, nthreads);
    #pragma omp parallel for num_threads(nthreads) shared(Checked, Queue, currentNI, i) 
    for (i = 0; i < outDegCount; i++) {
      printf("In thread %d - i:%d\n", omp_get_thread_num(), i);
      int outNId = currentNI.GetOutNId(i);
      bool isin = IsIn(Checked,outNId);
      if(!isin){
        Checked.push_back(outNId);
        Queue.push_back(outNId);
      }
    }
    printf("- Node %d  Num of threads %d  outDegCount = %d  ExecThreads = %d\n",currentId ,2 , outDegCount, nthreads);
  }

  return -1;
}



int main(int argc, char* argv[]) {

  //// what type of graph do you want to use?
  //typedef PUNGraph PGraph; // undirected graph
  typedef PNGraph PGraph;  //   directed graph
  
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  
  PGraph G = TSnap::LoadEdgeList<PNGraph>("twitter/twitter_combined.txt", 0,1);
  printf("Graph (%d, %d)\n", G->GetNodes(), G->GetEdges());
  for (int e = 0; e < 10; e++) {
    const int NId1 = G->GetRndNId();
    const int NId2 = G->GetRndNId();
    printf("Search Path %d -- %d\n", NId1,  NId2); 
    TExeTm ExeBfsTm;
    printf("run time: %s (%s)\n", ExeBfsTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr()); 
    int val = BFS(G, NId1,NId2);
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
