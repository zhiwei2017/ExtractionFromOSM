// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_GTFS_RAILWAY_H_
#define SRC_GTFS_RAILWAY_H_

#include <string>
#include <vector>
#include <utility>
#include "./Stop.h"
#include "../Process/TopoNode.h"

using std::string;
using std::to_string;
using std::vector;
using std::pair;
using std::make_pair;

typedef vector<string>::iterator StrListIte;
typedef vector<Stop*>::iterator StopPtrListIte;

/* a middle class to separate the information of a graph
 * into shape, trip and stop class
 */
class Railway : public BasicType<string> {
 public:
  /*
   * constructor
   *
   * @param routeID
   * @param trip
   * @param routeType
   * @return
   */
  Railway(const ID& routeID, vector<TopoNode*> trip, bool routeType);

  /*
   * get the edge-ID-list
   *
   * @param
   * @return edge-ID-list
   */
  vector<TopoNode*> GetEdges() const;

  /*
   * get the total count of the edges
   *
   * @param
   * @return size_t
   */
  size_t GetEdgesAmount() const;

  /*
   * add a stop to shape
   *
   * @param stopPtr
   * @return
   */
  void AddStop(Stop* stopPtr);

  /*
   * add a coordinate to latitude list and longitude list
   *
   * @param coord
   * @return
   */
  void AddCoordinate(const GeoCoordinate& coord);

  /*
   * return the begin and end iterators of the stop list
   *
   * @param
   * @return begin and end iterators
   */
  pair<StopPtrListIte, StopPtrListIte> GetStops();

  /*
   * get the total number of stops
   *
   * @param
   * @return number of stops
   */
  size_t GetStopsAmount() const;

  /*
   * get the latitude-list's begin and end iterators
   *
   * @param
   * @return begin and end iterators
   */
  pair<StrListIte, StrListIte> GetShapePtLats();

  /*
   * get the longitude-list's begin and end iterators
   *
   * @param
   * @return begin and end iterators
   */
  pair<StrListIte, StrListIte> GetShapePtLons();

  /*
   * get the sequence list's begin and end ierators
   *
   * @param
   * @return begin and end ierators
   */
  pair<StrListIte, StrListIte> GetShapePtSequence();

  /*
   * get the route type
   *
   * @param
   * @return false: local, 
   *         true: long_distance
   */
  bool GetRouteType() const;

 private:
  // edge-ID list
  vector<TopoNode*> edges_;

  // stop-list
  vector<Stop*> stops_;

  // latitude-list
  vector<string> shapePtLats_;

  // longitude-list
  vector<string> shapePtLons_;

  // sequence list
  vector<string> shapePtSequence_;

  // route type
  bool routeType_;
};

// Inline methods
// _____________________________________________________________________________
Railway::Railway(const ID& routeID, vector<TopoNode*> trip, bool routeType) {
  id_ = to_string(routeID);
  edges_ = trip;
  routeType_ = routeType;
}

// _____________________________________________________________________________
vector<TopoNode*> Railway::GetEdges() const {
  return edges_;
}

// _____________________________________________________________________________
size_t Railway::GetEdgesAmount() const {
  return edges_.size();
}

// _____________________________________________________________________________
void Railway::AddStop(Stop* stopPtr) {
  stops_.push_back(stopPtr);
}

// _____________________________________________________________________________
void Railway::AddCoordinate(const GeoCoordinate& coord) {
  shapePtLats_.push_back(to_string(coord.get<0>()));
  shapePtLons_.push_back(to_string(coord.get<1>()));
  shapePtSequence_.push_back(to_string(shapePtSequence_.size()));
}

// _____________________________________________________________________________
pair<StopPtrListIte, StopPtrListIte> Railway::GetStops() {
  return make_pair(stops_.begin(), stops_.end());
}

// _____________________________________________________________________________
size_t Railway::GetStopsAmount() const {
  return stops_.size();
}

// _____________________________________________________________________________
pair<StrListIte, StrListIte> Railway::GetShapePtLats() {
  return make_pair(shapePtLats_.begin(), shapePtLats_.end());
}

// _____________________________________________________________________________
pair<StrListIte, StrListIte> Railway::GetShapePtLons() {
  return make_pair(shapePtLons_.begin(), shapePtLons_.end());
}

// _____________________________________________________________________________
pair<StrListIte, StrListIte> Railway::GetShapePtSequence() {
  return make_pair(shapePtSequence_.begin(), shapePtSequence_.end());
}

// _____________________________________________________________________________
bool Railway::GetRouteType() const {
  return routeType_;
}

#endif  // SRC_GTFS_RAILWAY_H_
