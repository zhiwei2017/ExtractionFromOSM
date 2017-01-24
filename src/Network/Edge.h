// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_NETWORK_EDGE_H_
#define SRC_NETWORK_EDGE_H_

#include <deque>
#include "../OSMBasic.h"

using std::deque;

class Edge : public BasicType<ID> {
 public:
  /*
   * constructor
   *
   * @param id : the way's id in osm file
   * @return
   */
  explicit Edge(const ID& id);

  /*
   * get its first vertex's ID
   *
   * @param
   * @return vertex's ID
   */
  ID GetFrom() const;

  /*
   * get its last vertex's ID
   *
   * @param
   * @return vertex's ID
   */
  ID GetTo() const;

  /*
   * get the vertex's ID with given index
   *
   * @param index
   * @return vertex's ID
   */
  ID GetVertexID(const size_t& index) const;

  /*
   * get the list of vertices
   *
   * @param
   * @return list of vertices
   */
  deque<ID> GetVertexList();

  /*
   * get the size of the list of vertices
   *
   * @param
   * @return size of the list of vertices
   */
  size_t GetVertexListSize() const;

  /*
   * get the begin iterator of the vertices' list
   *
   * @param
   * @return begin iterator
   */
  deque<ID>::iterator GetVertexListBegin();

  /*
   * get end iterator of the vertices' list
   *
   * @param
   * @return end iterator
   */
  deque<ID>::iterator GetVertexListEnd();

  /*
   * add the vertex ID to the end of the vertices' list
   *
   * @param vertexID
   * @return
   */
  void PushBackVertex(const ID& vertexID);

  /*
   * add vertices to the end of the vertices' list
   *
   * @param pos1
   * @param pos2
   * @return
   */
  void InsertVertex(deque<ID>::iterator pos1, deque<ID>::iterator pos2);

  /*
   * add the vertices to the end of the vertices' list
   *
   * @param vertices
   * @return
   */
  void InsertVertex(deque<ID> vertices);

 private:
  // store the vertices' IDs
  deque<ID> vertices_;
};

// Inline methods
// _____________________________________________________________________________
Edge::Edge(const ID& id) {
  id_ =  id;
}

// _____________________________________________________________________________
ID Edge::GetFrom() const {
  return vertices_.front();
}

// _____________________________________________________________________________
ID Edge::GetTo() const {
  return vertices_.back();
}

// _____________________________________________________________________________
ID Edge::GetVertexID(const size_t& index) const {
  return vertices_[index];
}

// _____________________________________________________________________________
deque<ID> Edge::GetVertexList() {
  return vertices_;
}

// _____________________________________________________________________________
size_t Edge::GetVertexListSize() const {
  return vertices_.size();
}

// _____________________________________________________________________________
deque<ID>::iterator Edge::GetVertexListBegin() {
  return vertices_.begin();
}

// _____________________________________________________________________________
deque<ID>::iterator Edge::GetVertexListEnd() {
  return vertices_.end();
}

// _____________________________________________________________________________
void Edge::PushBackVertex(const ID& vertexID) {
  vertices_.push_back(vertexID);
}

// _____________________________________________________________________________
void Edge::InsertVertex(deque<ID>::iterator pos1, deque<ID>::iterator pos2) {
  vertices_.assign(pos1, pos2);
}

// _____________________________________________________________________________
void Edge::InsertVertex(deque<ID> vertices) {
  vertices_.assign(vertices.begin(), vertices.end());
}

#endif  // SRC_NETWORK_EDGE_H_
