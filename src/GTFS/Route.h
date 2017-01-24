// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_GTFS_ROUTE_H_
#define SRC_GTFS_ROUTE_H_

#include <string>
#include "../Network/Graph.h"

using std::string;
using std::to_string;

/* this class corresponds to the routes.txt file in GTFS */
class Route : public BasicType<string> {
 public:
  /*
   * constructor
   *
   * @param gPtr
   * @param agencyID
   * @return
   */
  explicit Route(Graph* gPtr, const string& agencyID = "6866");

  /*
   * get the short name
   *
   * @param
   * @return short name
   */
  string GetShortName() const;

  /*
   * get the name
   *
   * @param
   * @return name
   */
  string GetName() const;

  /*
   * change the name
   *
   * @param new name
   * @return
   */
  void ChangeName(const string& name);

  /*
   * get the type of this route
   *
   * @param
   * @return route's type
   */
  string GetRouteType() const;

  /*
   * get the agency id
   *
   * @param
   * @return agency id
   */
  string GetAgencyID() const;

  /*
   * get the route's color
   *
   * @param
   * @return route color
   */
  string GetRouteColor() const;

 private:
  // short name
  string shortName_;

  // name
  string name_;

  // route type: local or long_distance
  string routeType_;

  // agency id
  string agencyID_;

  // color of route
  string routeColor_;
};

// Inline methods
// _____________________________________________________________________________
Route::Route(Graph* gPtr, const string& agencyID) {
  id_ = to_string(gPtr->GetID());
  if (gPtr->ContainAttribute("ref")) {
    shortName_ = gPtr->GetAttribute("ref");
  } else {
    shortName_ = "Train";
  }
  name_ = gPtr->GetAttribute("name");
  if (name_ == ""
      && gPtr->ContainAttribute("from")
      && gPtr->ContainAttribute("to")) {
    name_ = gPtr->GetAttribute("from") + " - "
            + gPtr->GetAttribute("to");
  }
  routeType_ = "2";
  agencyID_ = agencyID;

  string trainType = gPtr->GetAttribute("service");
  if (LongDistanceKeywords.count(trainType)) {
    routeColor_ = "ff0000";
  } else {
    routeColor_ = "808080";
  }
}

// _____________________________________________________________________________
string Route::GetShortName() const {
  return shortName_;
}

// _____________________________________________________________________________
string Route::GetName() const {
  return name_;
}

// _____________________________________________________________________________
void Route::ChangeName(const string& name) {
  name_ = name;
}

// _____________________________________________________________________________
string Route::GetRouteType() const {
  return routeType_;
}

// _____________________________________________________________________________
string Route::GetAgencyID() const {
  return agencyID_;
}

// _____________________________________________________________________________
string Route::GetRouteColor() const {
  return routeColor_;
}

#endif  // SRC_GTFS_ROUTE_H_
