#include "Snap.h"

TStr CreateName(TStr& Prefix, TStr& Name) {
  if (Prefix.Empty()) {
    return Name;
  } else {
    TStr NewStr = Prefix;
    NewStr += " ";
    NewStr += Name;
    return NewStr;
  }
}

template <class PGraph>
void ParseLine(PGraph& Graph, TInt NId, PJsonVal& Val, TStr& Prefix) {
  if (Val->IsObj()) {
    for (int i = 0; i < Val->GetObjKeys(); i++) {
      TStr Key;
      PJsonVal NewVal;
      Val->GetObjKeyVal(i, Key, NewVal);
      TStr NewPrefix = CreateName(Prefix, Key);
      ParseLine(Graph, NId, NewVal, NewPrefix);
    }
  } else if (Val->IsBool()) {
    Graph->AddStrAttrDatN(NId, TStr("Bool"), Prefix);
  } else if (Val->IsNum()) {
    //Graph->AddStrAttrDatN(NId, Val->GetStr(), Prefix);
    double attr = Val->GetNum();
    TFlt DblVal(attr);
    Graph->AddStrAttrDatN(NId, DblVal.GetStr(), Prefix);
  } else if (Val->IsStr()) {
    PJsonVal ValStr = TJsonVal::GetValFromStr(Val->GetStr());
    if (ValStr->IsObj() || ValStr->IsNum()) {
      ParseLine(Graph, NId, ValStr, Prefix);
    } else {
      Graph->AddStrAttrDatN(NId, Val->GetStr(), Prefix);
    }
  } else if (Val->IsArr()) {
    for (int i = 0; i < Val->GetArrVals(); i++) {
      TStr NewPrefix = Prefix;
      NewPrefix += "-";
      NewPrefix += TInt(i).GetStr();
      PJsonVal ArrVal = Val->GetArrVal(i);
      ParseLine(Graph, NId, ArrVal, NewPrefix);
    }
    //Graph->AddStrAttrDatN(NId, TStr("arr"), Prefix);
  } else if (Val->IsNull()) {
    Graph->AddStrAttrDatN(NId, TStr::GetNullStr(), Prefix);
  }
}

template <class PGraph>
void ParseLine(PGraph& Graph, TInt NId, TStr& Line, TStr& Prefix) {
  PJsonVal Val = TJsonVal::GetValFromStr(Line);
  ParseLine(Graph, NId, Val, Prefix);
}

template <class PGraph>
void FindFreqAndReadGraph(PGraph& Net) {
  double start_freq = omp_get_wtime();
  THash<TStr, TPair<TInt, TInt> > Freq;
  Net->GetKeyFreq(Freq);
  double end = omp_get_wtime();
  FILE * f = fopen("attr/results2.txt", "w");
  int totalAttrs = 0;
  int totalNulls = 0;
  THash<TStr, TPair<TInt, TInt> >::TIter FreqI = Freq.BegI();
  while (!FreqI.IsEnd()){
    int val = FreqI.GetDat().Val1;
    int nulls = FreqI.GetDat().Val2;
    totalNulls += nulls;
    totalAttrs += val;
    fprintf(f, "%d\t%d\t%s\n", val, nulls, FreqI.GetKey().CStr());
    FreqI++;
  }
  fprintf(f, "%d\t%d\t__count__\n", totalAttrs, totalNulls);
  fclose(f);
  printf("Key count: %f\n", (end - start_freq));
}

int main(int argc, char* argv[]) {
  PNSparseNet Net = PNSparseNet::New();
  double start = omp_get_wtime();
  TStr FileName("test2.json");
  TSsFmt Separator = ssfWhiteSep;
  TSsParser Parser(FileName, Separator);
  int i = 0;
  while (Parser.Next()) {
    TChA Line = Parser.GetLnStr();
    TStr ConvLine(Line.CStr());
    TStr Prefix("");
    Net->AddNode(i);
    ParseLine(Net, TInt(i), ConvLine, Prefix);
    i++;
  }
  double start_freq = omp_get_wtime();
  THash<TStr, TPair<TInt, TInt> > Freq;
  Net->GetKeyFreq(Freq);
  double end = omp_get_wtime();
  FILE * f = fopen("results.txt", "w");
  int totalAttrs = 0;
  int totalNulls = 0;
  THash<TStr, TPair<TInt, TInt> >::TIter FreqI = Freq.BegI();
  while (!FreqI.IsEnd()){
    int val = FreqI.GetDat().Val1;
    int nulls = FreqI.GetDat().Val2;
    totalNulls += nulls;
    totalAttrs += val;
    fprintf(f, "%d\t%d\t%s\n", val, nulls, FreqI.GetKey().CStr());
    FreqI++;
  }
  fprintf(f, "%d\t%d\t__count__\n", totalAttrs, totalNulls);
  fclose(f);
  printf("Parsing: %f\n", (start_freq - start));
  printf("Key count: %f\n", (end - start_freq));
  TFOut SaveF("attr/save.bin");
  start = omp_get_wtime();
  Net->Save(SaveF);
  SaveF.Flush();
  end = omp_get_wtime();
  printf("Time to save: %f\n", (end-start));
  TFIn LoadF("attr/save.bin");
  start = omp_get_wtime();
  PNSparseNet Net2 = TNSparseNet::Load(LoadF);
  end = omp_get_wtime();
  printf("Time to load: %f\n", (end-start));
  FindFreqAndReadGraph(Net2);
  //Net->Dump();
  return 0;
}
