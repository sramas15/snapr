/////////////////////////////////////////////////
// Attribute Node Edge Network
bool TNEANetSparse2::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TNEANetSparse2::TNet, Flag);
}

bool TNEANetSparse2::TNodeI::IsInNId(const int& NId) const {
  const TNode& Node = Graph->NodeH.GetDat(NId);
  for (int edge = 0; edge < Node.GetInDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetInEId(edge)).GetSrcNId())
      return true;
  }
  return false;
}

bool TNEANetSparse2::TNodeI::IsOutNId(const int& NId) const {
  const TNode& Node = Graph->NodeH.GetDat(NId);
  for (int edge = 0; edge < Node.GetOutDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetOutEId(edge)).GetDstNId())
      return true;
  }
  return false;
}

bool TNEANetSparse2::NodeAttrIsDeleted(const int& NId, const TStr& attr) const {
  return NodeAttrIsIntDeleted(NId, attr) && NodeAttrIsStrDeleted(NId, attr) && NodeAttrIsFltDeleted(NId, attr);
}

bool TNEANetSparse2::NodeAttrIsIntDeleted(const int& NId, const TStr& attr) const {
  int AttrId = GetAttrIdN(attr, IntType);
  TPair<TInt, TInt> key(NId, AttrId);
  return IntAttrsN.IsKey(key);
}

bool TNEANetSparse2::NodeAttrIsStrDeleted(const int& NId, const TStr& attr) const {
  int AttrId = GetAttrIdN(attr, StrType);
  TPair<TInt, TInt> key(NId, AttrId);
  return StrAttrsN.IsKey(key);
}

bool TNEANetSparse2::NodeAttrIsFltDeleted(const int& NId, const TStr& attr) const {
  int AttrId = GetAttrIdN(attr, FltType);
  TPair<TInt, TInt> key(NId, AttrId);
  return FltAttrsN.IsKey(key);
}

bool TNEANetSparse2::EdgeAttrIsDeleted(const int& EId, const TStr& attr) const {
  return EdgeAttrIsIntDeleted(EId, attr) && EdgeAttrIsStrDeleted(EId, attr) && EdgeAttrIsFltDeleted(EId, attr);
}

bool TNEANetSparse2::EdgeAttrIsIntDeleted(const int& EId, const TStr& attr) const {
  int AttrId = GetAttrIdE(attr, IntType);
  TPair<TInt, TInt> key(EId, AttrId);
  return IntAttrsE.IsKey(key);
}

bool TNEANetSparse2::EdgeAttrIsStrDeleted(const int& EId, const TStr& attr) const {
  int AttrId = GetAttrIdE(attr, StrType);
  TPair<TInt, TInt> key(EId, AttrId);
  return StrAttrsE.IsKey(key);
}

bool TNEANetSparse2::EdgeAttrIsFltDeleted(const int& EId, const TStr& attr) const {
  int AttrId = GetAttrIdE(attr, FltType);
  TPair<TInt, TInt> key(EId, AttrId);
  return FltAttrsE.IsKey(key);
}

int TNEANetSparse2::AddNode(int NId) {
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  NodeH.AddDat(NId, TNode(NId));
  return NId;
}

void TNEANetSparse2::DelNode(const int& NId) {
  
  const TNode& Node = GetNode(NId);
  for (int out = 0; out < Node.GetOutDeg(); out++) {
    const int EId = Node.GetOutEId(out);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetSrcNId() == NId);
    GetNode(Edge.GetDstNId()).InEIdV.DelIfIn(EId);
    EdgeH.DelKey(EId);
  }
  for (int in = 0; in < Node.GetInDeg(); in++) {
    const int EId = Node.GetInEId(in);
    const TEdge& Edge = GetEdge(EId);
    IAssert(Edge.GetDstNId() == NId);
    GetNode(Edge.GetSrcNId()).OutEIdV.DelIfIn(EId);
    EdgeH.DelKey(EId);
  }
  NodeH.DelKey(NId);


  //UPDATE

}

int TNEANetSparse2::AddEdge(const int& SrcNId, const int& DstNId, int EId) {

  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %d already exists", EId));
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  EdgeH.AddDat(EId, TEdge(EId, SrcNId, DstNId));
  GetNode(SrcNId).OutEIdV.AddSorted(EId);
  GetNode(DstNId).InEIdV.AddSorted(EId);
  return EId;
}

void TNEANetSparse2::DelEdge(const int& EId) {

  IAssert(IsEdge(EId));
  const int SrcNId = GetEdge(EId).GetSrcNId();
  const int DstNId = GetEdge(EId).GetDstNId();
  GetNode(SrcNId).OutEIdV.DelIfIn(EId);
  GetNode(DstNId).InEIdV.DelIfIn(EId);
  EdgeH.DelKey(EId);

  //UPDATE
}

// delete all edges between the two nodes
void TNEANetSparse2::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  int EId = 0;
  bool Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  IAssert(Edge); // there is at least one edge
  while (Edge) {
    DelEdge(EId);
    Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  }
}

bool TNEANetSparse2::IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir) const {
  const TNode& SrcNode = GetNode(SrcNId);
  for (int edge = 0; edge < SrcNode.GetOutDeg(); edge++) {
    const TEdge& Edge = GetEdge(SrcNode.GetOutEId(edge));
    if (DstNId == Edge.GetDstNId()) {
      EId = Edge.GetId();
      return true;
    }
  }
  if (! IsDir) {
    for (int edge = 0; edge < SrcNode.GetInDeg(); edge++) {
      const TEdge& Edge = GetEdge(SrcNode.GetInEId(edge));
      if (DstNId == Edge.GetSrcNId()) {
        EId = Edge.GetId();
        return true;
      }
    }
  }
  return false;
}

void TNEANetSparse2::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N));
  }
}

void TNEANetSparse2::GetEIdV(TIntV& EIdV) const {
  EIdV.Gen(GetEdges(), 0);
  for (int E=EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    EIdV.Add(EdgeH.GetKey(E));
  }
}

