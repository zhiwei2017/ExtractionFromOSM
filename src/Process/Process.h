// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#ifndef SRC_PROCESS_PROCESS_H_
#define SRC_PROCESS_PROCESS_H_

#include <gtest/gtest_prod.h>
#include <boost/heap/fibonacci_heap.hpp>
#include <unordered_map>
#include <utility>
#include <vector>
#include <deque>
#include "../Network/Network.h"

namespace bh = boost::heap;

using std::deque;
using std::pair;
using std::make_pair;
using std::vector;

/*
 * Mega node used for path finding algorithm
 */
class MegaNode : public BasicType<ID> {
 public:
  /*
   * constructor
   *
   * @param edgeID
   * @param line: linestring of this edge
   * @return
   */
  MegaNode(const ID& edgeID, const LineString& line);

  /*
   * get the geographical line
   *
   * @param
   * @return LineString
   */
  LineString GetLine() const;

  /*
   * Get the priority of this MegaNode
   *
   * @param
   * @return value of priority_
   */
  double GetPriority() const;

  /*
   * set the priority with the given value
   *
   * @param priority
   * @return
   */
  void SetPriority(const double& priority);

 private:
  LineString line_;
  double priority_;
};

// compare function for heap
struct CompareMegaNode {
  bool operator()(const MegaNode& n1, const MegaNode& n2) const {
    return n1.GetPriority() < n2.GetPriority();
  }
};

typedef pair<Box, MegaNode> Value;
typedef bh::fibonacci_heap<MegaNode, bh::compare<CompareMegaNode>> Heap;

/* this class is mainly responsible for processing the network */
class Process : public Functions {
 public:
  /*
   * create a bounding box for given LineString
   *
   * @param line
   * @return Box
   */
  Box BoundingBox(const LineString& line) const;

  /*
   * check if the given two vertex-IDs are identical or considered
   * as equal
   *
   * @param line
   * @return Box
   */
  bool IsEqualVertices(const ID& vertexId1, const ID& vertexId2) const;

  /*
   * Check if the given two edges are conjoint
   *
   * @param edgeFront
   * @param edgeBack
   * @return true: if they are conjoint
   */
  bool AreConjoint(Edge* frontEdgePtr, Edge* backEdgePtr);
  FRIEND_TEST(AreConjointTest, AreConjoint);

  /*
   * run the process system
   *
   * @param
   * @return
   */
  virtual void Run() = 0;

 protected:
  Network* nPtr_;

  // r tree for fixing the gap
  bgi::rtree<Value, bgi::linear<16> > geometryRTree_;
};

// Inline methods
// _____________________________________________________________________________
MegaNode::MegaNode(const ID& edgeID, const LineString& line) {
  id_ = edgeID;
  line_ = line;
  priority_ = 0;
}

// _____________________________________________________________________________
LineString MegaNode::GetLine() const {
  return line_;
}

// _____________________________________________________________________________
double MegaNode::GetPriority() const {
  return priority_;
}

// _____________________________________________________________________________
void MegaNode::SetPriority(const double& priority) {
  priority_ = priority;
}

// _____________________________________________________________________________
Box Process::BoundingBox(const LineString& line) const {
  return bg::return_envelope<Box>(line);
}

// _____________________________________________________________________________
bool Process::IsEqualVertices(const ID& vertexId1, const ID& vertexId2) const {
  return vertexId1 == vertexId2
         || nPtr_->ContainEqualVertex(vertexId1, vertexId2);
}

// _____________________________________________________________________________
bool Process::AreConjoint(Edge* frontEdgePtr, Edge* backEdgePtr) {
  deque<ID> frontVertexList = frontEdgePtr->GetVertexList();
  deque<ID> backVertexList = backEdgePtr->GetVertexList();

  double boundary = 1.5;

  if (CalculateDistance(nPtr_->GetLine(frontEdgePtr),
                        nPtr_->GetLine(backEdgePtr)) > boundary) {
    return false;
  }

  bgi::rtree<pair<GeoCoordinate, int>, bgi::linear<16> > rtree;
  for (int j = 0; j < backVertexList.size(); j++) {
    Vertex* vertexPtr = nPtr_->GetVertexPtr(backVertexList[j]);
    rtree.insert(make_pair(vertexPtr->GetCoordinate(), j));
  }

  int front, back;
  double minDistance = EarthRadius;
  for (int i = static_cast<int>(frontVertexList.size()) - 1; i >= 0 ; i--) {
    Vertex* vertexPtr = nPtr_->GetVertexPtr(frontVertexList[i]);
    auto it = rtree.qbegin(bgi::nearest(vertexPtr->GetCoordinate(), 1));
    if (IsEqualVertices(frontVertexList[i], backVertexList[it->second]))
      return true;

    double distance = CalculateDistance(vertexPtr->GetCoordinate(), it->first);
    if (distance == 0.0) {
      nPtr_->AddEqualVertex(frontVertexList[i], backVertexList[it->second]);
      return true;
    }

    if (distance < minDistance) {
      minDistance = distance;
      front = i;
      back = it->second;
    }
  }

  ID frontVertexID = frontVertexList[front];
  ID backVertexID = backVertexList[back];
  Vertex* frontVertexPtr = nPtr_->GetVertexPtr(frontVertexID);
  Vertex* backVertexPtr = nPtr_->GetVertexPtr(backVertexID);
  if (minDistance <= boundary
      && (frontVertexPtr->GetAdjacenyListSize()
          || backVertexPtr->GetAdjacenyListSize())) {
    nPtr_->AddEqualVertex(frontVertexID, backVertexID);
    return true;
  }
  return false;
}

#endif  // SRC_PROCESS_PROCESS_H_
