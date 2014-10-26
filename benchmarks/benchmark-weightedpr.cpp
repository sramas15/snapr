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

  PTable UnweightedTable = TTable::LoadSS(PageRankS, "table/fb-unweighted.tsv", Context, RelevantCols);
  PTable WeightedTable = TTable::LoadSS(PageRankS, "table/fb-unweighted.tsv", Context, RelevantCols);

  // Convert table to network
  TStrV V;
  TStrV VE;
  VE.Add(TStr("Weight"));
  PNEANet UnweightedG = TSnap::ToNetwork<PNEANet>(UnweightedTable, TStr("Source"), TStr("Destination"),
   V, V, VE, aaLast);
  PNEANet WeightedG = TSnap::ToNetwork<PNEANet>(WeightedTable, TStr("Source"), TStr("Destination"),
   V, V, VE, aaLast);

  // Run 30 samples of WeightedPageRank with all edge weights = 1
  const TStr name = TStr("Weight");
  TIntFltH UWPRankH;
  TExeTm tl1;
  for (int i = 0; i < 30; i++) {
    TSnap::GetWeightedPageRank(UnweightedG, UWPRankH, name, 0.85, 1e-4, 10);
  }
  double tm1 = tl1.GetSecs();
  printf("30 samples of WeightedPageRank (each 10 iterations) took %f secs\n", tm1);

  // Run 30 samples of WeightedPageRank with not all edge weights = 1
  TIntFltH WPRankH;
  TExeTm tl2;
  for (int i = 0; i < 30; i++) {
    TSnap::GetWeightedPageRank(WeightedG, WPRankH, name, 0.85, 1e-4, 10);
  }
  double tm2 = tl2.GetSecs();
  printf("30 samples of WeightedPageRank (each 10 iterations) took %f secs\n", tm2);
}