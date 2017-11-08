#include "stdafx.h"
#include <vector>


int BFS(const PNGraph& G, int root, int goal) {

  // we can traverse the edges also like this
  TVec<int> Checked;    
  TVec<int> Queue;

  Queue.Add(root);
  while(!Queue.Empty())
  {
    int currentId = Queue[0];
    PNGraph::TObj::TNodeI currentNI = G->GetNI(currentId);
    Queue.Del(0);
    if(currentId == goal){
      return goal;
    }
    for (int e = 0; e < currentNI.GetOutDeg(); e++) {
      int outNId = currentNI.GetOutNId(e);
      if(!Checked.IsIn(outNId)){
        Checked.Add(outNId);
        Queue.Add(outNId);
      }
    }
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
