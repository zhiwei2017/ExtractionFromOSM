// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include "./ConnectionRepair.h"
#include <deque>
#include <vector>
#include <utility>

using std::pair;
using std::vector;
using std::deque;

// _____________________________________________________________________________
ConnectionRepair::ConnectionRepair(Network* nPtr) {
  nPtr_ = nPtr;
}

// _____________________________________________________________________________
vector<ID> ConnectionRepair::CreateNewEdges(const ID& oldEdgeID, bool three) {
  ID newEdgeID1 = oldEdgeID + 1;
  while (nPtr_->ContainEdge(newEdgeID1)) {
    newEdgeID1++;
  }
  ID newEdgeID2 = newEdgeID1 + 1;
  while (nPtr_->ContainEdge(newEdgeID2)) {
    newEdgeID2++;
  }
  nPtr_->AddEdge(newEdgeID1);
  nPtr_->AddEdge(newEdgeID2);
  if (!three) return {newEdgeID1, newEdgeID2};

  ID newEdgeID3 = newEdgeID2 + 1;
  while (nPtr_->ContainEdge(newEdgeID3)) {
    newEdgeID3++;
  }
  nPtr_->AddEdge(newEdgeID3);
  return {newEdgeID1, newEdgeID2, newEdgeID3};
}

// _____________________________________________________________________________
void ConnectionRepair::SeparateEdge(Graph* graphPtr, const int& index,
                                    const int& newState) {
  // create new edges
  ID oldEdgeID = graphPtr->GetEdgeID(index);
  vector<ID> newEdgeIDs = CreateNewEdges(oldEdgeID);
  int oldState = graphPtr->GetEdgeState(index);
  int separatePosition = oldState > Redundant ? oldState : newState;

  // seperate the vertices in the old edge's vertex list into two new
  // edges' vertex lists
  Edge* oldEdgePtr = nPtr_->GetEdgePtr(oldEdgeID);
  deque<ID>::iterator oldEdgeBegin = oldEdgePtr->GetVertexListBegin();
  deque<ID>::iterator oldEdgeEnd = oldEdgePtr->GetVertexListEnd();

  Edge* edgeFrontPtr = nPtr_->GetEdgePtr(newEdgeIDs[0]);
  edgeFrontPtr->InsertVertex(oldEdgeBegin, oldEdgeBegin + separatePosition + 1);

  Edge* edgeBackPtr = nPtr_->GetEdgePtr(newEdgeIDs[1]);
  edgeBackPtr->InsertVertex(oldEdgeBegin + separatePosition, oldEdgeEnd);

  ID vertexIDFirst = edgeFrontPtr->GetFrom();
  Vertex* vertexFirstPtr = nPtr_->GetVertexPtr(vertexIDFirst);
  vertexFirstPtr->AddToAdjacenyList(edgeFrontPtr->GetID());

  ID vertexIDMiddle = edgeBackPtr->GetFrom();
  Vertex* vertexMiddlePtr = nPtr_->GetVertexPtr(vertexIDMiddle);
  vertexMiddlePtr->AddToAdjacenyList(edgeFrontPtr->GetID());
  vertexMiddlePtr->AddToAdjacenyList(edgeBackPtr->GetID());

  ID vertexIDLast = edgeBackPtr->GetTo();
  Vertex* vertexLastPtr = nPtr_->GetVertexPtr(vertexIDLast);
  vertexLastPtr->AddToAdjacenyList(edgeBackPtr->GetID());

  graphPtr->DeleteEdge(index);
  // replace the oldedge by two new edges
  if (oldState == Normal && newState > Redundant) {
    graphPtr->InsertEdge(index, newEdgeIDs[1], NormalRedundant);
    graphPtr->InsertEdge(index, newEdgeIDs[0], Normal);
  } else if (oldState == Inverse && newState > Redundant) {
    graphPtr->InsertEdge(index, newEdgeIDs[0], InverseRedundant);
    graphPtr->InsertEdge(index, newEdgeIDs[1], Inverse);
  } else if (oldState > Redundant && newState == Normal) {
    graphPtr->InsertEdge(index, newEdgeIDs[1], Normal);
    graphPtr->InsertEdge(index, newEdgeIDs[0], InverseRedundant);
  } else if (oldState > Redundant && newState == Inverse) {
    graphPtr->InsertEdge(index, newEdgeIDs[0], Inverse);
    graphPtr->InsertEdge(index, newEdgeIDs[1], NormalRedundant);
  } else if (oldState == newState) {
    graphPtr->InsertEdge(index, newEdgeIDs[0], InverseRedundant);
    graphPtr->InsertEdge(index, newEdgeIDs[1], NormalRedundant);
  }
}

