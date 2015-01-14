class TNEANetSparse2;
/// Pointer to a directed attribute multigraph (TNEANetSparse2)
typedef TPt<TNEANetSparse2> PNEANetSparse2;

//#//////////////////////////////////////////////
/// Directed multigraph with node edge attributes. ##TNEANetSparse2::Class
class TNEANetSparse2 {
public:
  typedef TNEANetSparse2 TNet;
  typedef TPt<TNEANetSparse2> PNet;
public:
  class TNode {
  private:
    TInt Id;
    TIntV InEIdV, OutEIdV;
  public:
    TNode() : Id(-1), InEIdV(), OutEIdV(){ }
    TNode(const int& NId) : Id(NId), InEIdV(), OutEIdV() { }
    TNode(const TNode& Node) : Id(Node.Id), InEIdV(Node.InEIdV), OutEIdV(Node.OutEIdV) { }
    TNode(TSIn& SIn) : Id(SIn), InEIdV(SIn), OutEIdV(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); InEIdV.Save(SOut); OutEIdV.Save(SOut); }
    int GetId() const { return Id; }
    int GetDeg() const { return GetInDeg() + GetOutDeg(); }
    int GetInDeg() const { return InEIdV.Len(); }
    int GetOutDeg() const { return OutEIdV.Len(); }
    int GetInEId(const int& EdgeN) const { return InEIdV[EdgeN]; }
    int GetOutEId(const int& EdgeN) const { return OutEIdV[EdgeN]; }
    int GetNbrEId(const int& EdgeN) const { return EdgeN<GetOutDeg()?GetOutEId(EdgeN):GetInEId(EdgeN-GetOutDeg()); }
    bool IsInEId(const int& EId) const { return InEIdV.SearchBin(EId) != -1; }
    bool IsOutEId(const int& EId) const { return OutEIdV.SearchBin(EId) != -1; }

