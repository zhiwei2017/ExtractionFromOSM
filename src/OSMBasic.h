// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#ifndef SRC_OSMBASIC_H_
#define SRC_OSMBASIC_H_

#include <boost/lexical_cast.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

using std::string;
using std::unordered_map;
using std::unordered_set;

typedef uint64_t ID;
// the lat lon coordinate
typedef bg::model::point<double, 2,
bg::cs::spherical_equatorial<bg::degree> > GeoCoordinate;
typedef bg::model::segment<GeoCoordinate> Segment;
typedef bg::model::box<GeoCoordinate> Box;
typedef bg::model::linestring<GeoCoordinate> LineString;

#define EarthRadius 6372000
#define Normal -2
#define Inverse -1
#define NormalRedundant -4
#define InverseRedundant -3
#define Unknown -5
#define Redundant 0

const int MaxStationLength = 300;
const unordered_set<string> OriginalStr({"name", "ref", "from", "to"});
const unordered_set<string> StationKeywords({"stop_position",
  "platform", "stop_area", "station"});
const unordered_set<string> LongDistanceKeywords({"long_distance",
  "high_speed", "national", "international", "night"});
const unordered_set<string> LocalKeywords({"regional", "tourism",
  "commuter", "commuter_rail"});


/* this class is mainly responsible for the osm file seperation*/
class Functions {
 public:
  const char* ConvertToC(const unsigned char* id);

  const unsigned char* ConvertToUC(const char* id);

  template <typename T>
  ID ConvertToID(const T& t) const {
    return boost::lexical_cast<ID>(t);
  }

  template <typename T>
  double ConvertToDouble(const T& t) const {
    return boost::lexical_cast<double>(t);
  }

  template <typename T>
  string ConvertToStr(const T& t) const {
    return boost::lexical_cast<string>(t);
  }

  string GetLowercase(const string& orig) const;

  /*
   * calculate the geometric distance between two given geometric types.
   * I use 6372000 as the radius of the earth in meter.
   *
   * @param place1
   * @param place2
   * @return the geometric distance
   */
  template <typename T, typename S>
  double CalculateDistance(const T& t, const S& s) const {
    return bg::distance(t, s) * EarthRadius;
  }

  /*
   * calculate the geometric length of a given geometric line
   * I use 6372000 as the radius of the earth in meter.
   *
   * @param line
   * @return the geometric distance
   */
  double CalculateLength(const LineString& line) const;
};

template <typename T>
class BasicType {
 public:
  T GetID() const {
    return id_;
  }

 protected:
  T id_;
};

class Attribute {
 public:
  void AddToAttributes(const string& key, const string& value);

  void ChangeAttribute(const string& key, const string& value);

  bool ContainAttribute(const string& key) const;

  string GetAttribute(const string& key);

 protected:
  // store the attributes of this node
  unordered_map<string, string> attributes_;
};

// Inline methods
// _____________________________________________________________________________
const char* Functions::ConvertToC(const unsigned char* id) {
  return (const char*)id;
}

// _____________________________________________________________________________
const unsigned char* Functions::ConvertToUC(const char* id) {
  return (const unsigned char*)id;
}

// ____________________________________________________________________________
string Functions::GetLowercase(const string& orig) const {
  string retVal;
  retVal.reserve(orig.size());
  for (int i = 0; i < orig.size(); ++i) {
    retVal += tolower(orig[i]);
  }
  return retVal;
}

// _____________________________________________________________________________
double Functions::CalculateLength(const LineString& line) const {
  return length(line) * EarthRadius;
}

// _____________________________________________________________________________
void Attribute::AddToAttributes(const string& key, const string& value) {
  attributes_.emplace(key, value);
}

// _____________________________________________________________________________
void Attribute::ChangeAttribute(const string& key, const string& value) {
  attributes_.at(key) = value;
}

// _____________________________________________________________________________
string Attribute::GetAttribute(const string& key) {
  return attributes_.count(key) ? attributes_[key] : "";
}

// _____________________________________________________________________________
bool Attribute::ContainAttribute(const string& key) const {
  return attributes_.count(key);
}

#endif  // SRC_OSMBASIC_H_