void TNEANetSparse2::Defrag(const bool& OnlyNodeLinks) {
  for (int kid = NodeH.FFirstKeyId(); NodeH.FNextKeyId(kid); ) {
    TNode& Node = NodeH[kid];
    Node.InEIdV.Pack();  Node.OutEIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
  if (! OnlyNodeLinks && ! EdgeH.IsKeyIdEqKeyN()) { EdgeH.Defrag(); }
}

bool TNEANetSparse2::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.OutEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Out-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! Node.InEIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("In-edge list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    // check out-edge ids
    int prevEId = -1;
    for (int e = 0; e < Node.GetOutDeg(); e++) {
      if (! IsEdge(Node.GetOutEId(e))) {
        const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetOutEId(e), Node.GetId());
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetOutEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetOutEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetOutEId(e);
    }
    // check in-edge ids
    prevEId = -1;
    for (int e = 0; e < Node.GetInDeg(); e++) {
      if (! IsEdge(Node.GetInEId(e))) {
      const TStr Msg = TStr::Fmt("Out-edge id %d of node %d does not exist.",  Node.GetInEId(e), Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      if (e > 0 && prevEId == Node.GetInEId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplidate out-edge id %d.", Node.GetId(), Node.GetInEId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
      }
      prevEId = Node.GetInEId(e);
    }
  }
  for (int E = EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    const TEdge& Edge = EdgeH[E];
    if (! IsNode(Edge.GetSrcNId())) {
      const TStr Msg = TStr::Fmt("Edge %d source node %d does not exist.", Edge.GetId(), Edge.GetSrcNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
    if (! IsNode(Edge.GetDstNId())) {
      const TStr Msg = TStr::Fmt("Edge %d destination node %d does not exist.", Edge.GetId(), Edge.GetDstNId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); } RetVal=false;
    }
  }
  return RetVal;
}

void TNEANetSparse2::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  const int EdgePlaces = (int) ceil(log10((double) GetEdges()));
  fprintf(OutF, "-------------------------------------------------\nDirected Node-Edge Network: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (TNodeI NodeI = BegNI(); NodeI < EndNI(); NodeI++) {
    fprintf(OutF, "  %*d]\n", NodePlaces, NodeI.GetId());
    // load node attributes
   TIntV IntAttrN;
    IntAttrValueNI(NodeI.GetId(), IntAttrN);
    fprintf(OutF, "    nai[%d]", IntAttrN.Len());
    for (int i = 0; i < IntAttrN.Len(); i++) {
      fprintf(OutF, " %*i", NodePlaces, IntAttrN[i]()); }
    TStrV StrAttrN;
    StrAttrValueNI(NodeI.GetId(), StrAttrN);
    fprintf(OutF, "    nas[%d]", StrAttrN.Len());
    for (int i = 0; i < StrAttrN.Len(); i++) {
      fprintf(OutF, " %*s", NodePlaces, StrAttrN[i]()); }
    TFltV FltAttrN;
    FltAttrValueNI(NodeI.GetId(), FltAttrN);
    fprintf(OutF, "    naf[%d]", FltAttrN.Len());
    for (int i = 0; i < FltAttrN.Len(); i++) {
      fprintf(OutF, " %*f", NodePlaces, FltAttrN[i]()); }

    fprintf(OutF, "    in[%d]", NodeI.GetInDeg());
    for (int edge = 0; edge < NodeI.GetInDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetInEId(edge)); }
    fprintf(OutF, "\n");
    fprintf(OutF, "    out[%d]", NodeI.GetOutDeg());
    for (int edge = 0; edge < NodeI.GetOutDeg(); edge++) {
      fprintf(OutF, " %*d", EdgePlaces, NodeI.GetOutEId(edge)); }
    fprintf(OutF, "\n");
  }
  for (TEdgeI EdgeI = BegEI(); EdgeI < EndEI(); EdgeI++) {
    fprintf(OutF, "  %*d]  %*d  ->  %*d", EdgePlaces, EdgeI.GetId(), NodePlaces, EdgeI.GetSrcNId(), NodePlaces, EdgeI.GetDstNId());

    // load edge attributes
    TIntV IntAttrE;
    IntAttrValueEI(EdgeI.GetId(), IntAttrE);
    fprintf(OutF, "    eai[%d]", IntAttrE.Len());
    for (int i = 0; i < IntAttrE.Len(); i++) {
      fprintf(OutF, " %*i", EdgePlaces, IntAttrE[i]()); 
    }
    TStrV StrAttrE;
    StrAttrValueEI(EdgeI.GetId(), StrAttrE);
    fprintf(OutF, "    eas[%d]", StrAttrE.Len());
    for (int i = 0; i < StrAttrE.Len(); i++) {
      fprintf(OutF, " %*s", EdgePlaces, StrAttrE[i]()); 
    }
    TFltV FltAttrE;
    FltAttrValueEI(EdgeI.GetId(), FltAttrE);
    fprintf(OutF, "    eaf[%d]", FltAttrE.Len());
    for (int i = 0; i < FltAttrE.Len(); i++) {
      fprintf(OutF, " %*f", EdgePlaces, FltAttrE[i]()); 
    }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

// Attribute related function

int TNEANetSparse2::AddIntAttrDatN(const int& NId, const TInt& value, const TStr& attr) {
  TInt CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  TInt AttrId = AddAttrIdN(attr, IntType);
  TPair<TInt, TInt> Key(NId, AttrId);
  IntAttrsN.AddDat(Key, value);
  return 0;
} 

int TNEANetSparse2::AddStrAttrDatN(const int& NId, const TStr& value, const TStr& attr) {
  TInt CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  TInt AttrId = AddAttrIdN(attr, StrType);
  TPair<TInt, TInt> Key(NId, AttrId);
  StrAttrsN.AddDat(Key, value);
  return 0;
} 

int TNEANetSparse2::AddFltAttrDatN(const int& NId, const TFlt& value, const TStr& attr) {
  TInt CurrLen;

  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  TInt AttrId = AddAttrIdN(attr, FltType);
  TPair<TInt, TInt> Key(NId, AttrId);
  FltAttrsN.AddDat(Key, value);
  return 0;
} 

int TNEANetSparse2::AddIntAttrDatE(const int& EId, const TInt& value, const TStr& attr) {
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  TInt AttrId = AddAttrIdE(attr, IntType);
  TPair<TInt, TInt> Key(EId, AttrId);
  IntAttrsE.AddDat(Key, value);
  return 0;
} 

int TNEANetSparse2::AddStrAttrDatE(const int& EId, const TStr& value, const TStr& attr) {
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  TInt AttrId = AddAttrIdE(attr, StrType);
  TPair<TInt, TInt> Key(EId, AttrId);
  StrAttrsE.AddDat(Key, value);
  return 0;
} 

int TNEANetSparse2::AddFltAttrDatE(const int& EId, const TFlt& value, const TStr& attr) {
  TInt CurrLen;

  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  TInt AttrId = AddAttrIdE(attr, FltType);
  TPair<TInt, TInt> Key(EId, AttrId);
  FltAttrsE.AddDat(Key, value);
  return 0;
}

TInt TNEANetSparse2::GetIntAttrDatN(const int& NId, const TStr& attr) {
  TInt AttrId = GetAttrIdN(attr, IntType);
  TPair<TInt, TInt> Key(NId, AttrId);
  return IntAttrsN.GetDat(Key);
}

TStr TNEANetSparse2::GetStrAttrDatN(const int& NId, const TStr& attr) {
  TInt AttrId = GetAttrIdN(attr, StrType);
  TPair<TInt, TInt> Key(NId, AttrId);
  return StrAttrsN.GetDat(Key);
}

TFlt TNEANetSparse2::GetFltAttrDatN(const int& NId, const TStr& attr) {
  TInt AttrId = GetAttrIdN(attr, FltType);
  TPair<TInt, TInt> Key(NId, AttrId);
  return FltAttrsN.GetDat(Key);
}

TInt TNEANetSparse2::GetIntAttrDatE(const int& EId, const TStr& attr) {
  TInt AttrId = GetAttrIdE(attr, IntType);
  TPair<TInt, TInt> Key(EId, AttrId);
  return IntAttrsE.GetDat(Key);
}

TStr TNEANetSparse2::GetStrAttrDatE(const int& EId, const TStr& attr) {
  TInt AttrId = GetAttrIdE(attr, StrType);
  TPair<TInt, TInt> Key(EId, AttrId);
  return StrAttrsE.GetDat(Key);
}

TFlt TNEANetSparse2::GetFltAttrDatE(const int& EId, const TStr& attr) {
  TInt AttrId = GetAttrIdE(attr, FltType);
  TPair<TInt, TInt> Key(EId, AttrId);
  return FltAttrsE.GetDat(Key);
}

int TNEANetSparse2::DelAttrDatN(const int& NId, const TStr& attr) {
  if (IsAttrN(attr, IntType)) {
    TPair<TInt, TInt> Key(NId, GetAttrIdN(attr, IntType));
    IntAttrsN.DelKey(Key);
    return 0;
  }
  if (IsAttrN(attr, StrType)) {
    TPair<TInt, TInt> Key(NId, GetAttrIdN(attr, StrType));
    StrAttrsN.DelKey(Key);
    return 0;
  }
  if (IsAttrN(attr, FltType)) {
    TPair<TInt, TInt> Key(NId, GetAttrIdN(attr, FltType));
    FltAttrsN.DelKey(Key);
    return 0;
  }
  return -1;
}
             
int TNEANetSparse2::DelAttrDatE(const int& EId, const TStr& attr) {
  if (IsAttrE(attr, IntType)) {
    TPair<TInt, TInt> Key(EId, GetAttrIdE(attr, IntType));
    IntAttrsE.DelKey(Key);
    return 0;
  }
  if (IsAttrE(attr, StrType)) {
    TPair<TInt, TInt> Key(EId, GetAttrIdE(attr, StrType));
    StrAttrsE.DelKey(Key);
    return 0;
  }
  if (IsAttrE(attr, FltType)) {
    TPair<TInt, TInt> Key(EId, GetAttrIdE(attr, FltType));
    FltAttrsE.DelKey(Key);
    return 0;
  }
  return -1;
}

int TNEANetSparse2::GetFltAttrVecE(const TStr& attr, TVec<TFlt>& FltAttrs, int EId) const {
  THash<TInt, TEdge>::TIter EdgeHI = EdgeH.BegI();
  FltAttrs = TVec<TFlt>();
  TInt AttrId = GetAttrIdE(attr, FltType);
  while (!EdgeHI.IsEnd()) {
    TPair<TInt, TInt> Key(EdgeHI.GetDat().GetId(), AttrId);
    FltAttrs.Add(FltAttrsE.GetDat(Key));
    EdgeHI++;
  }
  /*
  StrIntPrH::TIter NodeHI, TStrV& Names) const {
  while (!NodeHI.IsEnd()) {
  } */
  return 0;
}

  // Get Vector for the Int Attribute attr.
int TNEANetSparse2::GetIntAttrVecE(const TStr& attr, TVec<TInt>& IntAttrs, int EId) const{ 
  THash<TInt, TEdge>::TIter EdgeHI = EdgeH.BegI();
  IntAttrs = TVec<TInt>();
  TInt AttrId = GetAttrIdE(attr, IntType);
  while (!EdgeHI.IsEnd()) {
    TPair<TInt, TInt> Key(EdgeHI.GetDat().GetId(), AttrId);
    IntAttrs.Add(IntAttrsE.GetDat(Key));
    EdgeHI++;
  }
  return 0;
}

  // Get Vector for the Str Attribute attr.
int TNEANetSparse2::GetStrAttrVecE(const TStr& attr, TVec<TStr>& StrAttrs, int EId) const{
  THash<TInt, TEdge>::TIter EdgeHI = EdgeH.BegI();
  StrAttrs = TVec<TStr>();
  TInt AttrId = GetAttrIdE(attr, StrType);
  while (!EdgeHI.IsEnd()) {
    TPair<TInt, TInt> Key(EdgeHI.GetDat().GetId(), AttrId);
    StrAttrs.Add(StrAttrsE.GetDat(Key));
    EdgeHI++;
  }
  return 0;
}

    // Get Vector for the Flt Attribute attr.
int TNEANetSparse2::GetFltAttrVecN(const TStr& attr, TVec<TFlt>& FltAttrs, int NId) const{
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  FltAttrs = TVec<TFlt>();
  TInt AttrId = GetAttrIdN(attr, FltType);
  while (!NodeHI.IsEnd()) {
    TPair<TInt, TInt> Key(NodeHI.GetDat().GetId(), AttrId);
    FltAttrs.Add(FltAttrsN.GetDat(Key));
    NodeHI++;
  }
  return 0;
}

  // Get Vector for the Int Attribute attr.
int TNEANetSparse2::GetIntAttrVecN(const TStr& attr, TVec<TInt>& IntAttrs, int NId) const {
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  IntAttrs = TVec<TInt>();
  TInt AttrId = GetAttrIdN(attr, IntType);
  while (!NodeHI.IsEnd()) {
    TPair<TInt, TInt> Key(NodeHI.GetDat().GetId(), AttrId);
    IntAttrs.Add(IntAttrsN.GetDat(Key));
    NodeHI++;
  }
  return 0;
}

  // Get Vector for the Str Attribute attr.
int TNEANetSparse2::GetStrAttrVecN(const TStr& attr, TVec<TStr>& StrAttrs, int NId) const {
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  StrAttrs = TVec<TStr>();
  TInt AttrId = GetAttrIdN(attr, StrType);
  while (!NodeHI.IsEnd()) {
    TPair<TInt, TInt> Key(NodeHI.GetDat().GetId(), AttrId);
    StrAttrs.Add(StrAttrsN.GetDat(Key));
    NodeHI++;
  }
  return 0;
}

// Return a small graph on 5 nodes and 6 edges.
PNEANetSparse2 TNEANetSparse2::GetSmallGraph() {
  PNEANetSparse2 Net = TNEANetSparse2::New();
  for (int i = 0; i < 5; i++) { Net->AddNode(i); }
  Net->AddEdge(0,1);  Net->AddEdge(0,2);
  Net->AddEdge(0,3);  Net->AddEdge(0,4);
  Net->AddEdge(1,2);  Net->AddEdge(1,2);
  return Net;
}

/* /// Returns a vector of int attr names for node NId.
  void TNEANetSparse2::IntAttrNI(const TInt& NId) const {
    TIntV AttrIds = TVec<TInt>();
    TIntV Attrs = TVec<TInt>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        AttrIds.Add(It.GetKey().GetVal2());
        Attrs.Add(It.GetDat());
      }
      It++;
    }
    return TAVIntI(AttrIds, Attrs, false, Graph); 
  }
  /// Returns a vector of str attr names for node NId.
  void TNEANetSparse2::StrAttrNI(const TInt& NId) const {
    TIntV AttrIds = TVec<TInt>();
    TStrV Attrs = TVec<TStr>();
    THash<TPair<TInt, TInt>, TStr>::TIter It = StrAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        AttrIds.Add(It.GetKey().GetVal2());
        Attrs.Add(It.GetDat());
      }
      It++;
    }
    return TAVStrI(AttrIds, Attrs, false, Graph); 
  }
  /// Returns a vector of int attr names for node NId.
  void TNEANetSparse2::FltAttrNI(const TInt& NId) const {
    TIntV AttrIds = TVec<TInt>();
    TFltV Attrs = TVec<TFlt>();
    THash<TPair<TInt, TInt>, TFlt>::TIter It = FltAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        AttrIds.Add(It.GetKey().GetVal2());
        Attrs.Add(It.GetDat());
      }
      It++;
    }
    return TAVFltI(AttrIds, Attrs, false, Graph); 
  }

  /// Returns a vector of int attr names for edge EId.
  void TNEANetSparse2::IntAttrEI(const TInt& EId) const {
    TIntV AttrIds = TVec<TInt>();
    TIntV Attrs = TVec<TInt>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        AttrIds.Add(It.GetKey().GetVal2());
        Attrs.Add(It.GetDat());
      }
      It++;
    }
    return TAVIntI(AttrIds, Attrs, true, Graph);
  }
  /// Returns a vector of str attr names for node NId.
  void TNEANetSparse2::StrAttrEI(const TInt& EId) const {
    TIntV AttrIds = TVec<TInt>();
    TStrV Attrs = TVec<TStr>();
    THash<TPair<TInt, TInt>, TStr>::TIter It = StrAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        AttrIds.Add(It.GetKey().GetVal2());
        Attrs.Add(It.GetDat());
      }
      It++;
    }
    return TAVStrI(AttrIds, Attrs, true, Graph);
  }
  /// Returns a vector of int attr names for node NId.
  void TNEANetSparse2::FltAttrEI(const TInt& EId) const {
    TIntV AttrIds = TVec<TInt>();
    TFltV Attrs = TVec<TFlt>();
    THash<TPair<TInt, TInt>, TFlt>::TIter It = FltAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        AttrIds.Add(It.GetKey().GetVal2());
        Attrs.Add(It.GetDat());
      }
      It++;
    }
    return TAVFltI(AttrIds, Attrs, true, Graph);
  }*/

    /// Returns a vector of attr names for node NId.
  void TNEANetSparse2::AttrNameNI(const TInt& NId, TStrV& Names) const {
    Names = TVec<TStr>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        Names.Add(GetAttrNameN(It.GetKey().GetVal2()));
      }
      It++;
    }
    THash<TPair<TInt, TInt>, TStr>::TIter It2 = StrAttrsN.BegI();
    while (!It2.IsEnd()) {
      if(It2.GetKey().GetVal1() == NId) {
        Names.Add(GetAttrNameN(It2.GetKey().GetVal2()));
      }
      It2++;
    }
    THash<TPair<TInt, TInt>, TFlt>::TIter It3 = FltAttrsN.BegI();
    while (!It3.IsEnd()) {
      if(It3.GetKey().GetVal1() == NId) {
        Names.Add(GetAttrNameN(It3.GetKey().GetVal2()));
      }
      It3++;
    }

  }
  /// Returns a vector of attr values for node NId.
  void TNEANetSparse2::AttrValueNI(const TInt& NId, TStrV& Values) const {
    Values = TVec<TStr>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        Values.Add(It.GetDat().GetStr());
      }
      It++;
    }
    THash<TPair<TInt, TInt>, TStr>::TIter It2 = StrAttrsN.BegI();
    while (!It2.IsEnd()) {
      if(It2.GetKey().GetVal1() == NId) {
        Values.Add(It2.GetDat());
      }
      It2++;
    }
    THash<TPair<TInt, TInt>, TFlt>::TIter It3 = FltAttrsN.BegI();
    while (!It3.IsEnd()) {
      if(It3.GetKey().GetVal1() == NId) {
        Values.Add(It3.GetDat().GetStr());
      }
      It3++;
    }

  }
  /// Returns a vector of int attr names for node NId.
  void TNEANetSparse2::IntAttrNameNI(const TInt& NId, TStrV& Names) const {
    Names = TVec<TStr>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        Names.Add(GetAttrNameN(It.GetKey().GetVal2()));
      }
      It++;
    }

  }
  /// Returns a vector of attr values for node NId.
  void TNEANetSparse2::IntAttrValueNI(const TInt& NId, TIntV& Values) const {
    Values = TVec<TInt>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        Values.Add(It.GetDat());
      }
      It++;
    }

  }
  /*
  TIntV AttrIds = TVec<TInt>();
    TStrV Attrs = TVec<TStr>();
    THash<TPair<TInt, TInt>, TStr>::TIter It = StrAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        AttrIds.Add(It.GetKey().GetVal2());
        Attrs.Add(It.GetDat());
      }
      It++;
    }
    return TAVStrI(AttrIds, Attrs, false, Graph); 
  }
  /// Returns a vector of int attr names for node NId.
  void TNEANetSparse2::FltAttrNI(const TInt& NId) const {
    TIntV AttrIds = TVec<TInt>();
    TFltV Attrs = TVec<TFlt>();
    THash<TPair<TInt, TInt>, TFlt>::TIter It = FltAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        AttrIds.Add(It.GetKey().GetVal2());
        Attrs.Add(It.GetDat());
      }
      It++;
    }
    return TAVFltI(AttrIds, Attrs, false, Graph); 
  */
  /// Returns a vector of str attr names for node NId.
  void TNEANetSparse2::StrAttrNameNI(const TInt& NId, TStrV& Names) const {
    Names = TVec<TStr>();
    THash<TPair<TInt, TInt>, TStr>::TIter It = StrAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        Names.Add(GetAttrNameN(It.GetKey().GetVal2()));
      }
      It++;
    }

  }
  /// Returns a vector of attr values for node NId.
  void TNEANetSparse2::StrAttrValueNI(const TInt& NId, TStrV& Values) const {
    Values = TVec<TStr>();
    THash<TPair<TInt, TInt>, TStr>::TIter It = StrAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        Values.Add(It.GetDat());
      }
      It++;
    }

  } 
  /// Returns a vector of int attr names for node NId.
  void TNEANetSparse2::FltAttrNameNI(const TInt& NId, TStrV& Names) const {
    Names = TVec<TStr>();
    THash<TPair<TInt, TInt>, TFlt>::TIter It = FltAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        Names.Add(GetAttrNameN(It.GetKey().GetVal2()));
      }
      It++;
    }

  }
  /// Returns a vector of attr values for node NId.
  void TNEANetSparse2::FltAttrValueNI(const TInt& NId, TFltV& Values) const {
    Values = TVec<TFlt>();
    THash<TPair<TInt, TInt>, TFlt>::TIter It = FltAttrsN.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == NId) {
        Values.Add(It.GetDat());
      }
      It++;
    }
  } 

  /// Returns a vector of attr names for edge EId.
  void TNEANetSparse2::AttrNameEI(const TInt& EId, TStrV& Names) const {
    Names = TVec<TStr>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == EId) {
        Names.Add(GetAttrNameE(It.GetKey().GetVal2()));
      }
      It++;
    }
    THash<TPair<TInt, TInt>, TStr>::TIter It2 = StrAttrsE.BegI();
    while (!It2.IsEnd()) {
      if(It2.GetKey().GetVal1() == EId) {
        Names.Add(GetAttrNameE(It2.GetKey().GetVal2()));
      }
      It2++;
    }
    THash<TPair<TInt, TInt>, TFlt>::TIter It3 = FltAttrsE.BegI();
    while (!It3.IsEnd()) {
      if(It3.GetKey().GetVal1() == EId) {
        Names.Add(GetAttrNameE(It3.GetKey().GetVal2()));
      }
      It3++;
    }
  }
  /// Returns a vector of attr values for edge EId.
  void TNEANetSparse2::AttrValueEI(const TInt& EId, TStrV& Values) const {
    Values = TVec<TStr>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == EId) {
        Values.Add(It.GetDat().GetStr());
      }
      It++;
    }
    THash<TPair<TInt, TInt>, TStr>::TIter It2 = StrAttrsE.BegI();
    while (!It2.IsEnd()) {
      if(It2.GetKey().GetVal1() == EId) {
        Values.Add(It2.GetDat());
      }
      It2++;
    }
    THash<TPair<TInt, TInt>, TFlt>::TIter It3 = FltAttrsE.BegI();
    while (!It3.IsEnd()) {
      if(It3.GetKey().GetVal1() == EId) {
        Values.Add(It3.GetDat().GetStr());
      }
      It3++;
    }
  }
  /// Returns a vector of int attr names for edge EId.
  void TNEANetSparse2::IntAttrNameEI(const TInt& EId, TStrV& Names) const {
    Names = TVec<TStr>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == EId) {
        Names.Add(GetAttrNameE(It.GetKey().GetVal2()));
      }
      It++;
    }
  }
  /// Returns a vector of attr values for edge EId.
  void TNEANetSparse2::IntAttrValueEI(const TInt& EId, TIntV& Values) const {
    Values = TVec<TInt>();
    THash<TPair<TInt, TInt>, TInt>::TIter It = IntAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == EId) {
        Values.Add(It.GetDat());
      }
      It++;
    }
  } 
  /// Returns a vector of str attr names for node NId.
  void TNEANetSparse2::StrAttrNameEI(const TInt& EId, TStrV& Names) const {
    Names = TVec<TStr>();
    THash<TPair<TInt, TInt>, TStr>::TIter It = StrAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == EId) {
        Names.Add(GetAttrNameE(It.GetKey().GetVal2()));
      }
      It++;
    }
  }
  /// Returns a vector of attr values for node NId.
  void TNEANetSparse2::StrAttrValueEI(const TInt& EId, TStrV& Values) const {
    Values = TVec<TStr>();
    THash<TPair<TInt, TInt>, TStr>::TIter It = StrAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == EId) {
        Values.Add(It.GetDat());
      }
      It++;
    }
  } 
  /// Returns a vector of int attr names for node NId.
  void TNEANetSparse2::FltAttrNameEI(const TInt& EId, TStrV& Names) const {
    Names = TVec<TStr>();
    THash<TPair<TInt, TInt>, TFlt>::TIter It = FltAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == EId) {
        Names.Add(GetAttrNameE(It.GetKey().GetVal2()));
      }
      It++;
    }
  }
  /// Returns a vector of attr values for node NId.
  void TNEANetSparse2::FltAttrValueEI(const TInt& EId, TFltV& Values) const {
    Values = TVec<TFlt>();
    THash<TPair<TInt, TInt>, TFlt>::TIter It = FltAttrsE.BegI();
    while (!It.IsEnd()) {
      if(It.GetKey().GetVal1() == EId) {
        Values.Add(It.GetDat());
      }
      It++;
    }

  }
    // Returns node attribute value, converted to Str type.
  TStr TNEANetSparse2::GetNodeAttrValue(const int& NId, const TStr attr) const {
    if (IsAttrN(attr, IntType)) {
      TInt AttrId = GetAttrIdN(attr, IntType);
      TPair<TInt, TInt> Key(NId, AttrId);
      return IntAttrsN.GetDat(Key).GetStr();
    }

    if (IsAttrN(attr, StrType)) {
      TInt AttrId = GetAttrIdN(attr, StrType);
      TPair<TInt, TInt> Key(NId, AttrId);
      return StrAttrsN.GetDat(Key);
    }

    if (IsAttrN(attr, FltType)) {
      TInt AttrId = GetAttrIdN(attr, FltType);
      TPair<TInt, TInt> Key(NId, AttrId);
      return FltAttrsN.GetDat(Key).GetStr();
    }
    return TStr::GetNullStr();
  }
  // Returns edge attribute value, converted to Str type.
  TStr TNEANetSparse2::GetEdgeAttrValue(const int& EId, const TStr attr) const {
    if (IsAttrE(attr, IntType)) {
      TInt AttrId = GetAttrIdE(attr, IntType);
      TPair<TInt, TInt> Key(EId, AttrId);
      return IntAttrsE.GetDat(Key).GetStr();
    }

    if (IsAttrE(attr, StrType)) {
      TInt AttrId = GetAttrIdE(attr, StrType);
      TPair<TInt, TInt> Key(EId, AttrId);
      return StrAttrsE.GetDat(Key);
    }

    if (IsAttrE(attr, FltType)) {
      TInt AttrId = GetAttrIdE(attr, FltType);
      TPair<TInt, TInt> Key(EId, AttrId);
      return FltAttrsE.GetDat(Key).GetStr();
    }
    return TStr::GetNullStr();
  }

  int TNEANetSparse2::GetFltAttrVecE(const TStr& attr, TVec<TPair<TInt, TFlt> >& FltAttrs, int EId) const {
  TIntV Perm;
  THash<TInt, TEdge>::TIter EdgeHI = EdgeH.BegI();
  while (!EdgeHI.IsEnd()) {
    Perm.Add(EdgeHI.GetKey());
    EdgeHI++;
  }
  TRnd R(time(NULL));
  Perm.Shuffle(R);
  FltAttrs = TVec<TPair<TInt, TFlt> >();
  TInt AttrId = GetAttrIdN(attr, FltType);
  for (int i = 0; i < Perm.Len(); i++) {
    TInt Id = Perm[i];
    TPair<TInt, TInt> Key(Id, AttrId);
    if (FltAttrsN.IsKey(Key)) {
      TPair<TInt, TFlt> Val(Id, FltAttrsN.GetDat(Key));
      FltAttrs.Add(Val);
    }
  }
  return 0;
}

  // Get Vector for the Int Attribute attr.
