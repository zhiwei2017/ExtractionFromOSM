// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_NETWORK_VERTEX_H_
#define SRC_NETWORK_VERTEX_H_

#include <unordered_set>
#include "../OSMBasic.h"

using std::unordered_set;

class Vertex : public Attribute, public BasicType<ID> {
 public:
  /*
   * constructor
   *
   * @param id : the node id in osm file
   * @return
   */
  explicit Vertex(const ID& id);

  /*
   * constructor
   *
   * @param id : the node id in osm file
   * @param lat : its latitude
   * @param lon : its longitude
   * @return
   */
  Vertex(const ID& id, const double& lat, const double& lon);

  /*
   * constructor
   *
   * @param id : the node's id in osm file
   * @param coord : its coordinate
   * @return
   */
  Vertex(const ID& id, const GeoCoordinate& coord);

  /*
   * set the geometric coordinate
   *
   * @param lat : latitude
   * @param lon : longitude
   * @return
   */
  void SetCoord(const double& lat, const double& lon);

  /*
   * get the geometric coordinate of the vertex
   *
   * @param
   * @return coordinate
   */
  GeoCoordinate GetCoordinate() const;

  /*
   * add the way id to the AdjacenyList
   *
   * @param wayID : wayID
   * @return
   */
  void AddToAdjacenyList(const ID& wayID);

  /*
   * get the size of AdjacenyList
   *
   * @param
   * @return size of AdjacenyList
   */
  size_t GetAdjacenyListSize() const;

 private:
  // store the coordinate
  GeoCoordinate coordinate_;

  // store the edges that go out from this node
  unordered_set<ID> adjacenyList_;
};

// Inline methods
// _____________________________________________________________________________
Vertex::Vertex(const ID& id) {
  id_ = id;
}

// _____________________________________________________________________________
Vertex::Vertex(const ID& id, const double& lat, const double& lon) {
  id_ = id;
  coordinate_ = GeoCoordinate(lat, lon);
}

// _____________________________________________________________________________
Vertex::Vertex(const ID& id, const GeoCoordinate& coord) {
  id_ = id;
  coordinate_ = coord;
}

// _____________________________________________________________________________
void Vertex::SetCoord(const double& lat, const double& lon) {
  coordinate_ = GeoCoordinate(lat, lon);
}

// _____________________________________________________________________________
GeoCoordinate Vertex::GetCoordinate() const {
  return coordinate_;
}

// _____________________________________________________________________________
void Vertex::AddToAdjacenyList(const ID& wayID) {
  adjacenyList_.emplace(wayID);
}

// _____________________________________________________________________________
size_t Vertex::GetAdjacenyListSize() const {
  return adjacenyList_.size();
}

#endif  // SRC_NETWORK_VERTEX_H_
