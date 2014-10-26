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

  PTable P = TTable::LoadSS(PageRankS, "table/fb-unweighted.tsv", Context, RelevantCols);

  TStrV V;
  TStrV VE;
  VE.Add(TStr("Weight"));
  TExeTm tl;
  PNEANet G = TSnap::ToNetwork<PNEANet>(P, TStr("Source"), TStr("Destination"), V, V, VE, aaLast);
  double tm = tl.GetSecs();
  printf("Table conversion took %f secs\n", tm);
}