int TNEANetSparse2::GetIntAttrVecE(const TStr& attr, TVec<TPair<TInt, TInt> >& IntAttrs, int EId) const{ 
  TIntV Perm;
  THash<TInt, TEdge>::TIter EdgeHI = EdgeH.BegI();
  while (!EdgeHI.IsEnd()) {
    Perm.Add(EdgeHI.GetKey());
    EdgeHI++;
  }
  TRnd R(time(NULL));
  Perm.Shuffle(R);
  IntAttrs = TVec<TPair<TInt, TInt> >();
  TInt AttrId = GetAttrIdN(attr, IntType);
  for (int i = 0; i < Perm.Len(); i++) {
    TInt Id = Perm[i];
    TPair<TInt, TInt> Key(Id, AttrId);
    if (IntAttrsN.IsKey(Key)) {
      TPair<TInt, TInt> Val(Id, IntAttrsN.GetDat(Key));
      IntAttrs.Add(Val);
    }
  }
  return 0;
}

  // Get Vector for the Str Attribute attr.
int TNEANetSparse2::GetStrAttrVecE(const TStr& attr, TVec<TPair<TInt, TStr> >& StrAttrs, int EId) const{
  TIntV Perm;
  THash<TInt, TEdge>::TIter EdgeHI = EdgeH.BegI();
  while (!EdgeHI.IsEnd()) {
    Perm.Add(EdgeHI.GetKey());
    EdgeHI++;
  }
  TRnd R(time(NULL));
  Perm.Shuffle(R);
  StrAttrs = TVec<TPair<TInt, TStr> >();
  TInt AttrId = GetAttrIdN(attr, StrType);
  for (int i = 0; i < Perm.Len(); i++) {
    TInt Id = Perm[i];
    TPair<TInt, TInt> Key(Id, AttrId);
    if (StrAttrsN.IsKey(Key)) {
      TPair<TInt, TStr> Val(Id, StrAttrsN.GetDat(Key));
      StrAttrs.Add(Val);
    }
  }
  return 0;
}

    // Get Vector for the Flt Attribute attr.