// _____________________________________________________________________________
void ConnectionRepair::SeparateEdgeThree(Graph* graphPtr, const int& index,
                                         const int& newState) {
  // create new edges
  ID oldEdgeID = graphPtr->GetEdgeID(index);
  vector<ID> newEdgeIDs = CreateNewEdges(oldEdgeID, true);
  int oldState = graphPtr->GetEdgeState(index);

  bool order = oldState < newState;
  int separatePositionFront = order ? oldState : newState;
  int separatePositionBack = order ? newState : oldState;

  Edge* oldEdgePtr = nPtr_->GetEdgePtr(oldEdgeID);
  deque<ID>::iterator oldEdgeBegin = oldEdgePtr->GetVertexListBegin();
  deque<ID>::iterator oldEdgeEnd = oldEdgePtr->GetVertexListEnd();

  Edge* edgeFrontPtr = nPtr_->GetEdgePtr(newEdgeIDs[0]);
  edgeFrontPtr->InsertVertex(oldEdgeBegin,
                             oldEdgeBegin + separatePositionFront + 1);

  Edge* edgeMiddlePtr = nPtr_->GetEdgePtr(newEdgeIDs[1]);
  edgeMiddlePtr->InsertVertex(oldEdgeBegin + separatePositionFront,
                              oldEdgeBegin + separatePositionBack + 1);

  Edge* edgeBackPtr = nPtr_->GetEdgePtr(newEdgeIDs[2]);
  edgeBackPtr->InsertVertex(oldEdgeBegin + separatePositionBack, oldEdgeEnd);

  ID vertexIDFirst = edgeFrontPtr->GetFrom();
  Vertex* vertexFirstPtr = nPtr_->GetVertexPtr(vertexIDFirst);
  vertexFirstPtr->AddToAdjacenyList(edgeFrontPtr->GetID());

  ID vertexIDMiddleFirst = edgeMiddlePtr->GetFrom();
  Vertex* vertexMiddleFirstPtr = nPtr_->GetVertexPtr(vertexIDMiddleFirst);
  vertexMiddleFirstPtr->AddToAdjacenyList(edgeFrontPtr->GetID());
  vertexMiddleFirstPtr->AddToAdjacenyList(edgeMiddlePtr->GetID());

  ID vertexIDMiddleSecond = edgeMiddlePtr->GetTo();
  Vertex* vertexMiddleSecondPtr = nPtr_->GetVertexPtr(vertexIDMiddleSecond);
  vertexMiddleSecondPtr->AddToAdjacenyList(edgeMiddlePtr->GetID());
  vertexMiddleSecondPtr->AddToAdjacenyList(edgeBackPtr->GetID());

  ID vertexIDLast = edgeBackPtr->GetTo();
  Vertex* vertexLastPtr = nPtr_->GetVertexPtr(vertexIDLast);
  vertexLastPtr->AddToAdjacenyList(edgeBackPtr->GetID());

  graphPtr->DeleteEdge(index);
  // replace the oldedge by two new edges
  if (order) {
    graphPtr->InsertEdge(index, newEdgeIDs[2], NormalRedundant);
    graphPtr->InsertEdge(index, newEdgeIDs[1], Normal);
    graphPtr->InsertEdge(index, newEdgeIDs[0], InverseRedundant);
  } else {
    graphPtr->InsertEdge(index, newEdgeIDs[0], InverseRedundant);
    graphPtr->InsertEdge(index, newEdgeIDs[1], Inverse);
    graphPtr->InsertEdge(index, newEdgeIDs[2], NormalRedundant);
  }
}

// _____________________________________________________________________________
int ConnectionRepair::RepairFontEdge(Graph* graphPtr, const int& index,
                                    const int& conjointPos,
                                     const int& lastItemIndex) {
  int state = graphPtr->GetEdgeState(index);
  int increment(1);
  switch (state) {
    case Normal:
      if (conjointPos == 0) {
        graphPtr->ChangeEdgeState(index,
                                  !index ? Inverse : NormalRedundant);
      } else if (conjointPos < lastItemIndex) {
        SeparateEdge(graphPtr, index, conjointPos);
        increment++;
      }
      break;

    case Inverse:
      if (conjointPos == lastItemIndex) {
        graphPtr->ChangeEdgeState(index, InverseRedundant);
      } else if (conjointPos > 0) {
        SeparateEdge(graphPtr, index, conjointPos);
        increment++;
      }
      break;

    default:
      if (conjointPos == 0) {
        SeparateEdge(graphPtr, index, Inverse);
        increment++;
      } else if (conjointPos == lastItemIndex) {
        SeparateEdge(graphPtr, index, Normal);
        increment++;
      } else {
        if (state != conjointPos) {
          SeparateEdgeThree(graphPtr, index, conjointPos);
          increment += 2;
        } else {
          SeparateEdge(graphPtr, index, conjointPos);
          increment++;
        }
      }
      break;
  }
  return increment;
}

