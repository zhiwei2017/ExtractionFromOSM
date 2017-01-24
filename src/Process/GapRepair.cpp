// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include "./GapRepair.h"
#include <list>
#include <deque>
#include <utility>
#include <tuple>

using std::make_pair;
using std::make_tuple;
using std::list;
using std::deque;

// _____________________________________________________________________________
GapRepair::GapRepair(Network* nPtr) {
  nPtr_ = nPtr;
  gapNum_ = 0;
}

// _____________________________________________________________________________
void GapRepair::GapCheck() {
  for (auto it = nPtr_->GetGraphMapBegin(); it != nPtr_->GetGraphMapEnd();
         ++it) {
    Graph* graph = &it->second;
    if (graph->GetEdgeListSize() < 2) {
      continue;
    }

    unordered_set<Key, KeyHash, KeyEqual> gapSet;
    for (int index = 0; index < graph->GetEdgeListSize() - 1; ++index) {
      ID edgeIDFront = graph->GetEdgeID(index);
      ID edgeIDBack = graph->GetEdgeID(index + 1);
      Edge* edgeFront = nPtr_->GetEdgePtr(edgeIDFront);
      Edge* edgeBack = nPtr_->GetEdgePtr(edgeIDBack);

      if (AreConjoint(edgeFront, edgeBack)) continue;

      Key idPair1 = make_tuple(edgeIDFront, edgeIDBack);
      Key idPair2 = make_tuple(edgeIDBack, edgeIDFront);
      if (!gapSet.count(idPair1) && !gapSet.count(idPair2)) {
        gapSet.insert(idPair1);
      }

      ID graphID = it->first;
      if (!graphGaps_.count(graphID)) {
        graphGaps_.emplace(graphID, Gap(graphID, index));
      } else {
        graphGaps_.at(graphID).PushBackGap(index);
      }
    }
    gapNum_ += gapSet.size();
  }
}

// _____________________________________________________________________________
void GapRepair::BuildRTree() {
  for (auto it = nPtr_->GetEdgeMapBegin(); it != nPtr_->GetEdgeMapEnd(); it++) {
    Edge* edge = &it->second;
    if (edge->GetVertexListSize() > 0) {
      LineString line = nPtr_->GetLine(edge);
      Box b = BoundingBox(line);
      MegaNode mNode(edge->GetID(), line);
      geometryRTree_.insert(make_pair(b, mNode));
    }
  }
}

// _____________________________________________________________________________
int GapRepair::GapFix(const ID& graphID, const int& gapIndex,
                     unordered_set<Key, KeyHash, KeyEqual>* gapSet) {
  Graph* graph = nPtr_->GetGraphPtr(graphID);
  ID edgeIdStart = graph->GetEdgeID(gapIndex);
  ID edgeIdEnd = graph->GetEdgeID(gapIndex + 1);

  // count gap number
  Key idPair1 = make_tuple(edgeIdStart, edgeIdEnd);
  Key idPair2 = make_tuple(edgeIdEnd, edgeIdStart);
  if (!gapSet->count(idPair1) && !gapSet->count(idPair2)) {
    gapSet->insert(idPair1);
  }

  // create the start vertex
  Edge* edgeStart = nPtr_->GetEdgePtr(edgeIdStart);
  LineString lineStart = nPtr_->GetLine(edgeStart);
  MegaNode source(edgeIdStart, lineStart);

  // create the end vertex
  Edge* edgeEnd = nPtr_->GetEdgePtr(edgeIdEnd);
  LineString lineEnd = nPtr_->GetLine(edgeEnd);
  MegaNode target(edgeIdEnd, lineEnd);

  deque<TopoNode> list = AStar(source, target);
//  deque<TopoNode> list = Dijkstra(source, target);
  graph->InsertEdge(gapIndex + 1, list);
  if (list.size() && AreConjoint(nPtr_->GetEdgePtr(list.back().GetID()),
                                    edgeEnd))
    return list.size();
  else
    return list.size() * -1;
}

// _____________________________________________________________________________
void GapRepair::Repair() {
  auto gapIt = graphGaps_.begin();
  while (gapIt != graphGaps_.end()) {
    Gap* gap = &gapIt->second;
    unordered_set<Key, KeyHash, KeyEqual> gapSet;
    list<int>::iterator it = gap->GetGapListBegin();
    while (it!= gap->GetGapListEnd()) {
      int numEdges = GapFix(gap->GetID(), (*it) + gap->GetOffset(), &gapSet);
      if (numEdges > 0) {
        gap->UpdateOffset(numEdges);
        it = gap->DeleteGap(it);
        continue;
      }
      break;
    }
    gapNum_ -= gapSet.size() - 1;
    if (gap->GetSize() == 0) {
      gapIt = graphGaps_.erase(gapIt);
      continue;
    }
    gapIt++;
  }
}

// _____________________________________________________________________________
int GapRepair::GetSize() const {
  return gapNum_;
}

// _____________________________________________________________________________
void GapRepair::Run() {
  GapCheck();
  std::cout << "Gap Number: " << GetSize() << std::endl;
  BuildRTree();
  Repair();
  std::cout << "After Fixing Gap Number: " << GetSize() << std::endl;
}