int TNEANetSparse2::GetFltAttrVecN(const TStr& attr, TVec<TPair<TInt, TFlt> >& FltAttrs, int NId) const{
  TIntV Perm;
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  while (!NodeHI.IsEnd()) {
    Perm.Add(NodeHI.GetKey());
    NodeHI++;
  }
  TRnd R(time(NULL));
  Perm.Shuffle(R);
  FltAttrs = TVec<TPair<TInt, TFlt> >();
  TInt AttrId = GetAttrIdN(attr, FltType);
  for (int i = 0; i < Perm.Len(); i++) {
    TInt Id = Perm[i];
    TPair<TInt, TInt> Key(Id, AttrId);
    if (FltAttrsN.IsKey(Key)) {
      TPair<TInt, TFlt> Val(Id, FltAttrsN.GetDat(Key));
      FltAttrs.Add(Val);
    }
  }
  return 0;
}

  // Get Vector for the Int Attribute attr.
int TNEANetSparse2::GetIntAttrVecN(const TStr& attr, TVec<TPair<TInt, TInt> >& IntAttrs, int NId) const {
  TIntV Perm;
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  while (!NodeHI.IsEnd()) {
    Perm.Add(NodeHI.GetKey());
    NodeHI++;
  }
  TRnd R(time(NULL));
  Perm.Shuffle(R);
  IntAttrs = TVec<TPair<TInt, TInt> >();
  TInt AttrId = GetAttrIdN(attr, IntType);
  for (int i = 0; i < Perm.Len(); i++) {
    TInt Id = Perm[i];
    TPair<TInt, TInt> Key(Id, AttrId);
    if (IntAttrsN.IsKey(Key)) {
      TPair<TInt, TInt> Val(Id, IntAttrsN.GetDat(Key));
      IntAttrs.Add(Val);
    }
  }
  return 0;
}

  // Get Vector for the Str Attribute attr.
