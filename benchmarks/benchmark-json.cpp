#include "Snap.h"

TStr CreateName(TStr& Prefix, TStr& Name) {
  if (Prefix.Empty()) {
    return Name;
  } else {
    TStr NewStr = Prefix;
    NewStr += ":";
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
    double attr = Val->GetNum();
    TFlt DblVal(attr);
    Graph->AddFltAttrDatN(NId, DblVal, Prefix);
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

int main(int argc, char* argv[]) {
  TStr FileName("attr/test.json");
  TSsFmt Separator = ssfWhiteSep;
  TSsParser Parser(FileName, Separator);
  PNSparseNet Net = PNSparseNet::New();
  int i = 0;
  while (Parser.Next()) {
    TChA Line = Parser.GetLnStr();
    TStr ConvLine(Line.CStr());
    TStr Prefix("");
    Net->AddNode(i);
    ParseLine(Net, TInt(i), ConvLine, Prefix);
    i++;
  }
  THash<TStr, TInt> Freq;
  Net->GetKeyFreq(Freq);
  THash<TStr, TInt>::TIter FreqI = Freq.BegI();
  while (!FreqI.EndI()){
  	int val = FreqI.GetDat();
  	printf("Attr: %s Frequency: %d");
  	FreqI++;
  }
  //Net->Dump();
  return 0;
}
