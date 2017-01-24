// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include "./Network.h"
#include <tuple>

using std::make_tuple;

// _____________________________________________________________________________
bool Network::ContainVertex(const ID& vertexId) const {
  return vertices_.count(vertexId);
}

// _____________________________________________________________________________
Vertex* Network::GetVertexPtr(const ID& vertexId) {
  return &vertices_.at(vertexId);
}

// _____________________________________________________________________________
VertexMapIt Network::GetVertexMapBegin() {
  return vertices_.begin();
}

// _____________________________________________________________________________
VertexMapIt Network::GetVertexMapEnd() {
  return vertices_.end();
}

// _____________________________________________________________________________
void Network::AddVertex(const ID& vertexId) {
  vertices_.emplace(vertexId, Vertex(vertexId));
}

// _____________________________________________________________________________
void Network::AddVertex(Vertex vertex) {
  vertices_.emplace(vertex.GetID(), vertex);
}

// _____________________________________________________________________________
bool Network::ContainEdge(const ID& edgeId) const {
  return edges_.count(edgeId);
}

// _____________________________________________________________________________
Edge* Network::GetEdgePtr(const ID& edgeId) {
  return &edges_.at(edgeId);
}

// _____________________________________________________________________________
void Network::AddEdge(const ID& edgeID) {
  edges_.emplace(edgeID, Edge(edgeID));
}

// _____________________________________________________________________________
void Network::AddEdge(Edge edge) {
  edges_.emplace(edge.GetID(), edge);
}

// _____________________________________________________________________________
EdgeMapIt Network::GetEdgeMapBegin() {
  return edges_.begin();
}

// _____________________________________________________________________________
EdgeMapIt Network::GetEdgeMapEnd() {
  return edges_.end();
}

// _____________________________________________________________________________
Graph* Network::GetGraphPtr(const ID& graphId) {
  return &graphs_.at(graphId);
}

// _____________________________________________________________________________
GraphMapIt Network::GetGraphMapBegin() {
  return graphs_.begin();
}

// _____________________________________________________________________________
GraphMapIt Network::GetGraphMapEnd() {
  return graphs_.end();
}

// _____________________________________________________________________________
void Network::AddGraph(Graph graph) {
  graphs_.emplace(graph.GetID(), graph);
}

// _____________________________________________________________________________
bool Network::ContainEqualVertex(const ID& vertexId1,
                                  const ID& vertexId2) const {
  Key equalVertices1 = make_tuple(vertexId1, vertexId2);
  Key equalVertices2 = make_tuple(vertexId2, vertexId1);
  return equalVertex_.count(equalVertices1)
  || equalVertex_.count(equalVertices2);
}

// _____________________________________________________________________________
void Network::AddEqualVertex(const ID& vertexId1, const ID& vertexId2) {
  equalVertex_.emplace(vertexId1, vertexId2);
}

// _____________________________________________________________________________
size_t Network::GetVertexMapSize() const {
  return vertices_.size();
}

// _____________________________________________________________________________
size_t Network::GetEdgeMapSize() const {
  return edges_.size();
}

// _____________________________________________________________________________
size_t Network::GetGraphMapSize() const {
  return graphs_.size();
}

// _____________________________________________________________________________
size_t Network::GetEqualVertexMapSize() const {
  return equalVertex_.size();
}

// _____________________________________________________________________________
GraphMapIt Network::DeleteGraph(GraphMapIt position) {
  return graphs_.erase(position);
}

// _____________________________________________________________________________
void Network::DeleteGraph(const ID& graphID) {
  graphs_.erase(graphID);
}

// _____________________________________________________________________________
LineString Network::GetLine(Edge* edgePtr) {
  LineString line;
  for (int i = 0; i < edgePtr->GetVertexListSize(); ++i) {
    ID vertexId = edgePtr->GetVertexID(i);
    Vertex* vertexPtr = GetVertexPtr(vertexId);
    line.push_back(vertexPtr->GetCoordinate());
  }
  return line;
}
