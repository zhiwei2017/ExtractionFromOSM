// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include "./OrderRepair.h"
#include <vector>
#include <deque>
#include <utility>
#include <tuple>
#include <limits>
#include <algorithm>

using std::unordered_map;
using std::vector;
using std::pair;
using std::make_pair;
using std::deque;
using std::tuple;
using std::make_tuple;

// _____________________________________________________________________________
OrderRepair::OrderRepair(Network* nPtr) {
  nPtr_ = nPtr;
}

// _____________________________________________________________________________
void OrderRepair::BuildRTree(Graph* graphPtr, vector<MegaNode>* edgeListPtr) {
  int index(0);
  while (index < graphPtr->GetEdgeListSize()) {
    ID edgeID = graphPtr->GetEdgeID(index);
    Edge* edgePtr = nPtr_->GetEdgePtr(edgeID);
    if (!edgePtr->GetVertexListSize()) {
      graphPtr->DeleteEdge(index);
      continue;
    }

    LineString line = nPtr_->GetLine(edgePtr);
    Box b = BoundingBox(line);
    MegaNode mNode(edgeID, line);
    geometryRTree_.insert(make_pair(b, mNode));

    edgeListPtr->push_back(mNode);
    ++index;
  }
}

// _____________________________________________________________________________
void OrderRepair::OrderCheck(Graph* graphPtr, vector<int>* wrongOrderListPtr) {
  for (int index = 1; index < graphPtr->GetEdgeListSize(); ++index) {
    ID frontEdgeID = graphPtr->GetEdgeID(index - 1);
    ID backEdgeID = graphPtr->GetEdgeID(index);
    Edge* frontEdgePtr = nPtr_->GetEdgePtr(frontEdgeID);
    Edge* backEdgePtr = nPtr_->GetEdgePtr(backEdgeID);
    if (!AreConjoint(frontEdgePtr, backEdgePtr)) {
      wrongOrderListPtr->push_back(index - 1);
    }
  }
}

// _____________________________________________________________________________
void OrderRepair::FindHeadEdge(deque<TopoNode>* listPtr) {
  unordered_map<ID, int> hash;
  for (int i = 0; i < listPtr->size(); ++i) {
    if (hash.count(listPtr->at(i).GetID()))
      ++hash[listPtr->at(i).GetID()];
    else
      hash.emplace(listPtr->at(i).GetID(), 1);
  }
  auto it = listPtr->begin();
  while (it != listPtr->end() && hash[it->GetID()] > 1) {
    it = listPtr->erase(it);
  }
}

// _____________________________________________________________________________
size_t OrderRepair::Repair(Graph* graphPtr, const vector<MegaNode>& edgeList) {
  vector<int> wrongOrderList;
  OrderCheck(graphPtr, &wrongOrderList);

  int offset(0);
  for (auto gapIndex : wrongOrderList) {
    deque<TopoNode> list = BidirectionalAStar(edgeList[gapIndex],
                                              edgeList[gapIndex + 1]);
    graphPtr->InsertEdge(gapIndex + offset + 1, list);
    offset += list.size();
  }
  FindHeadEdge(&graphPtr->GetEdgeList());
  return wrongOrderList.size();
}

// _____________________________________________________________________________
void OrderRepair::Run() {
  int gapNum(0);
  auto it = nPtr_->GetGraphMapBegin();
  while (it != nPtr_->GetGraphMapEnd()) {
    Graph* graphPtr = &it->second;
    if (graphPtr->GetEdgeListSize() < 2) {
      if (!graphPtr->GetEdgeListSize())
        it = nPtr_->DeleteGraph(it);
      else
        it++;
      continue;
    }

    vector<MegaNode> edgeList;
    BuildRTree(graphPtr, &edgeList);

    if (!graphPtr->GetEdgeListSize()) {
      it = nPtr_->DeleteGraph(it);
      continue;
    }

    gapNum += Repair(graphPtr, edgeList);

    geometryRTree_.clear();
    it++;
  }
  std::cout << "before: " << gapNum << std::endl;
}