    friend class TNEANetSparse2;
  };
  class TEdge {
  private:
    TInt Id, SrcNId, DstNId;

  public:
    TEdge() : Id(-1), SrcNId(-1), DstNId(-1) { }
    TEdge(const int& EId, const int& SourceNId, const int& DestNId) : Id(EId), SrcNId(SourceNId), DstNId(DestNId) { }
    TEdge(const TEdge& Edge) : Id(Edge.Id), SrcNId(Edge.SrcNId), DstNId(Edge.DstNId) { }
    TEdge(TSIn& SIn) : Id(SIn), SrcNId(SIn), DstNId(SIn) { }
    void Save(TSOut& SOut) const { Id.Save(SOut); SrcNId.Save(SOut); DstNId.Save(SOut); }
    int GetId() const { return Id; }
    int GetSrcNId() const { return SrcNId; }
    int GetDstNId() const { return DstNId; }

    friend class TNEANetSparse2;
  };
  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    typedef THash<TInt, TNode>::TIter THashIter;
    THashIter NodeHI;
    const TNEANetSparse2 *Graph;
  public:
    TNodeI() : NodeHI(), Graph(NULL) { }
    TNodeI(const THashIter& NodeHIter, const TNEANetSparse2* GraphPt) : NodeHI(NodeHIter), Graph(GraphPt) { }
    TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI), Graph(NodeI.Graph) { }
    TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; Graph=NodeI.Graph; return *this; }
    /// Increment iterator.
    TNodeI& operator++ (int) { NodeHI++; return *this; }
    bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }
    bool operator == (const TNodeI& NodeI) const { return NodeHI == NodeI.NodeHI; }
    /// Returns ID of the current node.
    int GetId() const { return NodeHI.GetDat().GetId(); }
    /// Returns degree of the current node, the sum of in-degree and out-degree.
    int GetDeg() const { return NodeHI.GetDat().GetDeg(); }
    /// Returns in-degree of the current node.
    int GetInDeg() const { return NodeHI.GetDat().GetInDeg(); }
    /// Returns out-degree of the current node.
    int GetOutDeg() const { return NodeHI.GetDat().GetOutDeg(); }
    /// Returns ID of EdgeN-th in-node (the node pointing to the current node). ##TNEANetSparse2::TNodeI::GetInNId
    int GetInNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetInEId(EdgeN)).GetSrcNId(); }
    /// Returns ID of EdgeN-th out-node (the node the current node points to). ##TNEANetSparse2::TNodeI::GetOutNId
    int GetOutNId(const int& EdgeN) const { return Graph->GetEdge(NodeHI.GetDat().GetOutEId(EdgeN)).GetDstNId(); }
    /// Returns ID of EdgeN-th neighboring node. ##TNEANetSparse2::TNodeI::GetNbrNId
    int GetNbrNId(const int& EdgeN) const { const TEdge& E = Graph->GetEdge(NodeHI.GetDat().GetNbrEId(EdgeN)); return GetId()==E.GetSrcNId() ? E.GetDstNId():E.GetSrcNId(); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const;
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const;
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return IsOutNId(NId) || IsInNId(NId); }
    /// Returns ID of EdgeN-th in-edge.
    int GetInEId(const int& EdgeN) const { return NodeHI.GetDat().GetInEId(EdgeN); }
    /// Returns ID of EdgeN-th out-edge.
    int GetOutEId(const int& EdgeN) const { return NodeHI.GetDat().GetOutEId(EdgeN); }
    /// Returns ID of EdgeN-th in or out-edge.
    int GetNbrEId(const int& EdgeN) const { return NodeHI.GetDat().GetNbrEId(EdgeN); }
    /// Tests whether the edge with ID EId is an in-edge of current node.
    bool IsInEId(const int& EId) const { return NodeHI.GetDat().IsInEId(EId); }
    /// Tests whether the edge with ID EId is an out-edge of current node.
    bool IsOutEId(const int& EId) const { return NodeHI.GetDat().IsOutEId(EId); }
    /// Tests whether the edge with ID EId is an in or out-edge of current node.
    bool IsNbrEId(const int& EId) const { return IsInEId(EId) || IsOutEId(EId); }
    /// Gets vector of int attribute names.
    TAVIntI GetIntAttrNames() const { return Graph->IntAttrNI(GetId()); }
    /// Gets vector of str attribute names.
    TAVStrI GetStrAttrNames() const { return Graph->StrAttrNI(GetId()); }
    /// Gets vector of flt attribute names.
    TAVFltI GetFltAttrNames() const { return Graph->FltAttrNI(GetId()); }
    friend class TNEANetSparse2;
  };
  /// Edge iterator. Only forward iteration (operator++) is supported.
  class TEdgeI {
  private:
    typedef THash<TInt, TEdge>::TIter THashIter;
    THashIter EdgeHI;
    const TNEANetSparse2 *Graph;
  public:
    TEdgeI() : EdgeHI(), Graph(NULL) { }
    TEdgeI(const THashIter& EdgeHIter, const TNEANetSparse2 *GraphPt) : EdgeHI(EdgeHIter), Graph(GraphPt) { }
    TEdgeI(const TEdgeI& EdgeI) : EdgeHI(EdgeI.EdgeHI), Graph(EdgeI.Graph) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { EdgeHI=EdgeI.EdgeHI; Graph=EdgeI.Graph; }  return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { EdgeHI++; return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return EdgeHI < EdgeI.EdgeHI; }
    bool operator == (const TEdgeI& EdgeI) const { return EdgeHI == EdgeI.EdgeHI; }
    /// Returns edge ID.
    int GetId() const { return EdgeHI.GetDat().GetId(); }
    /// Returns the source of the edge.
    int GetSrcNId() const { return EdgeHI.GetDat().GetSrcNId(); }
    /// Returns the destination of the edge.
    int GetDstNId() const { return EdgeHI.GetDat().GetDstNId(); }
    /// Gets vector of int attribute names.
    TAVIntI GetIntAttrs() const { return Graph->IntAttrEI(GetId()); }
    /// Gets vector of str attribute names.
    TAVStrI GetStrAttrs() const { return Graph->StrAttrEI(GetId()); }
    /// Gets vector of flt attribute names.
    TAVFltI GetFltAttrs() const { return Graph->FltAttrEI(GetId()); }
    friend class TNEANetSparse2;
  };

  /// Node/edge integer attribute iterator. Iterates through all nodes/edges for one integer attribute.
  class TAIntI {
  private:
    typedef TIntV::TIter TIntVecIter;
    TIntVecIter HI;
    bool isNode;
    TStr attr;
    const TNEANetSparse2 *Graph;
  public:
    TAIntI() : HI(), attr(), Graph(NULL) { }
    TAIntI(const TIntVecIter& HIter, TStr attribute, bool isEdgeIter, const TNEANetSparse2* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { isNode = !isEdgeIter; attr = attribute; }
    TAIntI(const TAIntI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAIntI& operator = (const TAIntI& I) { HI = I.HI; Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAIntI& I) const { return HI < I.HI; }
    bool operator == (const TAIntI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TInt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted(TInt defaultValueN=TInt::Mn, TInt defaultValueE=TInt::Mn) const { return isNode ? GetDat() == defaultValueN : GetDat() == defaultValueE; };
    TAIntI& operator++(int) { HI++; return *this; }
    friend class TNEANetSparse2;
  };

  class TAVIntI {
  private:
    typedef TIntV::TIter TIntVecIter;
    TIntVecIter HI;
    TIntVecIter NI;
    bool isNode;
    const TNEANetSparse2 *Graph;
  public:
    TAVIntI() : HI(), NI(), GRAPH(NULL) { }
    TAVIntI(const TIntVecIter& NIter, const TIntVecIter& HIter, bool isEdgeIter, const TNEANetSparse2* GraphPt) : NI(NIter), HI(HIter), Graph(GraphPt) { isNode = !isEdgeIter; }
    TAVIntI(const TAVIntI& I) : NI(I.NI), HI(I.HI), Graph(I.Graph) { isNode = I.isNode; }
    TAVIntI& operator = (const TAVInt& I) { NI = I.NI; HI = I.HI; Graph = I.Graph; isNode = I.isNode; }
    bool operator < (const TAVIntI& I) const { return HI < I.HI; }
    bool operator == (const TAVIntI& I) const { return HI == I.HI; }
    TStr GetName() { return Graph.GetAttrName(NI[0]); }
    TInt GetDat() const { return HI[0]; }
    TAVIntI& operator++(int) { HI++; NI++; return *this; }
    friend class TNEANetSparse2;
  };

  /// Node/edge string attribute iterator. Iterates through all nodes/edges for one string attribute.
  class TAStrI {
  private:
    typedef TStrV::TIter TStrVecIter;
    TStrVecIter HI;
    bool isNode;
    TStr attr;
    const TNEANetSparse2 *Graph;
  public:
    TAStrI() : HI(), attr(), Graph(NULL) { }
    TAStrI(const TStrVecIter& HIter, TStr attribute, bool isEdgeIter, const TNEANetSparse2* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { isNode = !isEdgeIter; attr = attribute; }
    TAStrI(const TAStrI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAStrI& operator = (const TAStrI& I) { HI = I.HI; Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAStrI& I) const { return HI < I.HI; }
    bool operator == (const TAStrI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TStr GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted(TStr defaultValueN=TStr::GetNullStr(), TStr defaultValueE=TStr::GetNullStr()) const { return isNode ? GetDat() == defaultValueN : GetDat() == defaultValueE; };
    TAStrI& operator++(int) { HI++; return *this; }
    friend class TNEANetSparse2;
  };

  class TAVStrI {
  private:
    typedef TIntV::TIter TIntVecIter;
    typedef TStrV::TIter TStrVecIter;
    TStrVecIter HI;
    TIntVecIter NI;
    bool isNode;
    const TNEANetSparse2 *Graph;
  public:
    TAVStrI() : HI(), NI(), GRAPH(NULL) { }
    TAVStrI(const TIntVecIter& NIter, const TStrVecIter& HIter, bool isEdgeIter, const TNEANetSparse2* GraphPt) : NI(NIter), HI(HIter), Graph(GraphPt) { isNode = !isEdgeIter; }
    TAVStrI(const TAVIntI& I) : NI(I.NI), HI(I.HI), Graph(I.Graph) { isNode = I.isNode; }
    TAVStrI& operator = (const TAVStr& I) { NI = I.NI; HI = I.HI; Graph = I.Graph; isNode = I.isNode; }
    bool operator < (const TAVStrI& I) const { return HI < I.HI; }
    bool operator == (const TAVStrI& I) const { return HI == I.HI; }
    TStr GetName() { return Graph.GetAttrName(NI[0]); }
    TInt GetDat() const { return HI[0]; }
    TAVStrI& operator++(int) { HI++; NI++; return *this; }
    friend class TNEANetSparse2;
  };

  /// Node/edge float attribute iterator. Iterates through all nodes/edges for one float attribute.
  class TAFltI {
  private:
    typedef TFltV::TIter TFltVecIter;
    TFltVecIter HI;
    bool isNode;
    TStr attr;
    const TNEANetSparse2 *Graph;
  public:
    TAFltI() : HI(), attr(), Graph(NULL) { }
    TAFltI(const TFltVecIter& HIter, TStr attribute, bool isEdgeIter, const TNEANetSparse2* GraphPt) : HI(HIter), attr(), Graph(GraphPt) { isNode = !isEdgeIter; attr = attribute; }
    TAFltI(const TAFltI& I) : HI(I.HI), attr(I.attr), Graph(I.Graph) { isNode = I.isNode; }
    TAFltI& operator = (const TAFltI& I) { HI = I.HI; Graph=I.Graph; isNode = I.isNode; attr = I.attr; return *this; }
    bool operator < (const TAFltI& I) const { return HI < I.HI; }
    bool operator == (const TAFltI& I) const { return HI == I.HI; }
    /// Returns an attribute of the node.
    TFlt GetDat() const { return HI[0]; }
    /// Returns true if the attribute has been deleted.
    bool IsDeleted(TFlt defaultValueN=TFlt::Mn, TFlt defaultValueE=TFlt::Mn) const { return isNode ? GetDat() == defaultValueN : GetDat() == defaultValueE; };
    TAFltI& operator++(int) { HI++; return *this; }
    friend class TNEANetSparse2;
  };

  class TAVFltI {
  private:
    typedef TIntV::TIter TIntVecIter;
    typedef TFltV::TIter TFltVecIter;
    TFltVecIter HI;
    TIntVecIter NI;
    bool isNode;
    const TNEANetSparse2 *Graph;
  public:
    TAVFltI() : HI(), NI(), GRAPH(NULL) { }
    TAVFltI(const TIntVecIter& NIter, const TFltVecIter& HIter, bool isEdgeIter, const TNEANetSparse2* GraphPt) : NI(NIter), HI(HIter), Graph(GraphPt) { isNode = !isEdgeIter; }
    TAVFltI(const TAVIntI& I) : NI(I.NI), HI(I.HI), Graph(I.Graph) { isNode = I.isNode; }
    TAVFltI& operator = (const TAVFlt& I) { NI = I.NI; HI = I.HI; Graph = I.Graph; isNode = I.isNode; }
    bool operator < (const TAVFltI& I) const { return HI < I.HI; }
    bool operator == (const TAVFltI& I) const { return HI == I.HI; }
    TStr GetName() { return Graph.GetAttrName(NI[0]); }
    TInt GetDat() const { return HI[0]; }
    TAVIntI& operator++(int) { HI++; NI++; return *this; }
    friend class TNEANetSparse2;
  };

private:
  TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }
  const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }
  TEdge& GetEdge(const int& EId) { return EdgeH.GetDat(EId); }
  const TEdge& GetEdge(const int& EId) const { return EdgeH.GetDat(EId); }

  /*/// Get Int node attribute val.  If not a proper attr, return default.
  TInt GetIntAttrDefaultN(const TStr& attribute) const { return IntDefaultsN.IsKey(attribute) ? IntDefaultsN.GetDat(attribute) : (TInt) TInt::Mn; }
  /// Get Str node attribute val.  If not a proper attr, return default.
  TStr GetStrAttrDefaultN(const TStr& attribute) const { return StrDefaultsN.IsKey(attribute) ? StrDefaultsN.GetDat(attribute) : (TStr) TStr::GetNullStr(); }
  /// Get Flt node attribute val.  If not a proper attr, return default.
  TFlt GetFltAttrDefaultN(const TStr& attribute) const { return FltDefaultsN.IsKey(attribute) ? FltDefaultsN.GetDat(attribute) : (TFlt) TFlt::Mn; }
  /// Get Int edge attribute val.  If not a proper attr, return default.
  TInt GetIntAttrDefaultE(const TStr& attribute) const { return IntDefaultsE.IsKey(attribute) ? IntDefaultsE.GetDat(attribute) : (TInt) TInt::Mn; }
  /// Get Str edge attribute val.  If not a proper attr, return default.
  TStr GetStrAttrDefaultE(const TStr& attribute) const { return StrDefaultsE.IsKey(attribute) ? StrDefaultsE.GetDat(attribute) : (TStr) TStr::GetNullStr(); }
  /// Get Flt edge attribute val.  If not a proper attr, return default.
  TFlt GetFltAttrDefaultE(const TStr& attribute) const { return FltDefaultsE.IsKey(attribute) ? FltDefaultsE.GetDat(attribute) : (TFlt) TFlt::Mn; }*/

  TInt GetAttrIdE(const TStr& attribute, const TInt& type) const { 
    TPair<TStr, TInt> key(attribute, type);
    if (AttrToIdE.IsKey(key)) { AttrToIdE.AddKey(key); } 
    return AttrToIdE.GetKeyId(key); 
  }
  bool IsAttrE(const TStr& attribute, const TInt& type) const { 
    TPair<TStr, TInt> key(attribute, type);
    return AttrToIdE.IsKey(key); 
  }
  TStr GetAttrNameE(const TInt AttrId) const { return AttrToIdE.GetKey(AttrId).GetVal1(); }
  TInt GetAttrTypeE(const TInt AttrId) const { return AttrToIdE.GetKey(AttrId).GetVal2(); }

  TInt GetAttrIdN(const TStr& attribute, const TInt& type) const { 
    TPair<TStr, TInt> key(attribute, type); 
    if (AttrToIdN.IsKey(key)) { AttrToIdN.AddKey(key); } 
    return AttrToIdN.GetKeyId(key);
  }
  bool IsAttrN(const TStr& attribute, const TInt& type) const { 
    TPair<TStr, TInt> key(attribute, type); 
    return AttrToIdN.IsKey(key);
  }
  TStr GetAttrNameN(const TInt AttrId) const { return AttrToIdN.GetKey(AttrId).GetVal1(); }
  TInt GetAttrTypeN(const TInt AttrId) const { return AttrToIdN.GetKey(AttrId).GetVal2(); }

private:
  TCRef CRef;
  TInt MxNId, MxEId;
  THash<TInt, TNode> NodeH;
  THash<TInt, TEdge> EdgeH;
  /// KeyToIndexType[N|E]: Key->(Type,Index).
  TStrIntPrH KeyToIndexTypeN, KeyToIndexTypeE;

  //THash<TStr, TInt> IntDefaultsN, IntDefaultsE;
  //THash<TStr, TStr> StrDefaultsN, StrDefaultsE;
  //THash<TStr, TFlt> FltDefaultsN, FltDefaultsE;

  THashSet<TPair<TStr, TInt> > AttrToIdE;
  THashSet<TPair<TStr, TInt> > AttrToIdN;

  THash<TPair<TInt, TInt>, TInt> IntAttrsN;
  THash<TPair<TInt, TInt>, TFlt> FltAttrsN;
  THash<TPair<TInt, TInt>, TStr> StrAttrsN;

  THash<TPair<TInt, TInt>, TInt> IntAttrsE;
  THash<TPair<TInt, TInt>, TFlt> FltAttrsE;
  THash<TPair<TInt, TInt>, TStr> StrAttrsE;

  // REMOVE THESE
  // REMOVE THESE

  enum { IntType, StrType, FltType };
public:

  //UPDATE !!!!!!!!!!!
  TNEANetSparse2() : CRef(), MxNId(0), MxEId(0), NodeH(), EdgeH(),
    KeyToIndexTypeN(), KeyToIndexTypeE(), AttrToIdE(), AttrToIdN(), IntAttrsN(), FltAttrsN(), StrAttrsN(),
    IntAttrsE(), FltAttrsE(), StrAttrsE() { }
  /// Constructor that reserves enough memory for a graph of nodes and edges.
  explicit TNEANetSparse2(const int& Nodes, const int& Edges) : CRef(),
    MxNId(0), MxEId(0), NodeH(), EdgeH(), KeyToIndexTypeN(), KeyToIndexTypeE(), AttrToIdE(), AttrToIdN(),
    IntAttrsN(), FltAttrsN(), StrAttrsN(), IntAttrsE(), FltAttrsE(), StrAttrsE()
    { Reserve(Nodes, Edges); }
  TNEANetSparse2(const TNEANetSparse2& Graph) : MxNId(Graph.MxNId), MxEId(Graph.MxEId),
    NodeH(Graph.NodeH), EdgeH(Graph.EdgeH), KeyToIndexTypeN(), KeyToIndexTypeE(), AttrToIdE(Graph.AttrToIdE), AttrToIdN(Graph.AttrToIdN),
    IntAttrsN(Graph.IntAttrsN), FltAttrsN(Graph.FltAttrsN), StrAttrsN(Graph.StrAttrsN),
    IntAttrsE(Graph.IntAttrsE), FltAttrsE(Graph.FltAttrsE), StrAttrsE(Graph.StrAttrsE) { }
  /// Constructor for loading the graph from a (binary) stream SIn.
  TNEANetSparse2(TSIn& SIn) : MxNId(SIn), MxEId(SIn), NodeH(SIn), EdgeH(SIn),
    KeyToIndexTypeN(SIn), KeyToIndexTypeE(SIn), AttrToIdE(SIn), AttrToIdN(SIn),
    IntAttrsN(SIn), FltAttrsN(SIn), StrAttrsN(SIn), IntAttrsE(SIn), FltAttrsE(SIn), StrAttrsE(SIn) { }
  /// Saves the graph to a (binary) stream SOut.
  void Save(TSOut& SOut) const {
    MxNId.Save(SOut); MxEId.Save(SOut); NodeH.Save(SOut); EdgeH.Save(SOut);
    KeyToIndexTypeN.Save(SOut); KeyToIndexTypeE.Save(SOut); AttrToIdE.Save(SOut); AttrToIdN.Save(SOut);
    IntAttrsN.Save(SOut); FltAttrsN.Save(SOut); StrAttrsN.Save(SOut); IntAttrsE.Save(SOut); FltAttrsE.Save(SOut); StrAttrsE.Save(SOut);}



  /// Static cons returns pointer to graph. Ex: PNEANetSparse2 Graph=TNEANetSparse2::New().
  static PNEANetSparse2 New() { return PNEANetSparse2(new TNEANetSparse2()); }
  /// Static constructor that returns a pointer to the graph and reserves enough memory for Nodes nodes and Edges edges. ##TNEANetSparse2::New
  static PNEANetSparse2 New(const int& Nodes, const int& Edges) { return PNEANetSparse2(new TNEANetSparse2(Nodes, Edges)); }
  /// Static constructor that loads the graph from a stream SIn and returns a pointer to it.
  static PNEANetSparse2 Load(TSIn& SIn) { return PNEANetSparse2(new TNEANetSparse2(SIn)); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  TNEANetSparse2& operator = (const TNEANetSparse2& Graph) { if (this!=&Graph) {
    MxNId=Graph.MxNId; MxEId=Graph.MxEId; NodeH=Graph.NodeH; EdgeH=Graph.EdgeH; }
    return *this; }

  /// Returns the number of nodes in the graph.
  int GetNodes() const { return NodeH.Len(); }
  /// Adds a node of ID NId to the graph. ##TNEANetSparse2::AddNode
  int AddNode(int NId = -1);
  /// Adds a node of ID NodeI.GetId() to the graph.
  int AddNode(const TNodeI& NodeId) { return AddNode(NodeId.GetId()); }
  /// Deletes node of ID NId from the graph. ##TNEANetSparse2::DelNode
  void DelNode(const int& NId);
  /// Deletes node of ID NodeI.GetId() from the graph.
  void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }
  /// Tests whether ID NId is a node.
  bool IsNode(const int& NId) const { return NodeH.IsKey(NId); }
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() const { return TNodeI(NodeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  TNodeI EndNI() const { return TNodeI(NodeH.EndI(), this); }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId), this); }

  /// Returns an iterator referring to the first node's int attribute.
  TAIntI BegNAIntI(const TStr& attr) const {
    TIntV vec;
    GetIntAttrVecN(attr, vec);
    return TAIntI(vec.BegI(), attr, false, this);
    /* return TAIntI(VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2].BegI(), attr, false, this);*/ }
  /// Returns an iterator referring to the past-the-end node's attribute.
  TAIntI EndNAIntI(const TStr& attr) const {
    TIntV vec;
    GetIntAttrVecN(attr, vec);
    return TAIntI(vec.EndI(), attr, false, this);
    /* return TAIntI(VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2].EndI(), attr, false, this);*/ }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TAIntI GetNAIntI(const TStr& attr, const int& NId) const {
    TIntV vec;
    int index = GetIntAttrVecN(attr, vec, NId);
    return TAIntI(vec.GetI(index), attr, false, this);
    /*return TAIntI(VecOfIntVecsN[KeyToIndexTypeN.GetDat(attr).Val2].GetI(NodeH.GetKeyId(NId)), attr, false, this);*/ }
  /// Returns an iterator referring to the first node's str attribute.
  TAStrI BegNAStrI(const TStr& attr) const {
    TStrV vec;
    GetStrAttrVecN(attr, vec);
    return TAStrI(vec.BegI(), attr, false, this);
    /*return TAStrI(VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2].BegI(), attr, false, this);*/ }
  /// Returns an iterator referring to the past-the-end node's attribute.
  TAStrI EndNAStrI(const TStr& attr) const {
    TStrV vec;
    GetStrAttrVecN(attr, vec);
    return TAStrI(vec.EndI(), attr, false, this);
    /*return TAStrI(VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2].EndI(), attr, false, this);*/ }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TAStrI GetNAStrI(const TStr& attr, const int& NId) const {
    TStrV vec;
    int index = GetStrAttrVecN(attr, vec, NId);
    return TAStrI(vec.GetI(index), attr, false, this);
    /*return TAStrI(VecOfStrVecsN[KeyToIndexTypeN.GetDat(attr).Val2].GetI(NodeH.GetKeyId(NId)), attr, false, this);*/ }
  /// Returns an iterator referring to the first node's flt attribute.
  TAFltI BegNAFltI(const TStr& attr) const {
    TFltV vec;
    GetFltAttrVecN(attr, vec);
    return TAFltI(vec.BegI(), attr, false, this);
    /*return TAFltI(VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2].BegI(), attr, false, this);*/ }
  /// Returns an iterator referring to the past-the-end node's attribute.
  TAFltI EndNAFltI(const TStr& attr) const {
    TFltV vec;
    GetFltAttrVecN(attr, vec);
    return TAFltI(vec.EndI(), attr, false, this);
    /*return TAFltI(VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2].EndI(), attr, false, this);*/ }
  /// Returns an iterator referring to the node of ID NId in the graph.
  TAFltI GetNAFltI(const TStr& attr, const int& NId) const {
    TFltV vec;
    int index = GetFltAttrVecN(attr, vec, NId);
    return TAFltI(vec.GetI(index), attr, false, this);
    /* return TAFltI(VecOfFltVecsN[KeyToIndexTypeN.GetDat(attr).Val2].GetI(NodeH.GetKeyId(NId)), attr, false, this);*/ }


  // FIX THESE!!!!!!!!!!!

  /// Returns a vector of int attr names for node NId.
  void IntAttrNI(const TInt& NId) const { const TNode& Node = NodeHI.GetDat(NId); return TAVIntI(Node.IntAttrIds, Node.IntAttrs, false, Graph); }
  /// Returns a vector of str attr names for node NId.
  void StrAttrNI(const TInt& NId) const { const TNode& Node = NodeHI.GetDat(NId); return TAVStrI(Node.StrAttrIds, Node.StrAttrs, false, Graph); }
  /// Returns a vector of int attr names for node NId.
  void FltAttrNI(const TInt& NId) const { const TNode& Node = NodeHI.GetDat(NId); return TAVFltI(Node.FltAttrIds, Node.FltAttrs, false, Graph); }

  /// Returns a vector of int attr names for edge EId.
  void IntAttrEI(const TInt& EId) const { const TEdge& Edge = EdgeHI.GetDat(EI); return TAVIntI(Edge.IntAttrIds, Edge.IntAttrs, true, Graph); }
  /// Returns a vector of str attr names for node NId.
  void StrAttrEI(const TInt& EId) const { const TEdge& Edge = EdgeHI.GetDat(EI); return TAVStrI(Edge.StrAttrIds, Edge.StrAttrs, true, Graph); }
  /// Returns a vector of int attr names for node NId.
  void FltAttrEI(const TInt& EId) const { const TEdge& Edge = EdgeHI.GetDat(EI); return TAVFltI(Edge.FltAttrIds, Edge.FltAttrs, true, Graph); }




  /// Returns an iterator referring to the first edge's int attribute.
  TAIntI BegEAIntI(const TStr& attr) const {
    TIntV vec;
    GetIntAttrVecE(attr, vec);
    return TAIntI(vec.BegI(), attr, true, this);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAIntI EndEAIntI(const TStr& attr) const {
    TIntV vec;
    GetIntAttrVecE(attr, vec);
    return TAIntI(vec.EndI(), attr, true, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAIntI GetEAIntI(const TStr& attr, const int& EId) const {
    TIntV vec;
    int index = GetIntAttrVecE(attr, vec, EId);
    return TAIntI(vec.GetI(index), attr, true, this);
  }
  /// Returns an iterator referring to the first edge's str attribute.
  TAStrI BegEAStrI(const TStr& attr) const {
    TStrV vec;
    GetStrAttrVecE(attr, vec);
    return TAStrI(vec.BegI(), attr, true, this);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAStrI EndEAStrI(const TStr& attr) const {
    TStrV vec;
    GetStrAttrVecE(attr, vec);
    return TAStrI(vec.EndI(), attr, true, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAStrI GetEAStrI(const TStr& attr, const int& EId) const {
    TStrV vec;
    int index = GetStrAttrVecE(attr, vec, EId);
    return TAStrI(vec.GetI(index), attr, true, this);
  }
  /// Returns an iterator referring to the first edge's flt attribute.
  TAFltI BegEAFltI(const TStr& attr) const {
    TFltV vec;
    GetFltAttrVecE(attr, vec);
    return TAFltI(vec.BegI(), attr, true, this);
  }
  /// Returns an iterator referring to the past-the-end edge's attribute.
  TAFltI EndEAFltI(const TStr& attr) const {
    TFltV vec;
    GetFltAttrVecE(attr, vec);
    return TAFltI(vec.EndI(), attr, true, this);
  }
  /// Returns an iterator referring to the edge of ID EId in the graph.
  TAFltI GetEAFltI(const TStr& attr, const int& EId) const {
    TFltV vec;
    int index = GetFltAttrVecE(attr, vec, EId);
    return TAFltI(vec.GetI(index), attr, true, this);
    //return TAFltI(VecOfFltVecsE[KeyToIndexTypeE.GetDat(attr).Val2].GetI(EdgeH.GetKeyId(EId)), attr, true, this);
  }


  /// Returns an ID that is larger than any node ID in the network.
  int GetMxNId() const { return MxNId; }
  /// Returns an ID that is larger than any edge ID in the network.
  int GetMxEId() const { return MxEId; }

  /// Returns the number of edges in the graph.
  int GetEdges() const { return EdgeH.Len(); }
  /// Adds an edge with ID EId between node IDs SrcNId and DstNId to the graph. ##TNEANetSparse2::AddEdge
  int AddEdge(const int& SrcNId, const int& DstNId, int EId  = -1);
  /// Adds an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the graph.
  int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId(), EdgeI.GetId()); }
  /// Deletes an edge with edge ID EId from the graph.
  void DelEdge(const int& EId);
  /// Deletes all edges between node IDs SrcNId and DstNId from the graph. ##TNEANetSparse2::DelEdge
  void DelEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true);
  /// Tests whether an edge with edge ID EId exists in the graph.
  bool IsEdge(const int& EId) const { return EdgeH.IsKey(EId); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const int& SrcNId, const int& DstNId, const bool& IsDir = true) const { int EId; return IsEdge(SrcNId, DstNId, EId, IsDir); }
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph. if an edge exists, return its edge ID in EId
  bool IsEdge(const int& SrcNId, const int& DstNId, int& EId, const bool& IsDir = true) const;
  /// Returns an edge ID between node IDs SrcNId and DstNId, if such an edge exists. Otherwise, return -1.
  int GetEId(const int& SrcNId, const int& DstNId) const { int EId; return IsEdge(SrcNId, DstNId, EId)?EId:-1; }
  /// Returns an iterator referring to the first edge in the graph.
  TEdgeI BegEI() const { return TEdgeI(EdgeH.BegI(), this); }
  /// Returns an iterator referring to the past-the-end edge in the graph.
  TEdgeI EndEI() const { return TEdgeI(EdgeH.EndI(), this); }
  /// Returns an iterator referring to edge with edge ID EId. 
  TEdgeI GetEI(const int& EId) const { return TEdgeI(EdgeH.GetI(EId), this); }
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph. 
  TEdgeI GetEI(const int& SrcNId, const int& DstNId) const { return GetEI(GetEId(SrcNId, DstNId)); }

  /// Returns an ID of a random node in the graph.
  int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random node in the graph.
  TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }
  /// Returns an ID of a random edge in the graph.
  int GetRndEId(TRnd& Rnd=TInt::Rnd) { return EdgeH.GetKey(EdgeH.GetRndKeyId(Rnd, 0.8)); }
  /// Returns an interator referring to a random edge in the graph.
  TEdgeI GetRndEI(TRnd& Rnd=TInt::Rnd) { return GetEI(GetRndEId(Rnd)); }
  /// Gets a vector IDs of all nodes in the graph.
  void GetNIdV(TIntV& NIdV) const;
  /// Gets a vector IDs of all edges in the graph.
  void GetEIdV(TIntV& EIdV) const;

  /// Tests whether the graph is empty (has zero nodes).
  bool Empty() const { return GetNodes()==0; }


  //UPDATE !!!!!!!!!!!!!!!!
  /// Deletes all nodes and edges from the graph.
  void Clr() { MxNId=0; MxEId=0; NodeH.Clr(); EdgeH.Clr(),
    KeyToIndexTypeN.Clr(), KeyToIndexTypeE.Clr(); AttrToIdE.Clr(); AttrToIdN.Clr();
    IntAttrsN.Clr(); FltAttrsN.Clr(); StrAttrsN.Clr(); IntAttrsE.Clr(); FltAttrsE.Clr(); StrAttrsE.Clr();}




  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) {
    if (Nodes>0) { NodeH.Gen(Nodes/2); } if (Edges>0) { EdgeH.Gen(Edges/2); } }
  /// Defragments the graph. ##TNEANetSparse2::Defrag
  void Defrag(const bool& OnlyNodeLinks=false);
  /// Checks the graph data structure for internal consistency. ##TNEANetSparse2::IsOk
  bool IsOk(const bool& ThrowExcept=true) const;
  /// Print the graph in a human readable form to an output stream OutF.
  void Dump(FILE *OutF=stdout) const;

  /// Attribute based add function for attr to Int value. ##TNEANetSparse2::AddIntAttrDatN
  int AddIntAttrDatN(const TNodeI& NodeId, const TInt& value, const TStr& attr) { return AddIntAttrDatN(NodeId.GetId(), value, attr); }
  int AddIntAttrDatN(const int& NId, const TInt& value, const TStr& attr);
  /// Attribute based add function for attr to Str value. ##TNEANetSparse2::AddStrAttrDatN
  int AddStrAttrDatN(const TNodeI& NodeId, const TStr& value, const TStr& attr) { return AddStrAttrDatN(NodeId.GetId(), value, attr); }
  int AddStrAttrDatN(const int& NId, const TStr& value, const TStr& attr);
  /// Attribute based add function for attr to Flt value. ##TNEANetSparse2::AddFltAttrDatN
  int AddFltAttrDatN(const TNodeI& NodeId, const TFlt& value, const TStr& attr) { return AddFltAttrDatN(NodeId.GetId(), value, attr); }
  int AddFltAttrDatN(const int& NId, const TFlt& value, const TStr& attr);

  /// Attribute based add function for attr to Int value. ##TNEANetSparse2::AddIntAttrDatE
  int AddIntAttrDatE(const TEdgeI& EdgeId, const TInt& value, const TStr& attr) { return AddIntAttrDatE(EdgeId.GetId(), value, attr); }
  int AddIntAttrDatE(const int& EId, const TInt& value, const TStr& attr);
  /// Attribute based add function for attr to Str value. ##TNEANetSparse2::AddStrAttrDatE
  int AddStrAttrDatE(const TEdgeI& EdgeId, const TStr& value, const TStr& attr) { return AddStrAttrDatE(EdgeId.GetId(), value, attr); }
  int AddStrAttrDatE(const int& EId, const TStr& value, const TStr& attr);
  /// Attribute based add function for attr to Flt value. ##TNEANetSparse2::AddFltAttrDatE
  int AddFltAttrDatE(const TEdgeI& EdgeId, const TFlt& value, const TStr& attr) { return AddFltAttrDatE(EdgeId.GetId(), value, attr); }
  int AddFltAttrDatE(const int& EId, const TFlt& value, const TStr& attr);

  /// Gets the value of int attr from the node attr value vector.
  TInt GetIntAttrDatN(const TNodeI& NodeId, const TStr& attr) { return GetIntAttrDatN(NodeId.GetId(), attr); }
  TInt GetIntAttrDatN(const int& NId, const TStr& attr);

  /// Gets the value of str attr from the node attr value vector.
  TStr GetStrAttrDatN(const TNodeI& NodeId, const TStr& attr) { return GetStrAttrDatN(NodeId.GetId(), attr); }
  TStr GetStrAttrDatN(const int& NId, const TStr& attr);
  /// Gets the value of flt attr from the node attr value vector.
  TFlt GetFltAttrDatN(const TNodeI& NodeId, const TStr& attr) { return GetFltAttrDatN(NodeId.GetId(), attr); }
  TFlt GetFltAttrDatN(const int& NId, const TStr& attr);

  /// Gets the value of int attr from the edge attr value vector.
  TInt GetIntAttrDatE(const TEdgeI& EdgeId, const TStr& attr) { return GetIntAttrDatE(EdgeId.GetId(), attr); }
  TInt GetIntAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of str attr from the edge attr value vector.
  TStr GetStrAttrDatE(const TEdgeI& EdgeId, const TStr& attr) { return GetStrAttrDatE(EdgeId.GetId(), attr); }
  TStr GetStrAttrDatE(const int& EId, const TStr& attr);
  /// Gets the value of flt attr from the edge attr value vector.
  TFlt GetFltAttrDatE(const TEdgeI& EdgeId, const TStr& attr) { return GetFltAttrDatE(EdgeId.GetId(), attr); }
  TFlt GetFltAttrDatE(const int& EId, const TStr& attr);
 
  /// Deletes the node attribute for NodeId.
  int DelAttrDatN(const TNodeI& NodeId, const TStr& attr) { return DelAttrDatN(NodeId.GetId(), attr); } 
  int DelAttrDatN(const int& NId, const TStr& attr); 
  /// Deletes the edge attribute for NodeId.
  int DelAttrDatE(const TEdgeI& EdgeId, const TStr& attr) { return DelAttrDatE(EdgeId.GetId(), attr); } 
  int DelAttrDatE(const int& EId, const TStr& attr); 

  // Returns true if NId deleted for current node attr iterator.
  bool NodeAttrIsDeleted(const int& NId, const TStr& attr) const;
  // Returns true if NId deleted for current node int attr iterator.
  bool NodeAttrIsIntDeleted(const int& NId, const TStr& attr) const;
  // Returns true if NId deleted for current node str attr iterator.
  bool NodeAttrIsStrDeleted(const int& NId, const TStr& attr) const;
  // Returns true if NId deleted for current node flt attr iterator.
  bool NodeAttrIsFltDeleted(const int& NId, const TStr& attr) const;

  // Returns true if EId deleted for current edge attr iterator.
  bool EdgeAttrIsDeleted(const int& EId, const TStr& attr) const;
  // Returns true if EId deleted for current edge int attr iterator.
  bool EdgeAttrIsIntDeleted(const int& EId, const TStr& attr) const;
  // Returns true if EId deleted for current edge str attr iterator.
  bool EdgeAttrIsStrDeleted(const int& EId, const TStr& attr) const;
  // Returns true if EId deleted for current edge flt attr iterator.
  bool EdgeAttrIsFltDeleted(const int& EId, const TStr& attr) const;

  // Get Vector for the Flt Attribute attr.
  int GetFltAttrVecE(const TStr& attr, TVec<TFlt>& FltAttrs, int EId=-1);

  // Get Vector for the Int Attribute attr.
  int GetIntAttrVecE(const TStr& attr, TVec<TInt>& IntAttrs, int EId=-1);

  // Get Vector for the Str Attribute attr.
  int GetStrAttrVecE(const TStr& attr, TVec<TStr>& StrAttrs, int EId=-1);

    // Get Vector for the Flt Attribute attr.
  int GetFltAttrVecN(const TStr& attr, TVec<TFlt>& FltAttrs, int NId=-1);

  // Get Vector for the Int Attribute attr.
  int GetIntAttrVecN(const TStr& attr, TVec<TInt>& IntAttrs, int NId=-1);

  // Get Vector for the Str Attribute attr.
  int GetStrAttrVecN(const TStr& attr, TVec<TStr>& StrAttrs, int NId=-1);
 
  /// Returns a small multigraph on 5 nodes and 6 edges. ##TNEANetSparse2::GetSmallGraph
  static PNEANetSparse2 GetSmallGraph();
  friend class TPt<TNEANetSparse2>;
};

// set flags
namespace TSnap {
template <> struct IsMultiGraph<TNEANetSparse2> { enum { Val = 1 }; };
template <> struct IsDirected<TNEANetSparse2> { enum { Val = 1 }; };
}