int TNEANetSparse2::GetStrAttrVecN(const TStr& attr, TVec<TPair<TInt, TStr> >& StrAttrs, int NId) const {
  TIntV Perm;
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  while (!NodeHI.IsEnd()) {
    Perm.Add(NodeHI.GetKey());
    NodeHI++;
  }
  TRnd R(time(NULL));
  Perm.Shuffle(R);
  StrAttrs = TVec<TPair<TInt, TStr> >();
  TInt AttrId = GetAttrIdN(attr, StrType);
  for (int i = 0; i < Perm.Len(); i++) {
    TInt Id = Perm[i];
    TPair<TInt, TInt> Key(Id, AttrId);
    if (StrAttrsN.IsKey(Key)) {
      TPair<TInt, TStr> Val(Id, StrAttrsN.GetDat(Key));
      StrAttrs.Add(Val);
    }
  }
  return 0;
}


void TNEANetSparse2::GroupByIntNodeVal(THash<TInt, TVec<TInt> >& Grouping, TStr &attr, bool IncludeNoVal, TInt NoVal) const {
  Grouping = THash<TInt, TVec<TInt> >();
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  TInt AttrId = GetAttrIdN(attr, IntType);
  if (IncludeNoVal) {
    Grouping.AddDat(NoVal, TIntV());
  }
  while (!NodeHI.IsEnd()) {
    TInt Id = NodeHI.GetKey();
    TPair<TInt, TInt> Key(Id, AttrId);
    if (IntAttrsN.IsKey(Key)) {
      TInt Val = IntAttrsN.GetDat(Key);
      if (!Grouping.IsKey(Val)) {
        Grouping.AddDat(Val, TIntV());
      }
      Grouping.GetDat(Val).Add(Id);
    } else if (IncludeNoVal) {
      Grouping.GetDat(NoVal).Add(Id);
    }
    NodeHI++;
  }
}
 
