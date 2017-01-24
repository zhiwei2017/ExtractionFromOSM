// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include <gtest/gtest.h>
#include <deque>
#include "../src/Process/NetworkBuilder.h"
#include "../src/Process/NetworkBuilder.cpp"

using std::deque;

// _____________________________________________________________________________
TEST(BuildTest, Build) {
  {
    Network og;
    NetworkBuilder networkBuilder(&og);
    networkBuilder.Build("./test/test.osm", true, false, 500);
    Graph* graphPtr = og.GetGraphPtr(111);

    deque<ID> edgeIDs = {11, 22, 33, 44, 55};
    deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6};
    deque<deque<GeoCoordinate>> coords = {
      {GeoCoordinate(0.0002, 0.002), GeoCoordinate(0.0004, 0.0004)},
      {GeoCoordinate(0.0004, 0.0004), GeoCoordinate(0.0006, 0.0006)},
      {GeoCoordinate(0.0006, 0.0006), GeoCoordinate(0.0008, 0.0008)},
      {GeoCoordinate(0.0008, 0.0008), GeoCoordinate(0.0010, 0.0010)},
      {GeoCoordinate(0.0010, 0.0010), GeoCoordinate(0.0012, 0.0012)}};
    deque<deque<ID>> vertexLits = {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}};

    ASSERT_EQ(og.GetGraphMapSize(), 1);
    ASSERT_EQ(og.GetEdgeMapSize(), 5);
    ASSERT_EQ(graphPtr->GetEdgeListSize(), 5);
    for (int i = 0; i < graphPtr->GetEdgeListSize(); ++i) {
      ASSERT_EQ(edgeIDs[i], graphPtr->GetEdgeID(i));
      Edge* edgePtr = og.GetEdgePtr(graphPtr->GetEdgeID(i));
      for (int j = 0; j < edgePtr->GetVertexListSize(); ++j) {
        ASSERT_EQ(vertexLits[i][j], edgePtr->GetVertexID(j));
        Vertex* vertexPtr = og.GetVertexPtr(edgePtr->GetVertexID(j));
        ASSERT_EQ(coords[i][j].get<0>(), vertexPtr->GetCoordinate().get<0>());
      }
    }
  }
  {
    Network og;
    NetworkBuilder networkBuilder(&og);
    networkBuilder.Build("./test/test.osm", true, true, 500);
    Graph* graphPtr = og.GetGraphPtr(111);

    deque<ID> edgeIDs = {11, 22, 33, 44, 55};
    deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6};
    deque<deque<GeoCoordinate>> coords = {
      {GeoCoordinate(0.0002, 0.002), GeoCoordinate(0.0004, 0.0004)},
      {GeoCoordinate(0.0004, 0.0004), GeoCoordinate(0.0006, 0.0006)},
      {GeoCoordinate(0.0006, 0.0006), GeoCoordinate(0.0008, 0.0008)},
      {GeoCoordinate(0.0008, 0.0008), GeoCoordinate(0.0010, 0.0010)},
      {GeoCoordinate(0.0010, 0.0010), GeoCoordinate(0.0012, 0.0012)}};
    deque<deque<ID>> vertexLits = {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}};

    ASSERT_EQ(og.GetGraphMapSize(), 1);
    ASSERT_EQ(og.GetEdgeMapSize(), 6);
    ASSERT_EQ(og.GetVertexMapSize(), 8);
    ASSERT_EQ(graphPtr->GetEdgeListSize(), 5);
    for (int i = 0; i < graphPtr->GetEdgeListSize(); ++i) {
      ASSERT_EQ(edgeIDs[i], graphPtr->GetEdgeID(i));
      Edge* edgePtr = og.GetEdgePtr(graphPtr->GetEdgeID(i));
      for (int j = 0; j < edgePtr->GetVertexListSize(); ++j) {
        ASSERT_EQ(vertexLits[i][j], edgePtr->GetVertexID(j));
        Vertex* vertexPtr = og.GetVertexPtr(edgePtr->GetVertexID(j));
        ASSERT_EQ(coords[i][j].get<0>(), vertexPtr->GetCoordinate().get<0>());
      }
    }
  }
}
