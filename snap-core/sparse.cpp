/////////////////////////////////////////////////
// Attribute Node Edge Network
bool TNEANetSparse1::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TNEANetSparse1::TNet, Flag);
}

bool TNEANetSparse1::TNodeI::IsInNId(const int& NId) const {
  const TNode& Node = NodeH.GetDat(NId);
  for (int edge = 0; edge < Node.GetInDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetInEId(edge)).GetSrcNId())
      return true;
  }
  return false;
}

bool TNEANetSparse1::TNodeI::IsOutNId(const int& NId) const {
  const TNode& Node = NodeH.GetDat(NId);
  for (int edge = 0; edge < Node.GetOutDeg(); edge++) {
    if (NId == Graph->GetEdge(Node.GetOutEId(edge)).GetDstNId())
      return true;
  }
  return false;
}

bool TNEANetSparse1::NodeAttrIsDeleted(const int& NId, const TStr& attr) const {
  return NodeAttrIsIntDeleted(NId, attr) && NodeAttrIsStrDeleted(NId, attr) && NodeAttrIsFltDeleted(NId, attr);
}

bool TNEANetSparse1::NodeAttrIsIntDeleted(const int& NId, const TStr& attr) const {
  const TNode& Node = NodeH.GetDat(NId);
  int index = Node.GetIntAttrIndex(GetAttrIdN(attr, IntType));
  return index == -1;
}

bool TNEANetSparse1::NodeAttrIsStrDeleted(const int& NId, const TStr& attr) const {
  const TNode& Node = NodeH.GetDat(NId);
  int index = Node.GetStrAttrIndex(GetAttrIdN(attr, StrType));
  return index == -1;
}

bool TNEANetSparse1::NodeAttrIsFltDeleted(const int& NId, const TStr& attr) const {
  const TNode& Node = NodeH.GetDat(NId);
  int index = Node.GetFltAttrIndex(GetAttrIdN(attr, FltType));
  return index == -1;
}

bool TNEANetSparse1::EdgeAttrIsDeleted(const int& EId, const TStr& attr) const {
  return EdgeAttrIsIntDeleted(NId, attr) && EdgeAttrIsStrDeleted(NId, attr) && EdgeAttrIsFltDeleted(NId, attr);
}

bool TNEANetSparse1::EdgeAttrIsIntDeleted(const int& EId, const TStr& attr) const {
  const TEdge& Edge = EdgeH.GetDat(EId);
  int index = Edge.GetIntAttrIndex(GetAttrIdE(attr, IntType));
  return index == -1;
}

bool TNEANetSparse1::EdgeAttrIsStrDeleted(const int& EId, const TStr& attr) const {
  const TEdge& Edge = EdgeH.GetDat(EId);
  int index = Edge.GetStrAttrIndex(GetAttrIdE(attr, StrType));
  return index == -1;
}

bool TNEANetSparse1::EdgeAttrIsFltDeleted(const int& EId, const TStr& attr) const {
  const TEdge& Edge = EdgeH.GetDat(EId);
  int index = Edge.GetFltAttrIndex(GetAttrIdE(attr, FltType));
  return index == -1;
}

int TNEANetSparse1::AddNode(int NId) {
  int i;
  if (NId == -1) {
    NId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    MxNId = TMath::Mx(NId+1, MxNId());
  }
  return NId;
}

void TNEANetSparse1::DelNode(const int& NId) {
  int i;
  
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
}

int TNEANetSparse1::AddEdge(const int& SrcNId, const int& DstNId, int EId) {
  int i;

  if (EId == -1) { EId = MxEId;  MxEId++; }
  else { MxEId = TMath::Mx(EId+1, MxEId()); }
  IAssertR(!IsEdge(EId), TStr::Fmt("EdgeId %d already exists", EId));
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  EdgeH.AddDat(EId, TEdge(EId, SrcNId, DstNId));
  GetNode(SrcNId).OutEIdV.AddSorted(EId);
  GetNode(DstNId).InEIdV.AddSorted(EId);
  return EId;
}

void TNEANetSparse1::DelEdge(const int& EId) {
  int i;

  IAssert(IsEdge(EId));
  const int SrcNId = GetEdge(EId).GetSrcNId();
  const int DstNId = GetEdge(EId).GetDstNId();
  GetNode(SrcNId).OutEIdV.DelIfIn(EId);
  GetNode(DstNId).InEIdV.DelIfIn(EId);
  EdgeH.DelKey(EId);
}

// delete all edges between the two nodes
void TNEANetSparse1::DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir) {
  int EId = 0;
  bool Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  IAssert(Edge); // there is at least one edge
  while (Edge) {
    DelEdge(EId);
    Edge = IsEdge(SrcNId, DstNId, EId, IsDir);
  }
}

bool TNEANetSparse1::IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir) const {
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

void TNEANetSparse1::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N));
  }
}

