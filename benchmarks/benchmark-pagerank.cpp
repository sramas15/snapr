#include "Snap.h"

double GetSystemTime();

int main(int argc, char* argv[]) {
  if (argc < 2) { return -1; }
  TTableContext Context;
  // Create schema.
  Schema PageRankS;
  PageRankS.Add(TPair<TStr,TAttrType>("Source", atInt));
  PageRankS.Add(TPair<TStr,TAttrType>("Destination", atInt));
  PageRankS.Add(TPair<TStr,TAttrType>("Weight", atFlt));
  TIntV RelevantCols;
  RelevantCols.Add(0); RelevantCols.Add(1); RelevantCols.Add(2);

  double start = omp_get_wtime();
  TStr Path(argv[1]);
  PTable P = TTable::LoadSS(PageRankS, Path,
   Context, RelevantCols, '\t', false);
  double end = omp_get_wtime();
  printf("Input time %f\n", (end-start));

  // Convert table to network
  TStrV V;
  TStrV VE;
  VE.Add(TStr("Weight"));
  start = omp_get_wtime();
  PNEANet G = TSnap::ToNetwork<PNEANet>(P, TStr("Source"),
   TStr("Destination"), V, V, VE, aaLast);
  end = omp_get_wtime();
  printf("Conversion time %f\n", (end-start));

  // Run 30 samples of PageRank
  TIntFltH PRankH;
  start = omp_get_wtime();
  for (int i = 0; i < 80; i++) {
    TSnap::GetPageRank(G, PRankH, 0.85, 1e-4, 10);
  }
  end = omp_get_wtime();
  printf("80 samples of PageRank took %f seconds\n", (end-start));
  printf("Time per sample %f\n", ((end-start)/80));

  // Run 30 samples of WeightedPageRank
  const TStr name = TStr("Weight");
  TIntFltH WPRankH;
  start = omp_get_wtime();
  for (int i = 0; i < 80; i++) {
    TSnap::GetWeightedPageRank(G, WPRankH, name, 0.85, 1e-4, 10);
  }
  end = omp_get_wtime();
  printf("80 samples of WeightedPageRank took %f secs\n", (end-start));
  printf("Time per sample %f\n", ((end-start)/80));
  return 0;
}

