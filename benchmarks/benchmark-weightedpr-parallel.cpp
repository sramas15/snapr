#include "Snap.h"

int main(int argc, char* argv[]) {
  if (argc < 3) { return -1; }
  int num_iterations = atoi(argv[2]);
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

  const TStr name = TStr("Weight");
  // Run num_iterations samples of WeightedPageRankMP1
  TIntFltH PRankH;
  start = omp_get_wtime();
  for (int i = 0; i < num_iterations; i++) {
    TSnap::GetWeightedPageRankMP1(G, PRankH, name, 0.85, 1e-4, 10);
  }
  end = omp_get_wtime();
  printf("%d samples of WeightedPageRankMP1 took %f seconds\n", num_iterations,
    (end-start));
  printf("Time per sample %f\n", ((end-start)/num_iterations));

  // Run num_iterations samples of WeightedPageRankMP2
  TIntFltH WPRankH;
  start = omp_get_wtime();
  for (int i = 0; i < num_iterations; i++) {
    TSnap::GetWeightedPageRankMP2(G, WPRankH, name, 0.85, 1e-4, 10);
  }
  end = omp_get_wtime();
  printf("%d samples of WeightedPageRankMP2 took %f secs\n", num_iterations,
    (end - start));
  printf("Time per sample %f\n", ((end-start)/num_iterations));
  return 0;
}