void TNEANetSparse2::GroupByFltNodeVal(THash<TFlt, TVec<TInt> >& Grouping, TStr &attr, bool IncludeNoVal, TFlt NoVal) const {
  Grouping = THash<TFlt, TVec<TInt> >();
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  TInt AttrId = GetAttrIdN(attr, FltType);
  if (IncludeNoVal) {
    Grouping.AddDat(NoVal, TIntV());
  }
  while (!NodeHI.IsEnd()) {
    TInt Id = NodeHI.GetKey();
    TPair<TInt, TInt> Key(Id, AttrId);
    if (FltAttrsN.IsKey(Key)) {
      TFlt Val = FltAttrsN.GetDat(Key);
      if (!Grouping.IsKey(Val)) {
        Grouping.AddDat(Val, TIntV());
      }
      Grouping.GetDat(Val).Add(Id);
    } else if (IncludeNoVal) {
      Grouping.GetDat(NoVal).Add(Id);
    }
    NodeHI++;
  }
}

void TNEANetSparse2::GroupByStrNodeVal(THash<TStr, TVec<TInt> >& Grouping, TStr &attr, bool IncludeNoVal, TStr NoVal) const {
  Grouping = THash<TStr, TVec<TInt> >();
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  TInt AttrId = GetAttrIdN(attr, StrType);
  if (IncludeNoVal) {
    Grouping.AddDat(NoVal, TIntV());
  }
  while (!NodeHI.IsEnd()) {
    TInt Id = NodeHI.GetKey();
    TPair<TInt, TInt> Key(Id, AttrId);
    if (StrAttrsN.IsKey(Key)) {
      TStr Val = StrAttrsN.GetDat(Key);
      if (!Grouping.IsKey(Val)) {
        Grouping.AddDat(Val, TIntV());
      }

      Grouping.GetDat(Val).Add(Id);
    } else if (IncludeNoVal) {
      Grouping.GetDat(NoVal).Add(Id);
    }
    NodeHI++;
  }
}