void TNEANetSparse1::GetEIdV(TIntV& EIdV) const {
  EIdV.Gen(GetEdges(), 0);
  for (int E=EdgeH.FFirstKeyId(); EdgeH.FNextKeyId(E); ) {
    EIdV.Add(EdgeH.GetKey(E));
  }
}

void TNEANetSparse1::Defrag(const bool& OnlyNodeLinks) {
  for (int kid = NodeH.FFirstKeyId(); NodeH.FNextKeyId(kid); ) {
    TNode& Node = NodeH[kid];
    Node.InEIdV.Pack();  Node.OutEIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) { NodeH.Defrag(); }
  if (! OnlyNodeLinks && ! EdgeH.IsKeyIdEqKeyN()) { EdgeH.Defrag(); }
}

bool TNEANetSparse1::IsOk(const bool& ThrowExcept) const {
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

void TNEANetSparse1::Dump(FILE *OutF) const {
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

int TNEANetSparse1::AddIntAttrDatN(const int& NId, const TInt& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  TNode& Node = NodeH.GetDat(NId);
  TInt AttrId = GetAttrIdN(attr, IntType);
  Node.AddIntAttr(AttrId, value);
  return 0;
} 

int TNEANetSparse1::AddStrAttrDatN(const int& NId, const TStr& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  TNode& Node = NodeH.GetDat(NId);
  TInt AttrId = GetAttrIdN(attr, StrType);
  Node.AddStrAttr(AttrId, value);
  return 0;
} 

int TNEANetSparse1::AddFltAttrDatN(const int& NId, const TFlt& value, const TStr& attr) {
  int i;
  TInt CurrLen;

  if (!IsNode(NId)) {
    // AddNode(NId);
    return -1;
  }
  TNode& Node = NodeH.GetDat(NId);
  TInt AttrId = GetAttrIdN(attr, FltType);
  Node.AddFltAttr(AttrId, value);
  return 0;
} 

int TNEANetSparse1::AddIntAttrDatE(const int& EId, const TInt& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  TEdge& Edge = EdgeH.GetDat(EId);
  TInt AttrId = Edge.GetAttrIdE(attr, IntType);
  Edge.AddIntAttr(AttrId, value);
  return 0;
} 

int TNEANetSparse1::AddStrAttrDatE(const int& EId, const TStr& value, const TStr& attr) {
  int i;
  TInt CurrLen;
  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  TEdge& Edge = EdgeH.GetDat(EId);
  TInt AttrId = Edge.GetAttrIdE(attr, StrType);
  Edge.AddStrAttr(AttrId, value);
  return 0;
} 

int TNEANetSparse1::AddFltAttrDatE(const int& EId, const TFlt& value, const TStr& attr) {
  int i;
  TInt CurrLen;

  if (!IsEdge(EId)) {
    //AddEdge(EId);
     return -1;
  }
  TEdge& Edge = EdgeH.GetDat(EId);
  TInt AttrId = Edge.GetAttrIdE(attr, FltType);
  Edge.AddFltAttr(AttrId, value);
  return 0;
}

TInt TNEANetSparse1::GetIntAttrDatN(const int& NId, const TStr& attr) {
  TNode& Node = NodeH.GetDat(NId);
  TInt AttrId = GetAttrIdN(attr, IntType);
  return Node.GetIntAttrVal(AttrId);
}

TStr TNEANetSparse1::GetStrAttrDatN(const int& NId, const TStr& attr) {
  TNode& Node = NodeH.GetDat(NId);
  TInt AttrId = GetAttrIdN(attr, StrType);
  return Node.GetStrAttrVal(AttrId);
}

TFlt TNEANetSparse1::GetFltAttrDatN(const int& NId, const TStr& attr) {
  TNode& Node = NodeH.GetDat(NId);
  TInt AttrId = GetAttrIdN(attr, FltType);
  return Node.GetFltAttrVal(AttrId);
}

TInt TNEANetSparse1::GetIntAttrDatE(const int& EId, const TStr& attr) {
  TEdge& Edge = EdgeH.GetDat(EId);
  TInt AttrId = GetAttrIdE(attr, IntType);
  return Edge.GetIntAttrVal(AttrId);
}

TStr TNEANetSparse1::GetStrAttrDatE(const int& EId, const TStr& attr) {
  TEdge& Edge = EdgeH.GetDat(EId);
  TInt AttrId = GetAttrIdE(attr, StrType);
  return Edge.GetStrAttrVal(AttrId);
}

TFlt TNEANetSparse1::GetFltAttrDatE(const int& EId, const TStr& attr) {
  TEdge& Edge = EdgeH.GetDat(EId);
  TInt AttrId = GetAttrIdE(attr, FltType);
  return Edge.GetFltAttrVal(AttrId);
}

int TNEANetSparse1::DelAttrDatN(const int& NId, const TStr& attr) {
  TNode& Node = NodeH.GetDat(NId);
  if (isAttrN(attr, IntType)) {
    Node.DelIntAttr(GetAttrIdN(attr, IntType));
    return;
  }
  if (isAttrN(attr, StrType)) {
    Node.DelStrAttr(GetAttrIdN(attr, StrType));
    return;
  }
  if (isAttrN(attr, FltType)) {
    Node.DelFltAttr(GetAttrIdN(attr, FltType));
    return;
  }
}
             
int TNEANetSparse1::DelAttrDatE(const int& EId, const TStr& attr) {
  TEdge& Edge = EdgeH.GetDat(EId);
  if (isAttrE(attr, IntType)) {
    Edge.DelIntAttr(GetAttrIdE(attr, IntType));
    return;
  }
  if (isAttrE(attr, StrType)) {
    Edge.DelStrAttr(GetAttrIdE(attr, StrType));
    return;
  }
  if (isAttrE(attr, FltType)) {
    Edge.DelFltAttr(GetAttrIdE(attr, FltType));
    return;
  }
}

int TNEANetSparse1::GetFltAttrVecE(const TStr& attr, TVec<TFlt>& FltAttrs, int EId=-1) {
  THash<TInt, TEdge>::TIter EdgeHI = EdgeH.BegI();
  FltAttrs = TVec<TFlt>();
  TInt AttrId = GetAttrIdE(attr, FltType);
  while (!EdgeHI.IsEnd()) {
    TEdge& Edge = EdgeH.GetDat(EdgeHI.GetId());
    FltAttrs.Add(Edge.GetFltAttr(AttrId));
  }
  /*
  StrIntPrH::TIter NodeHI, TStrV& Names) const {
  Names = TVec<TStr>();
  while (!NodeHI.IsEnd()) {
    if (!NodeAttrIsDeleted(NId, NodeHI)) {
      Names.Add(NodeHI.GetKey());
    }
    NodeHI++;
  } 
  */
}

  // Get Vector for the Int Attribute attr.
int TNEANetSparse1::GetIntAttrVecE(const TStr& attr, TVec<TInt>& IntAttrs, int EId=-1) { 
  THash<TInt, TEdge>::TIter EdgeHI = EdgeH.BegI();
  IntAttrs = TVec<TInt>();
  TInt AttrId = GetAttrIdE(attr, IntType);
  while (!EdgeHI.IsEnd()) {
    TEdge& Edge = EdgeH.GetDat(EdgeHI.GetId());
    IntAttrs.Add(Edge.GetIntAttr(AttrId));
  }
}

  // Get Vector for the Str Attribute attr.
int TNEANetSparse1::GetStrAttrVecE(const TStr& attr, TVec<TStr>& StrAttrs, int EId=-1) {
  THash<TInt, TEdge>::TIter EdgeHI = EdgeH.BegI();
  StrAttrs = TVec<TStr>();
  TInt AttrId = GetAttrIdE(attr, StrType);
  while (!EdgeHI.IsEnd()) {
    TEdge& Edge = EdgeH.GetDat(EdgeHI.GetId());
    StrAttrs.Add(Edge.GetStrAttr(AttrId));
  }
}

    // Get Vector for the Flt Attribute attr.
int TNEANetSparse1::GetFltAttrVecN(const TStr& attr, TVec<TFlt>& FltAttrs, int NId=-1) {
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  FltAttrs = TVec<TFlt>();
  TInt AttrId = GetAttrIdN(attr, FltType);
  while (!NodeHI.IsEnd()) {
    TNode& Node = NodeH.GetDat(NodeHI.GetId());
    FltAttrs.Add(Node.GetFltAttr(AttrId));
  }
}

  // Get Vector for the Int Attribute attr.
int TNEANetSparse1::GetIntAttrVecN(const TStr& attr, TVec<TInt>& IntAttrs, int NId=-1) {
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  IntAttrs = TVec<TInt>();
  TInt AttrId = GetAttrIdN(attr, IntType);
  while (!NodeHI.IsEnd()) {
    TNode& Node = NodeH.GetDat(NodeHI.GetId());
    IntAttrs.Add(Node.GetIntAttr(AttrId));
  }
}

  // Get Vector for the Str Attribute attr.
int TNEANetSparse1::GetStrAttrVecN(const TStr& attr, TVec<TStr>& StrAttrs, int NId=-1) {
  THash<TInt, TNode>::TIter NodeHI = NodeH.BegI();
  StrAttrs = TVec<TStr>();
  TInt AttrId = GetAttrIdN(attr, StrType);
  while (!NodeHI.IsEnd()) {
    TNode& Node = NodeH.GetDat(NodeHI.GetId());
    StrAttrs.Add(Node.GetStrAttr(AttrId));
  }
}

// Return a small graph on 5 nodes and 6 edges.
PNEANetSparse2 TNEANetSparse1::GetSmallGraph() {
  PNEANetSparse2 Net = TNEANetSparse1::New();
  for (int i = 0; i < 5; i++) { Net->AddNode(i); }
  Net->AddEdge(0,1);  Net->AddEdge(0,2);
  Net->AddEdge(0,3);  Net->AddEdge(0,4);
  Net->AddEdge(1,2);  Net->AddEdge(1,2);
  return Net;
}
