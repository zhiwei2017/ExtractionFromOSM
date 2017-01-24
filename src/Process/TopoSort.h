// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_PROCESS_TOPOSORT_H_
#define SRC_PROCESS_TOPOSORT_H_

#include <gtest/gtest_prod.h>
#include <stack>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include "./Process.h"

using std::stack;
using std::unordered_map;
using std::unordered_set;
using std::deque;

/* this class is to store the edge list information from graph*/
class TopoGraph {
 public:
  /*
   * constructor
   *
   * @param graphPtr
   * @return
   */
  explicit TopoGraph(Graph* graphPtr);
  FRIEND_TEST(TopoGraphTest, TopoGraph);

  /*
   * add the adjacent edge-ID to first edge-ID's adjacency list
   *
   * @param edgeID
   * @param adjacentEdgeID
   * @return
   */
  void AddToAdjacencyList(const ID& edgeID, const ID& adjacentEdgeID);

  /*
   * get the adjacency list of the given edge-ID
   *
   * @param edgeID
   * @return adjacency list
   */
  unordered_set<ID> GetAdjacencyList(const ID& edgeID);

 private:
  // pointer of a graph
  Graph* graphPtr_;

  // adjacency lists' map
  unordered_map<ID, unordered_set<ID>> adjacencyList_;
};

/* topological sort class to get a topological sorted edge list*/
class TopoSort : public Process {
 public:
  /*
   * constructor
   *
   * @param nPtr
   * @return
   */
  explicit TopoSort(Network* nPtr);

  /*
   * update the state of the edge
   *
   * @param frontPtr
   * @param backPtr
   * @return
   */
  void UpdateEdgeState(TopoNode* frontPtr, TopoNode* backPtr,
                       bool firstPair);

  /*
   * do the sort for each graph
   *
   * @param topoGraph
   * @param graphPtr
   * @return
   */
  void Sorting(TopoGraph topoGraph, Graph* graphPtr);
  FRIEND_TEST(SortingTest, Sorting);

  /*
   * run the topological sort
   *
   * @param 
   * @return
   */
  void Run();
};

// Inline methods
// _____________________________________________________________________________
TopoGraph::TopoGraph(Graph* graphPtr) {
  graphPtr_ = graphPtr;
  deque<TopoNode> edgeList = graphPtr->GetEdgeList();
  // if state == 0 means this node is not discovered
  for (size_t i = 1; i < edgeList.size(); i++) {
    AddToAdjacencyList(edgeList[i].GetID(), edgeList[i - 1].GetID());
    AddToAdjacencyList(edgeList[i - 1].GetID(), edgeList[i].GetID());
    size_t j = i - 1;
    while (j > 0 && edgeList[j].GetState() < -2) {
      j--;
      if (edgeList[i].GetID() == edgeList[j].GetID()) continue;

      AddToAdjacencyList(edgeList[i].GetID(), edgeList[j].GetID());
      AddToAdjacencyList(edgeList[j].GetID(), edgeList[i].GetID());
    }
  }
}

// _____________________________________________________________________________
void TopoGraph::AddToAdjacencyList(const ID& edgeID, const ID& adjacentEdgeID) {
  if (adjacencyList_.count(edgeID)) {
    adjacencyList_.at(edgeID).emplace(adjacentEdgeID);
  } else {
    unordered_set<ID> adj({adjacentEdgeID});
    adjacencyList_.emplace(edgeID, adj);
  }
}

// _____________________________________________________________________________
unordered_set<ID> TopoGraph::GetAdjacencyList(const ID& edgeID) {
  return adjacencyList_[edgeID];
}

// _____________________________________________________________________________
TopoSort::TopoSort(Network* nPtr) {
  nPtr_ = nPtr;
}

// _____________________________________________________________________________
void TopoSort::Sorting(TopoGraph topoGraph, Graph* graphPtr) {
  ID start = graphPtr->GetEdgeID(0);
  deque<TopoNode> result;
  // dfs
  stack<ID> S;
  S.push(start);
  stack<size_t> fathers;
  fathers.push(0);
  unordered_set<ID> discovered;
  while (!S.empty()) {
    ID v = S.top();
    S.pop();
    size_t father = fathers.top();
    fathers.pop();
    if (!discovered.count(v)) {
      // update the state
      discovered.emplace(v);
      TopoNode topoNode(v, -2, father);
      result.push_back(topoNode);
      if (result.size() > 1) {
        TopoNode* front = &result[father];
        UpdateEdgeState(front, &result.back(), result.size() == 2);
      }

      size_t index = result.size() - 1;
      unordered_set<ID> adjacentNodes = topoGraph.GetAdjacencyList(v);
      for (auto it = adjacentNodes.begin(); it != adjacentNodes.end(); it++) {
        fathers.push(index);
        if (*it == v) continue;
        S.push(*it);
      }
    }
  }
  graphPtr->ClearEdgeList();
  graphPtr->PushBackEdge(result);
}

// _____________________________________________________________________________
void TopoSort::UpdateEdgeState(TopoNode* frontPtr, TopoNode* backPtr,
                               bool firstPair) {
  ID frontEdgeID = frontPtr->GetID();
  ID backEdgeID = backPtr->GetID();
  Edge* frontEdgePtr = nPtr_->GetEdgePtr(frontEdgeID);
  Edge* backEdgePtr = nPtr_->GetEdgePtr(backEdgeID);
  deque<ID> frontVertexList = frontEdgePtr->GetVertexList();
  deque<ID> backVertexList = backEdgePtr->GetVertexList();
  if (IsEqualVertices(frontVertexList.back(), backVertexList.back())) {
    backPtr->ChangeState(-1);
  } else if (IsEqualVertices(frontVertexList.front(), backVertexList.back())) {
    if (firstPair) frontPtr->ChangeState(-1);
    backPtr->ChangeState(-1);
  } else if (firstPair && IsEqualVertices(frontVertexList.front(),
                                          backVertexList.front())) {
    frontPtr->ChangeState(-1);
  }
}

// _____________________________________________________________________________
void TopoSort::Run() {
  for (auto it = nPtr_->GetGraphMapBegin();
       it != nPtr_->GetGraphMapEnd(); it++) {
    Graph* graphPtr = &it->second;
    if (!graphPtr->GetEdgeListSize()) continue;

    TopoGraph topoGraph(graphPtr);
    Sorting(topoGraph, graphPtr);
  }
}

#endif  // SRC_PROCESS_TOPOSORT_H_
