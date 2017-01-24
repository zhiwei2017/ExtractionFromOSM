// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_GTFS_SHAPE_H_
#define SRC_GTFS_SHAPE_H_

#include <string>
#include <vector>
#include <utility>
#include "./Railway.h"

using std::string;
using std::to_string;
using std::vector;

/* this class corresponds to the shapes.txt file in GTFS */
class Shape : public BasicType<string> {
 public:
  /*
   * constructor
   *
   * @param railway
   * @param num
   * @return
   */
  explicit Shape(Railway railway, const int& num);

  /*
   * add a coordinate to the shape
   *
   * @param coord
   * @return
   */
  void AddCoord(const GeoCoordinate& coord);

  /*
   * get all the latitudes in the shape
   *
   * @param
   * @return list of latitude
   */
  vector<string> GetShapePtLats() const;

  /*
   * get all the longitudes in the shape
   *
   * @param
   * @return list of longitude
   */
  vector<string> GetShapePtLons() const;

  /*
   * get the sequence list of all points in the shape
   *
   * @param
   * @return sequence list
   */
  vector<string> GetShapePtSequence() const;

 private:
  // latitude-list
  vector<string> shapePtLats_;

  // longitude-list
  vector<string> shapePtLons_;

  // sequence
  vector<string> shapePtSequence_;
};

// Inline methods
// _____________________________________________________________________________
Shape::Shape(Railway railway, const int& num) {
  id_ = to_string(num);
  pair<StrListIte, StrListIte> latTails = railway.GetShapePtLats();
  pair<StrListIte, StrListIte> lonTails = railway.GetShapePtLons();
  pair<StrListIte, StrListIte> seqTails = railway.GetShapePtSequence();
  shapePtLats_.assign(latTails.first, latTails.second);
  shapePtLons_.assign(lonTails.first, lonTails.second);
  shapePtSequence_.assign(seqTails.first, seqTails.second);
}

// _____________________________________________________________________________
void Shape::AddCoord(const GeoCoordinate& coord) {
  shapePtLats_.push_back(to_string(coord.get<0>()));
  shapePtLons_.push_back(to_string(coord.get<1>()));
  shapePtSequence_.push_back(to_string(shapePtSequence_.size()));
}

// _____________________________________________________________________________
vector<string> Shape::GetShapePtLats() const {
  return shapePtLats_;
}

// _____________________________________________________________________________
vector<string> Shape::GetShapePtLons() const {
  return shapePtLons_;
}

// _____________________________________________________________________________
vector<string> Shape::GetShapePtSequence() const {
  return shapePtSequence_;
}

#endif  // SRC_GTFS_SHAPE_H_