// _____________________________________________________________________________
int ConnectionRepair::RepairBackEdge(Graph* graphPtr, const int& index,
                                   const int& conjointPos,
                                   const int& lastItemIndex,
                                   const int& increment) {
  if (conjointPos == lastItemIndex) {
    graphPtr->ChangeEdgeState(index + increment, Inverse);
  } else if (conjointPos > 0) {
    graphPtr->ChangeEdgeState(index + increment, conjointPos);
  }
  return increment - 1;
}


// _____________________________________________________________________________
int ConnectionRepair::Repair(Graph* graphPtr, const int& index) {
  ID edgeIDFront = graphPtr->GetEdgeID(index);
  ID edgeIDBack = graphPtr->GetEdgeID(index + 1);
  Edge* edgeFrontPtr = nPtr_->GetEdgePtr(edgeIDFront);
  Edge* edgeBackPtr = nPtr_->GetEdgePtr(edgeIDBack);
  deque<ID> ndListFront = edgeFrontPtr->GetVertexList();
  deque<ID> ndListBack = edgeBackPtr->GetVertexList();

  bgi::rtree<pair<GeoCoordinate, int>, bgi::linear<16> > rtree;
  for (int j = 0; j < ndListBack.size(); j++) {
    Vertex* vertexPtr = nPtr_->GetVertexPtr(ndListBack[j]);
    rtree.insert(make_pair(vertexPtr->GetCoordinate(), j));
  }

  int front, back;
  double minDistance = EarthRadius;
  for (int i = static_cast<int>(ndListFront.size()) - 1; i >= 0 ; i--) {
    Vertex* vertexPtr = nPtr_->GetVertexPtr(ndListFront[i]);
    auto it = rtree.qbegin(bgi::nearest(vertexPtr->GetCoordinate(), 1));
    if (IsEqualVertices(ndListFront[i], ndListBack[it->second])) {
      int increment = RepairFontEdge(graphPtr, index, i,
                                     ndListFront.size() - 1);
      return RepairBackEdge(graphPtr, index, it->second, ndListBack.size() - 1,
                          increment);
    }

    double distance = CalculateDistance(vertexPtr->GetCoordinate(), it->first);
    if (distance == 0.0) {
      nPtr_->AddEqualVertex(ndListFront[i], ndListBack[it->second]);
      int increment = RepairFontEdge(graphPtr, index, i,
                                     ndListFront.size() - 1);
      return RepairBackEdge(graphPtr, index, it->second, ndListBack.size() - 1,
                            increment);
    }

    if (distance < minDistance) {
      minDistance = distance;
      front = i;
      back = it->second;
    }
  }

  ID vertexIDFront = ndListFront[front];
  ID vertexIDBack = ndListBack[back];
  Vertex* vertexFrontPtr = nPtr_->GetVertexPtr(vertexIDFront);
  Vertex* vertexBackPtr = nPtr_->GetVertexPtr(vertexIDBack);
  if (minDistance <= 1.5
      && (vertexFrontPtr->GetAdjacenyListSize()
          || vertexBackPtr->GetAdjacenyListSize())) {
    nPtr_->AddEqualVertex(vertexIDFront, vertexIDBack);
    int increment = RepairFontEdge(graphPtr, index, front,
                                   ndListFront.size() - 1);
    return RepairBackEdge(graphPtr, index, back, ndListBack.size() - 1,
                          increment);
  }
  return -1;
}

// _____________________________________________________________________________
void ConnectionRepair::Run() {
  for (auto it = nPtr_->GetGraphMapBegin();
       it != nPtr_->GetGraphMapEnd(); it++) {
    Graph* graphPtr = &it->second;
    if (graphPtr->GetEdgeListSize() < 2) continue;

    int index(0);
    while (index < static_cast<int>(graphPtr->GetEdgeListSize()) - 1) {
      int conjoint = Repair(graphPtr, index);
      if (conjoint > -1) {
        index += conjoint + 1;
        continue;
      }
      graphPtr->DeleteEdge(index + 1, true);
      break;
    }
    int lastIndex = static_cast<int>(graphPtr->GetEdgeListSize()) - 1;
    int lastState = graphPtr->GetEdgeState(lastIndex);
    if (lastState > 0) {
      SeparateEdge(graphPtr, lastIndex, Normal);
    }
  }
}
