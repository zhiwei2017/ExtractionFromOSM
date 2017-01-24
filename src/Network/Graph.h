// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_NETWORK_GRAPH_H_
#define SRC_NETWORK_GRAPH_H_

#include <deque>
#include "../OSMBasic.h"
#include "../Process/TopoNode.h"

using std::deque;

typedef deque<TopoNode>::iterator EdgeListIte;

class Graph : public Attribute, public BasicType<ID> {
 public:
  /*
   * constructor
   *
   * @param id : a relation-ID in the osm file
   * @return
   */
  explicit Graph(const ID& id);

  /*
   * delete edge-ID at the given position
   * can also delete all edge-IDs behind the given position
   *
   * @param pos : position
   * @param toEnd : if delete all the IDs behind the given position
   * @return an iterator of next position
   */
  EdgeListIte DeleteEdge(const int& index, bool toEnd = false);

  /*
   * delete all the edges' IDs
   *
   * @param
   * @return
   */
  void ClearEdgeList();

  /*
   * add the given edge-ID to the end of edgeList
   *
   * @param edgeID : edge-ID
   * @param state : the state of this edge
   * @return
   */
  void PushBackEdge(const ID& edgeID, const int& state = -2);

  /*
   * add the given edge-ID list to the end of edgeList
   *
   * @param edgeIDList : an edge-ID list
   * @return
   */
  void PushBackEdge(deque<ID> edgeIDList);

  /*
   * add the given edge-ID list to the end of edgeList
   *
   * @param edgeList : an edge-ID list
   * @return
   */
  void PushBackEdge(deque<TopoNode>  edgeList);

  /*
   * insert the edge-ID at the given position
   *
   * @param pos : position
   * @param edgeID : edge-ID
   * @param state : the state of this edge
   * @return
   */
  void InsertEdge(const int& pos, const ID& edgeID, const int& state = -2);

  /*
   * add the given edge-ID list to the given position in edgeList
   *
   * @param pos : position
   * @param edgeIDList : an edge-ID list
   * @return
   */
  void InsertEdge(const int& pos, deque<ID> edgeIDList);

  /*
   * add the given edge-ID list to the given position in edgeList
   *
   * @param pos : position
   * @param edgeIDList : an edge-ID list
   * @return
   */
  void InsertEdge(const int& pos, deque<TopoNode>  edgeList);

  /*
   * get an edge-id with given index
   *
   * @param index : index
   * @return edgeID
   */
  ID GetEdgeID(const int& index) const;

  /*
   * get an edge-state at the given index
   *
   * @param index : position
   * @return state
   */
  int GetEdgeState(const int& index) const;

  /*
   * change the edge-state at the given index
   *
   * @param index : position
   * @param state : new state
   * @return
   */
  void ChangeEdgeState(const int& index, const int& state);

  /*
   * get the size of edgeList
   *
   * @param
   * @return size of the edgeList
   */
  size_t GetEdgeListSize() const;

  /*
   * get the reference of edgeList
   *
   * @param
   * @return reference of edgeList
   */
  deque<TopoNode>& GetEdgeList();

  /*
   * get the pointer of a record in edgeList
   *
   * @param
   * @return pointer
   */
  TopoNode* GetTopoNodePtr(const int& index);

  /*
   * delete the given position vertex-ID from the vertexList
   *
   * @param pos : position
   * @return the iterator of next position
   */
  deque<ID>::iterator DeleteVertex(const int& pos);

  /*
   * add the vertex-ID to the end of vertexList
   *
   * @param vertexID
   * @return
   */
  void PushBackVertex(const ID& vertexID);

  /*
   * add a list of vertex-IDs to the end of vertexList
   *
   * @param vertexIDList
   * @return
   */
  void PushBackVertex(deque<ID> vertexIDList);

  /*
   * insert a vertex-ID at given position
   *
   * @param pos : position
   * @param vertexIDList
   * @return
   */
  void InsertVertex(const int& pos, const ID& vertexID);

  /*
   * insert a list of vertex-ID at given position
   *
   * @param pos : position
   * @param vertexIDList
   * @return
   */
  void InsertVertex(const int& pos, deque<ID> vertexIDList);

