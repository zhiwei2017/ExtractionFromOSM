// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_GTFS_GTFS_H_
#define SRC_GTFS_GTFS_H_

#include <gtest/gtest_prod.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "../Network/Network.h"
#include "./CSVWriter.h"

using std::string;
using std::hash;
using std::unordered_map;
using std::unordered_set;
using std::pair;

typedef pair<GeoCoordinate, ID> GTFSValue;

// hash function for stop
struct HashStop {
  size_t operator()(const Stop& stop) const {
    return hash<string>()(stop.GetID());
  }
};

// compare function for stop
struct CompareStop {
  bool operator()(const Stop& stop1, const Stop& stop2) const {
    return !(stop1.GetID().compare(stop2.GetID()));
  }
};

/* transform ntwork into GTFS feeds */
class GTFS : public Functions {
 public:
  /*
   * constructor
   *
   * @param nPtr
   * @return
   */
  explicit GTFS(Network* nPtr);

  /*
   * check the given vertex is a station or not
   *
   * @param vertexPtr
   * @return
   */
  bool IsStation(Vertex* vertexPtr) const;
  FRIEND_TEST(IsStationTest, IsStation);

  /*
   * build the r-tree for checking equal station
   *
   * @param
   * @return
   */
  void BuildRtree();

  /*
   * check if there is a equal station for the given one
   *
   * @param vertexPtr
   * @return
   */
  void EqualStationCheck(Vertex* vertexPtr);
  FRIEND_TEST(EqualStationCheckTest, EqualStationCheck);

  /*
   * generate a name
   *
   * @param first: first station
   * @param last: last station
   * @return name
   */
  string GenerateRouteName(Stop* first, Stop* last) const;

  /*
   * generate a uic_ref
   *
   * @param
   * @return uic_ref
   */
  string GenerateUicRef();

  /*
   * transform the network into GTFS
   *
   * @param
   * @return
   */
  void TransformIntoGTFS();
  FRIEND_TEST(TransformIntoGTFSTest, TransformIntoGTFS);

  /*
   * create a railway for each branch
   *
   * @param gPtr
   * @return
   */
  vector<Railway> Separate(Graph* gPtr);

  /*
   * generate GTFS feeds
   *
   * @param 
   * @return
   */
  void GenerateFeed();

  /*
   * run the tranform system
   *
   * @param 
   * @return
   */
  void Run();

 private:
  // network pointer
  Network* nPtr_;

  // r-tree
  bgi::rtree<GTFSValue, bgi::linear<16> > geometryRTree_;

  // stop map, id maps stop
  unordered_map<string, Stop> stops_;

  // route list
  vector<Route> routes_;

  // trip list
  vector<Trip> trips_;

  // shape list
  vector<Shape> shapes_;

  // count how many uic_ref has been generated
  size_t num_;

  // set of uic_ref
  unordered_set<string> uicRefs_;
};

#endif  // SRC_GTFS_GTFS_H_