void TNEANetSparse2::GetIntKeyFreq(THash<TStr, TInt>& Freq) const{
  Freq = THash<TStr, TInt> ();
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  while(!NodeHI.IsEnd()) {
    TStrV Names;
    IntAttrNameNI(NodeHI.GetKey(), Names);
    for (int i = 0; i < Names.Len(); i++) {
      if (!Freq.IsKey(Names[i])) {
        Freq.AddDat(Names[i], TInt(1));
      } else { 
        TStr Name = Names[i];
        int CurrFreq = Freq(Name);
        Freq.AddDat(Names[i], CurrFreq+1);
      }
    }
    NodeHI++;
  }
  /*Freq = THash<TStr, TInt> ();
  THash<TInt, TInt> AttrFreqById;
  // TPair<TInt, TInt> Key(NId, AttrId);
  //return IntAttrsN.GetDat(Key);

  THash<TPair<TInt, TInt>, TInt >::TIter IntI = IntAttrsN.BegI();
  while(!IntI.IsEnd()) {
    TInt Key = IntI.GetKey().GetVal2();
    if (!AttrFreqById.IsKey(Key)) {
      AttrFreqById.AddDat(Key, 0);
    }
    AttrFreqById[Key] += 1;
    IntI++;
  }

  THash<TInt, TInt>::TIter FreqI = AttrFreqById.BegI();
  while (!FreqI.IsEnd()) {
    TStr Name = GetAttrNameN(FreqI.GetKey());
    Freq.AddDat(Name, FreqI.GetDat());
    FreqI++;
  }*/
}
void TNEANetSparse2::GetFltKeyFreq(THash<TStr, TInt>& Freq) const {
  Freq = THash<TStr, TInt> ();
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  while(!NodeHI.IsEnd()) {
    TStrV Names;
    FltAttrNameNI(NodeHI.GetKey(), Names);
    for (int i = 0; i < Names.Len(); i++) {
      if (!Freq.IsKey(Names[i])) {
        Freq.AddDat(Names[i], TInt(1));
      } else { 
        TStr Name = Names[i];
        int CurrFreq = Freq(Name);
        Freq.AddDat(Names[i], CurrFreq+1);
      }
    }
    NodeHI++;
  }
  /*Freq = THash<TStr, TInt> ();
  THash<TInt, TInt> AttrFreqById;

  THash<TPair<TInt, TInt>, TFlt >::TIter FltI = FltAttrsN.BegI();
  while(!FltI.IsEnd()) {
    TInt Key = FltI.GetKey().GetVal2();
    if (!AttrFreqById.IsKey(Key)) {
      AttrFreqById.AddDat(Key, 0);
    }
    AttrFreqById[Key] += 1;
    FltI++;
  }


  THash<TInt, TInt>::TIter FreqI = AttrFreqById.BegI();
  while (!FreqI.IsEnd()) {
    TStr Name = GetAttrNameN(FreqI.GetKey());
    Freq.AddDat(Name, FreqI.GetDat());
    FreqI++;
  }*/

}
void TNEANetSparse2::GetStrKeyFreq(THash<TStr, TInt>& Freq) const {
  Freq = THash<TStr, TInt> ();
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  while(!NodeHI.IsEnd()) {
    TStrV Names;
    StrAttrNameNI(NodeHI.GetKey(), Names);
    for (int i = 0; i < Names.Len(); i++) {
      if (!Freq.IsKey(Names[i])) {
        Freq.AddDat(Names[i], TInt(1));
      } else { 
        TStr Name = Names[i];
        int CurrFreq = Freq(Name);
        Freq.AddDat(Names[i], CurrFreq+1);
      }
    }
    NodeHI++;
  }
  /*Freq = THash<TStr, TInt> ();
  THash<TInt, TInt> AttrFreqById;

  THash<TPair<TInt, TInt>, TStr >::TIter StrI = StrAttrsN.BegI();
  while(!StrI.IsEnd()) {
    TInt Key = StrI.GetKey().GetVal2();
    if (!AttrFreqById.IsKey(Key)) {
      AttrFreqById.AddDat(Key, 0);
    }
    AttrFreqById[Key] += 1;
    StrI++;
  }

  THash<TInt, TInt>::TIter FreqI = AttrFreqById.BegI();
  while (!FreqI.IsEnd()) {
    TStr Name = GetAttrNameN(FreqI.GetKey());
    Freq.AddDat(Name, FreqI.GetDat());
    FreqI++;
  }*/
}
void TNEANetSparse2::GetKeyFreq(THash<TStr, TInt>& Freq) const {
  Freq = THash<TStr, TInt> ();
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  while(!NodeHI.IsEnd()) {
    TStrV Names;
    AttrNameNI(NodeHI.GetKey(), Names);
    for (int i = 0; i < Names.Len(); i++) {
      if (!Freq.IsKey(Names[i])) {
        Freq.AddDat(Names[i], TInt(1));
      } else { 
        TStr Name = Names[i];
        int CurrFreq = Freq(Name);
        Freq.AddDat(Names[i], CurrFreq+1);
      }
    }
    NodeHI++;
  }
  /*Freq = THash<TStr, TInt> ();
  THash<TInt, TInt> AttrFreqById;
  
  THash<TPair<TInt, TInt>, TInt >::TIter IntI = IntAttrsN.BegI();
  while(!IntI.IsEnd()) {
    TInt Key = IntI.GetKey().GetVal2();
    if (!AttrFreqById.IsKey(Key)) {
      AttrFreqById.AddDat(Key, 0);
    }
    AttrFreqById[Key] += 1;
    IntI++;
  }

  THash<TPair<TInt, TInt>, TFlt >::TIter FltI = FltAttrsN.BegI();
  while(!FltI.IsEnd()) {
    TInt Key = FltI.GetKey().GetVal2();
    if (!AttrFreqById.IsKey(Key)) {
      AttrFreqById.AddDat(Key, 0);
    }
    AttrFreqById[Key] += 1;
    FltI++;
  }

  THash<TPair<TInt, TInt>, TStr >::TIter StrI = StrAttrsN.BegI();
  while(!StrI.IsEnd()) {
    TInt Key = StrI.GetKey().GetVal2();
    if (!AttrFreqById.IsKey(Key)) {
      AttrFreqById.AddDat(Key, 0);
    }
    AttrFreqById[Key] += 1;
    StrI++;
  }

  THash<TInt, TInt>::TIter FreqI = AttrFreqById.BegI();
  while (!FreqI.IsEnd()) {
    TStr Name = GetAttrNameN(FreqI.GetKey());
    Freq.AddDat(Name, FreqI.GetDat());
    FreqI++;
  }*/

}