  /*
   * get number of deleted edge-IDs
   *
   * @param
   * @return the number of deleted edge-IDs
   */
  int GetDeletedEdgeNum();

 private:
  // store the station vertice' IDs
  deque<ID> vertexList_;

  // store the edges' IDs
  deque<TopoNode> edgeList_;

  // count how many edge-IDs are deleted from the graph
  int deletedEdgeNum_;
};

// Inline methods
// _____________________________________________________________________________
Graph::Graph(const ID& id) {
  id_ = id;
  deletedEdgeNum_ = 0;
}

// _____________________________________________________________________________
EdgeListIte Graph::DeleteEdge(const int& index, bool toEnd) {
  if (toEnd) {
    return edgeList_.erase(edgeList_.begin() + index, edgeList_.end());
  }
  ++deletedEdgeNum_;
  return edgeList_.erase(edgeList_.begin() + index);
}

// _____________________________________________________________________________
void Graph::ClearEdgeList() {
  edgeList_.clear();
}

// _____________________________________________________________________________
void Graph::PushBackEdge(const ID& edgeID, const int& state) {
  edgeList_.emplace_back(edgeID, state);
}

// _____________________________________________________________________________
void Graph::PushBackEdge(deque<ID> edgeIDList) {
  for (auto& edgeID : edgeIDList) {
    edgeList_.emplace_back(edgeID, -2);
  }
}

// _____________________________________________________________________________
void Graph::PushBackEdge(deque<TopoNode>  edgeList) {
  edgeList_.insert(edgeList_.end(), edgeList.begin(), edgeList.end());
}

// _____________________________________________________________________________
void Graph::InsertEdge(const int& pos, const ID& edgeID, const int& state) {
  edgeList_.emplace(edgeList_.begin() + pos, edgeID, state);
}

// _____________________________________________________________________________
void Graph::InsertEdge(const int& pos, deque<ID> edgeIDList) {
  for (auto& edgeId : edgeIDList) {
    edgeList_.insert(edgeList_.begin() + pos, TopoNode(edgeId, -2));
  }
}

// _____________________________________________________________________________
void Graph::InsertEdge(const int& pos, deque<TopoNode>  edgeList) {
  edgeList_.insert(edgeList_.begin() + pos, edgeList.begin(), edgeList.end());
}

// _____________________________________________________________________________
ID Graph::GetEdgeID(const int& index) const {
  return (edgeList_.begin() + index)->GetID();
}

// _____________________________________________________________________________
int Graph::GetEdgeState(const int& index) const {
  return (edgeList_.begin() + index)->GetState();
}

// _____________________________________________________________________________
void Graph::ChangeEdgeState(const int& index, const int& state) {
  (edgeList_.begin() + index)->ChangeState(state);
}

// _____________________________________________________________________________
size_t Graph::GetEdgeListSize() const {
  return edgeList_.size();
}

// _____________________________________________________________________________
deque<TopoNode>& Graph::GetEdgeList() {
  return edgeList_;
}

// _____________________________________________________________________________
TopoNode* Graph::GetTopoNodePtr(const int& index) {
  return &edgeList_[index];
}

// _____________________________________________________________________________
deque<ID>::iterator Graph::DeleteVertex(const int& pos) {
  return vertexList_.erase(vertexList_.begin() + pos);
}

// _____________________________________________________________________________
void Graph::PushBackVertex(const ID& vertexID) {
  vertexList_.push_back(vertexID);
}

// _____________________________________________________________________________
void Graph::PushBackVertex(deque<ID> vertexIDList) {
  vertexList_.insert(vertexList_.end(),
                     vertexIDList.begin(), vertexIDList.end());
}

// _____________________________________________________________________________
void Graph::InsertVertex(const int& pos, const ID& vertexID) {
  vertexList_.insert(vertexList_.begin() + pos, vertexID);
}

// _____________________________________________________________________________
void Graph::InsertVertex(const int& pos, deque<ID> vertexIDList) {
  vertexList_.insert(vertexList_.begin() + pos, vertexIDList.begin(),
                   vertexIDList.end());
}

// _____________________________________________________________________________
int Graph::GetDeletedEdgeNum() {
  return deletedEdgeNum_;
}

#endif  // SRC_NETWORK_GRAPH_H_
