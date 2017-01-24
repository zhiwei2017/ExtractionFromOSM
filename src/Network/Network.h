// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_NETWORK_NETWORK_H_
#define SRC_NETWORK_NETWORK_H_

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <tuple>
#include "./Vertex.h"
#include "./Edge.h"
#include "./Graph.h"

typedef unordered_map<ID, Vertex>::iterator VertexMapIt;
typedef unordered_map<ID, Edge>::iterator EdgeMapIt;
typedef unordered_map<ID, Graph>::iterator GraphMapIt;

using std::unordered_map;
using std::unordered_set;
using std::tuple;

// key type for storing two related vertex-IDs in an unordered map
typedef tuple<ID, ID> Key;

// hash function for the new key type
struct KeyHash : public std::unary_function<Key, std::size_t> {
  std::size_t operator()(const Key& k) const {
    return std::get<0>(k) ^ std::get<1>(k);
  }
};

// equal function for the new key type
struct KeyEqual : public std::binary_function<Key, Key, bool> {
  bool operator()(const Key& v0, const Key& v1) const {
    return (std::get<0>(v0) == std::get<0>(v1) &&
            std::get<1>(v0) == std::get<1>(v1));
  }
};

class Network {
 public:
  /*
   * check if the vertex-map contains a given vertex-id
   *
   * @param vertexId
   * @return bool
   */
  bool ContainVertex(const ID& vertexId) const;

  /*
   * get the correponding vertex-pointer from the vertex-map with a given id
   *
   * @param vertexId
   * @return Vertex* : pointer of the vertex
   */
  Vertex* GetVertexPtr(const ID& vertexId);

  /*
   * get the begin iterator of vertex-map
   *
   * @param
   * @return begin iterator
   */
  VertexMapIt GetVertexMapBegin();

  /*
   * get the end iterator of vertex-map
   *
   * @param
   * @return end iterator
   */
  VertexMapIt GetVertexMapEnd();

  /*
   * add a vertex to vertex-map
   *
   * @param vertexId
   * @return
   */
  void AddVertex(const ID& vertexId);

  /*
   * add vertex to vertex-map
   *
   * @param vertex
   * @return
   */
  void AddVertex(Vertex vertex);

  /*
   * check if edge-map contains an edge with given id
   *
   * @param edgeId
   * @return bool
   */
  bool ContainEdge(const ID& edgeId) const;

  /*
   * get the edge-pointer from edge-map with given id
   *
   * @param edgeId
   * @return Edge* : pointer to the edge
   */
  Edge* GetEdgePtr(const ID& edgeId);

  /*
   * add an edge with given id to edge-map
   *
   * @param edgeID
   * @return
   */
  void AddEdge(const ID& edgeID);

  /*
   * add an edge to edge-map
   *
   * @param edge
   * @return
   */
  void AddEdge(Edge edge);

  /*
   * get the begin iterator of edge-map
   *
   * @param
   * @return begin iterator
   */
  EdgeMapIt GetEdgeMapBegin();

  /*
   * get the end iterator of edge-map
   *
   * @param
   * @return end iterator
   */
  EdgeMapIt GetEdgeMapEnd();

  /*
   * get a graph-pointer from the graph-map with given id
   *
   * @param graphId
   * @return Graph* : pointer of a graph
   */
  Graph* GetGraphPtr(const ID& graphId);

  /*
   * get the graph-map's begin iterator
   *
   * @param
   * @return begin iterator
   */
  GraphMapIt GetGraphMapBegin();

  /*
   * get the graph-map's end iterator
   *
   * @param
   * @return end iterator
   */
  GraphMapIt GetGraphMapEnd();

  /*
   * add the graph to graph-map
   *
   * @param graph
   * @return
   */
  void AddGraph(Graph graph);

  /*
   * check if the equal-vertex-map contain a pair of equal vertices
   *
   * @param vertexId1
   * @param vertexId2
   * @return bool
   */
  bool ContainEqualVertex(const ID& vertexId1, const ID& vertexId2) const;

  /*
   * add the pair of equal vertices into the equal-vertex-map
   *
   * @param vertexId1
   * @param vertexId2
   * @return
   */
  void AddEqualVertex(const ID& vertexId1, const ID& vertexId2);

  /*
   * get the size of vertex-map
   *
   * @param
   * @return size of vertex-map
   */
  size_t GetVertexMapSize() const;

  /*
   * get the size of edge-map
   *
   * @param
   * @return size of edge-map
   */
  size_t GetEdgeMapSize() const;

  /*
   * get the size of graph-map
   *
   * @param
   * @return size of graph-map
   */
  size_t GetGraphMapSize() const;

  /*
   * get the size of equal-vertex-map
   *
   * @param
   * @return size of equal-vertices-map
   */
  size_t GetEqualVertexMapSize() const;

  /*
   * delete a record in graph-map at given position
   *
   * @param position: iterator
   * @return the position of next record
   */
  GraphMapIt DeleteGraph(GraphMapIt position);

  /*
   * delete a record in graph-map with given key
   *
   * @param graphID: key
   * @return
   */
  void DeleteGraph(const ID& graphID);

  /*
   * get the linestring of a given edge-pointer
   *
   * @param edge: edge-pointer
   * @return
   */
  LineString GetLine(Edge* edgePtr);

 private:
  unordered_map<ID, Vertex> vertices_;
  unordered_map<ID, Edge> edges_;
  unordered_map<ID, Graph> graphs_;
  unordered_set<Key, KeyHash, KeyEqual> equalVertex_;
};

#endif  // SRC_NETWORK_NETWORK_H_
