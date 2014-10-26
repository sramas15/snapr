#include "Snap.h"

int main(int argc, char* argv[]) {
  TTableContext Context;
  // Create schema.
  Schema PageRankS;
  PageRankS.Add(TPair<TStr,TAttrType>("Weight", atFlt));
  PageRankS.Add(TPair<TStr,TAttrType>("Source", atInt));
  PageRankS.Add(TPair<TStr,TAttrType>("Destination", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0); RelevantCols.Add(1); RelevantCols.Add(2);

  PTable P = TTable::LoadSS(PageRankS, "table/fb-unweighted.tsv",
   Context, RelevantCols);

  // Convert table to network
  TStrV V;
  TStrV VE;
  VE.Add(TStr("Weight"));
  PNEANet G = TSnap::ToNetwork<PNEANet>(P, TStr("Source"),
   TStr("Destination"), V, V, VE, aaLast);

  // Run 30 samples of PageRank
  TIntFltH PRankH;
  TExeTm tl;
  for (int i = 0; i < 30; i++) {
    TSnap::GetPageRank(G, PRankH, 0.85, 1e-4, 10);
  }
  double tm = tl.GetSecs();
  printf("30 samples of PageRank (each 10 iterations) took %f seconds\n", tm);


  // Run 30 samples of WeightedPageRank
  const TStr name = TStr("Weight");
  TIntFltH WPRankH;
  TExeTm tl2;
  for (int i = 0; i < 30; i++) {
    TSnap::GetWeightedPageRank(G, WPRankH, name, 0.85, 1e-4, 10);
  }
  double tm2 = tl2.GetSecs();
  printf("30 samples of WeightedPageRank (each 10 iterations) took %f secs\n", tm2);
}

