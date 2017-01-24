// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include <gtest/gtest.h>
#include <deque>
#include "../src/Network/Network.h"
#include "../src/Network/Network.cpp"
#include "../src/GTFS/GTFS.h"
#include "../src/GTFS/GTFS.cpp"

using std::deque;

// _____________________________________________________________________________
TEST(IsStationTest, IsStation) {
  Vertex vertex(1);
  vertex.AddToAttributes("public_transport", "platform");
  Network osmGraph;
  GTFS g(&osmGraph);
  ASSERT_TRUE(g.IsStation(&vertex));
}

// _____________________________________________________________________________
TEST(EqualStationCheckTest, EqualStationCheck) {
  {
    Vertex vertex1(1, 1.0, 1.0);
    Vertex vertex2(2, 1.000002, 1.000002);
    vertex1.AddToAttributes("public_transport", "platform");
    vertex2.AddToAttributes("public_transport", "platform");
    vertex1.AddToAttributes("uic_ref", "1111");
    Network osmGraph;
    osmGraph.AddVertex(vertex1);
    osmGraph.AddVertex(vertex2);

    GTFS g(&osmGraph);
    g.BuildRtree();
    g.EqualStationCheck(&vertex2);
    ASSERT_EQ("1111", g.stops_.at("2").GetParentStation());
  }
  {
    Vertex vertex1(1, 1.0, 1.0);
    Vertex vertex2(2, 1.000002, 1.000002);
    Vertex vertex3(3, 2.000002, 2.000002);
    vertex1.AddToAttributes("public_transport", "platform");
    vertex2.AddToAttributes("public_transport", "station");
    vertex3.AddToAttributes("public_transport", "station");
    vertex3.AddToAttributes("uic_ref", "1111");
    Network osmGraph;
    osmGraph.AddVertex(vertex1);
    osmGraph.AddVertex(vertex2);

    GTFS g(&osmGraph);
    g.BuildRtree();
    g.EqualStationCheck(&vertex2);
    ASSERT_EQ("10001", g.stops_.at("1").GetParentStation());
    ASSERT_EQ("10001", g.stops_.at("2").GetParentStation());
  }
}

// _____________________________________________________________________________
TEST(TransformIntoGTFSTest, TransformIntoGTFS) {
  Network og;
  deque<deque<ID>> ndLists = {{1, 2, 3}, {3, 4, 5, 6}, {6, 7, 8}};
  deque<ID> edgeIDsIni = {111, 222, 333};

  deque<TopoNode> edgeIDList;
  deque<ID> vertexIdList = {2, 4, 7};

  for (int i = 0; i < ndLists.size(); i++) {
    Edge edge(edgeIDsIni[i]);
    edge.InsertVertex(ndLists[i]);
    og.AddEdge(edge);
    edgeIDList.push_back(TopoNode(edgeIDsIni[i], -2));
  }
  Graph graph(1111);
  graph.PushBackEdge(edgeIDList);
  graph.PushBackVertex(vertexIdList);
  og.AddGraph(graph);

  deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8};
  deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
    GeoCoordinate(0.002, 0.002), GeoCoordinate(0.004, 0.004),
    GeoCoordinate(0.006, 0.006), GeoCoordinate(0.008, 0.008),
    GeoCoordinate(0.010, 0.010), GeoCoordinate(0.012, 0.012),
    GeoCoordinate(0.014, 0.014)};
  deque<deque<ID>> adjLists = {{111}, {}, {111, 222}, {}, {}, {222, 333}, {}, {333}};
  for (int i = 0; i < vertexIDs.size(); i++) {
    Vertex vertex(vertexIDs[i], coords[i]);
    for (int j = 0; j < adjLists[i].size() ; j++) {
      vertex.AddToAdjacenyList(adjLists[i][j]);
    }
    if (i == 1) {
      vertex.AddToAttributes("name", "A");
      vertex.AddToAttributes("public_transport", "station");
    } else if (i == 3) {
      vertex.AddToAttributes("name", "B");
      vertex.AddToAttributes("public_transport", "station");
    } else if (i == 6) {
      vertex.AddToAttributes("name", "C");
      vertex.AddToAttributes("public_transport", "station");
    }
    og.AddVertex(vertex);
  }

  GTFS g(&og);
  g.BuildRtree();
  g.TransformIntoGTFS();
  vector<string> lats = g.shapes_[0].GetShapePtLats();
  vector<string> lons = g.shapes_[0].GetShapePtLons();
  ASSERT_EQ(1, g.shapes_.size());
  ASSERT_EQ(8, lats.size());
  ASSERT_EQ(8, lons.size());
  for (int i = 0; i < coords.size(); i++) {
    ASSERT_EQ(to_string(coords[i].get<0>()), lats[i]);
    ASSERT_EQ(to_string(coords[i].get<1>()), lons[i]);
  }

  ASSERT_EQ(3, g.stops_.size());
  for (auto it = g.stops_.begin(); it != g.stops_.end(); it++) {
    if (it->first == "2") {
      ASSERT_EQ("2", g.stops_.at("2").GetID());
    } else if (it->first == "2") {
      ASSERT_EQ("4", g.stops_.at("4").GetID());
    } else {
      ASSERT_EQ("7", g.stops_.at("7").GetID());
    }
  }
  ASSERT_EQ(1, g.routes_.size());
  ASSERT_EQ("A - C", g.routes_.at(0).GetName());
  ASSERT_EQ(24, g.trips_.size());
}

