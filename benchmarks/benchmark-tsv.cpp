#include <stdio.h>
#include "Snap.h"

int main(int argc, char* argv[]) {
  TFIn LoadF("attr/save-vec.bin");
  double start = omp_get_wtime();
  PNSparseNet Net = TNSparseNet::Load(LoadF);
  double end = omp_get_wtime();
  printf("Time to read from binary: %f\n", (end-start));

  start = omp_get_wtime();
  FILE *fp = fopen("attr/output-vec.tsv", "w");
  Net->ConvertToTSV(fp, 1000);
  end = omp_get_wtime();
  printf("Time to convert to TSV: %f\n", (end-start));
}
