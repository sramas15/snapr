#include <stdio.h>
#include "Snap.h"

int main(int argc, char* argv[]) {
  TFIn LoadF("attr/save-vec.bin");
  double start = omp_get_wtime();
  PNSparseNet Net = TNSparseNet::Load(LoadF);
  double end = omp_get_wtime();
  printf("Time to read from binary: %f\n", (end-start));

  start = omp_get_wtime();
  TStr Name("created-0");
  Net->ConvertStrAttr(Name, 0);  
  end = omp_get_wtime();
  int val = Net->GetIntAttrDatN(0, Name);
  printf("Time to convert single attr: %f\n", (end-start));
  printf("Value for node 0 %d\n", val);
}
