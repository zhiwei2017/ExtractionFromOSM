// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_GTFS_STOP_H_
#define SRC_GTFS_STOP_H_

#include <string>
#include "../Network/Vertex.h"
#include "../OSMBasic.h"

using std::string;
using std::to_string;

/* this class corresponds to the stops.txt file in GTFS */
class Stop : public BasicType<string>, public Functions {
 public:
  /*
   * constructor
   *
   * @param vertexPtr
   * @return
   */
  explicit Stop(Vertex* vertexPtr);

  /*
   * constructor
   *
   * @param vertexPtr
   * @param uicRef: parent station id
   * @return
   */
  Stop(Vertex* vertexPtr, const string& uicRef);

  /*
   * add a parent station to the current stop
   *
   * @param uicRef
   * @return
   */
  void AddParentStation(const string& uicRef);

  /*
   * add the child stop's coordinate
   *
   * @param coordinate
   * @return
   */
  void AddChildCoordinate(const GeoCoordinate& coordinate);

  /*
   * get the name of stop
   *
   * @param
   * @return name
   */
  string GetName() const;

  /*
   * get the latitude
   *
   * @param
   * @return latitude
   */
  string GetLat() const;

  /*
   * get the longitude
   *
   * @param
   * @return longitude
   */
  string GetLon() const;

  /*
   * get the coordinate
   *
   * @param
   * @return coordinate
   */
  GeoCoordinate GetCoordinate() const;

  /*
   * get the location type of this stop
   *
   * @param
   * @return value of location type
   */
  string GetLocationType() const;

  /*
   * get the parent station's id
   *
   * @param
   * @return id of parent station 
   */
  string GetParentStation() const;

 private:
  // name of the stop
  string name_;

  // latitude of the stop
  string lat_;

  // longitude of the stop
  string lon_;

  // indicate whether the stop has a parent stop or not
  // it could only be 0 or 1
  // 0: is a normal stop or a child stop
  // 1: is a parent stop
  string locationType_;

  // parent stop's id, i.e. uic_ref
  string parentStation_;

  // count the number of children stops
  int children_;
};

// Inline methods
// _____________________________________________________________________________
Stop::Stop(Vertex* vertexPtr) {
  id_ = to_string(vertexPtr->GetID());
  lat_ = to_string(vertexPtr->GetCoordinate().get<0>());
  lon_ = to_string(vertexPtr->GetCoordinate().get<1>());
  if (vertexPtr->ContainAttribute("uic_name")) {
    name_ = vertexPtr->GetAttribute("uic_name");
  } else if (vertexPtr->ContainAttribute("name")) {
    name_ = vertexPtr->GetAttribute("name");
  } else {
    name_ = "TrainStation";
  }
  locationType_ = "0";
  children_ = 0;
}

// _____________________________________________________________________________
Stop::Stop(Vertex* vertexPtr, const string& uicRef) {
  id_ = uicRef;
  lat_ = to_string(vertexPtr->GetCoordinate().get<0>());
  lon_ = to_string(vertexPtr->GetCoordinate().get<1>());
  if (vertexPtr->ContainAttribute("uic_name")) {
    name_ = vertexPtr->GetAttribute("uic_name");
  } else if (vertexPtr->ContainAttribute("name")) {
    name_ = vertexPtr->GetAttribute("name");
  } else {
    name_ = "TrainStation";
  }
  locationType_ = "1";
  children_ = 1;
}

// _____________________________________________________________________________
void Stop::AddParentStation(const string& uicRef) {
  parentStation_ = uicRef;
}

// _____________________________________________________________________________
void Stop::AddChildCoordinate(const GeoCoordinate& coordinate) {
  double lat = ConvertToDouble(lat_);
  double lon = ConvertToDouble(lon_);
  lat = (lat * children_ + coordinate.get<0>()) / (children_ + 1);
  lon = (lon * children_ + coordinate.get<1>()) / (children_ + 1);
  children_++;
  lat_ = to_string(lat);
  lon_ = to_string(lon);
}

// _____________________________________________________________________________
string Stop::GetName() const {
  return name_;
}

// _____________________________________________________________________________
string Stop::GetLat() const {
  return lat_;
}

// _____________________________________________________________________________
string Stop::GetLon() const {
  return lon_;
}

// _____________________________________________________________________________
GeoCoordinate Stop::GetCoordinate() const {
  double lat = ConvertToDouble(lat_);
  double lon = ConvertToDouble(lon_);
  return GeoCoordinate(lat, lon);
}

// _____________________________________________________________________________
string Stop::GetLocationType() const {
  return locationType_;
}

// _____________________________________________________________________________
string Stop::GetParentStation() const {
  return parentStation_;
}

#endif  // SRC_GTFS_STOP_H_
