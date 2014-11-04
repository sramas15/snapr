#include "Snap.h"

int main(int argc, char* argv[]) {
  TTableContext Context;
  // Create schema.
  Schema PageRankS;
  PageRankS.Add(TPair<TStr,TAttrType>(TStr("Source"), atInt));
  PageRankS.Add(TPair<TStr,TAttrType>(TStr("Destination"), atInt)); 
  PageRankS.Add(TPair<TStr,TAttrType>(TStr("Weight"), atFlt));
  TIntV RelevantCols;
  RelevantCols.Add(TInt(0)); RelevantCols.Add(TInt(1)); RelevantCols.Add(TInt(2));

  PTable P = TTable::LoadSS(PageRankS, "table/higgs_twitter.tsv",
   Context, RelevantCols);

  // Convert table to network
  TStrV V;
  TStrV VE;
  //VE.Add(TStr("Weight"));
  PNEANet G = TSnap::ToNetwork<PNEANet>(P, TStr("Source"),
   TStr("Destination"), V, V, VE, aaLast);

  for (TNEANet::TEdgeI It = G->BegEI(); It < G->EndEI(); It++) {
    G->AddFltAttrDatE(It.GetId(), 1, TStr("Weight"));
  }

  // Run 50 samples of PageRank
  TIntFltH PRankH;
  TExeTm tl;
  for (int i = 0; i < 1; i++) {
    TSnap::GetPageRank(G, PRankH, 0.85, 1e-4, 10);
  }
  double tm = tl.GetSecs();
  printf("1 sample of PageRank (each 10 iterations) took %f seconds\n", tm);


  // Run 50 samples of WeightedPageRank
  const TStr name = TStr("Weight");
  TIntFltH WPRankH;
  TExeTm tl2;
  for (int i = 0; i < 1; i++) {
    TSnap::GetWeightedPageRank(G, WPRankH, name, 0.85, 1e-4, 10);
  }
  double tm2 = tl2.GetSecs();
  printf("1 sample of WeightedPageRank (each 10 iterations) took %f secs\n", tm2);
  /*double sum = 0;
  int num = 0;
  for (TIntFltH::TIter It = PRankH.BegI(); It < PRankH.EndI(); It++) {
    printf("PR %d %f\n", It.GetKey(), It.GetDat());
    num++;
    if (num == 10) break;
  }
  num = 0;
  for (TIntFltH::TIter It = WPRankH.BegI(); It < WPRankH.EndI(); It++) {
    if (It.GetDat() < 0)
      printf("error: less than 0");
    sum += It.GetDat();
    printf("WPR %d %f\n", It.GetKey(), It.GetDat());
    num++;
    if (num == 10) break;
  }
  printf("Sum: %f", sum);*/
}

