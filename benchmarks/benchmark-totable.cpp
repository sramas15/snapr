#include <stdio.h>
#include "Snap.h"

int main(int argc, char* argv[]) {
  TFIn LoadF("attr/save-hash.bin");
  double start = omp_get_wtime();
  PNEANetSparse2 Net = TNEANetSparse2::Load(LoadF);
  double end = omp_get_wtime();
  printf("Time to read from binary: %f\n", (end-start));

  start = omp_get_wtime();
  TStr Name("clientId");
  //int type = Net->GetAttrType(Name);
  //printf("type: %d\n", type);
  TSnap::ToTable(Net, 5);
  end = omp_get_wtime();
  printf("Time to convert to table: %f\n", (end-start));
}
