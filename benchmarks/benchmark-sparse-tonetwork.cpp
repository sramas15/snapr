#include "Snap.h"

int main(int argc, char* argv[]) {
  TTableContext Context;
  Schema NetworkS;
  NetworkS.Add(TPair<TStr, TAttrType>("Year", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("Month", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("DayOfMonth", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("DayOfWeek", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("DepTime", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("CRSDepTime", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("ArrTime", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("CRSArrTime", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("UniqueCarrier", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("FlightNum", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("TailNum", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("ActualElapsedTime", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("CRSElapsedTime", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("AirTime", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("ArrDelay", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("DepDelay", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("Origin", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("Dest", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("Distance", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("TaxiIn", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("TaxiOut", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("Cancelled", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("CancellationCode", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("Diverted", atStr));
  NetworkS.Add(TPair<TStr, TAttrType>("CarrierDelay", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("WeatherDelay", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("NASDelay", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("SecurityDelay", atInt));
  NetworkS.Add(TPair<TStr, TAttrType>("LateAircraftDelay", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0); RelevantCols.Add(1); RelevantCols.Add(2);RelevantCols.Add(3); RelevantCols.Add(4); RelevantCols.Add(5);
  RelevantCols.Add(6); RelevantCols.Add(7); RelevantCols.Add(8); RelevantCols.Add(9); RelevantCols.Add(10); RelevantCols.Add(11);
  RelevantCols.Add(12); RelevantCols.Add(13); RelevantCols.Add(14); RelevantCols.Add(15); RelevantCols.Add(16); RelevantCols.Add(17);
  RelevantCols.Add(18); RelevantCols.Add(19); RelevantCols.Add(20); RelevantCols.Add(21); RelevantCols.Add(22); RelevantCols.Add(23);
  RelevantCols.Add(24); RelevantCols.Add(25); RelevantCols.Add(26); RelevantCols.Add(27); RelevantCols.Add(28);
  PTable P = TTable::LoadSS(NetworkS, "table/2007.csv",
  Context, RelevantCols, ',', false);

  TStrV SV;
  TStrV DV;
  TStrV VE;
  double start = omp_get_wtime();
  PNSparseNet G = TSnap::ToNetwork<PNSparseNet>(P, TStr("Origin"),
   TStr("Dest"), SV, DV, VE, aaLast);
  double end = omp_get_wtime();
  printf("Conversion time without attributes %f\n", (end-start));

  start = omp_get_wtime();
  TSnap::AddAttrTable<PNSparseNet>(P, G, TStr("Origin"),
   TStr("Dest"), SV, DV, VE, aaLast);
  end = omp_get_wtime();
  printf("Conversion time with attributes %f\n", (end-start));

  /*(PTable Table, PGraph& Graph, const TStr& SrcCol, const TStr& DstCol, 
  TStrV& SrcAttrV, TStrV& DstAttrV, TStrV& EdgeAttrV, TAttrAggr AggrPolicy, TInt DefaultInt,
  TFlt DefaultFlt, TStr DefaultStr)*/

}