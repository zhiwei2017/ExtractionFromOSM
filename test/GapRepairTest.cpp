// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include <gtest/gtest.h>
#include <vector>
#include <deque>
#include "../src/Process/GapRepair.h"
#include "../src/Process/GapRepair.cpp"
#include "../src/Network/Network.h"
#include "../src/Network/Network.cpp"

using std::vector;
using std::deque;

// _____________________________________________________________________________
TEST(GapCheckTest, GapCheck) {
  Network network;
  deque<deque<ID>> vertexLists = {{1, 2, 3}, {8, 7, 4}, {8, 9, 10}, {16, 13}};
  deque<ID> edgeIDsIni = {111, 222, 333, 444};

  deque<TopoNode> edgeIDList;
  deque<ID> vertexIdList = {2, 4, 10};

  for (int i = 0; i < vertexLists.size(); i++) {
    Edge edge(edgeIDsIni[i]);
    edge.InsertVertex(vertexLists[i]);
    network.AddEdge(edge);
    edgeIDList.push_back(TopoNode(edgeIDsIni[i], -2));
  }
  Graph g(1111);
  g.PushBackEdge(edgeIDList);
  g.PushBackVertex(vertexIdList);
  network.AddGraph(g);

  deque<ID> vertexIDs = {1, 2, 3, 4, 7, 8, 9, 10, 13, 16};
  deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
    GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
    GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00008, 0.00008),
    GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
    GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016),
    GeoCoordinate(0.00018, 0.00018)};
  deque<deque<ID>> adjLists = {{111}, {}, {111}, {222}, {},
    {222, 333}, {}, {333}, {444}, {444}};
  for (int i = 0; i < vertexIDs.size(); i++) {
    Vertex vertex(vertexIDs[i], coords[i]);
    for (int j = 0; j < adjLists[i].size() ; j++) {
      vertex.AddToAdjacenyList(adjLists[i][j]);
    }
    network.AddVertex(vertex);
  }


  GapRepair gr(&network);
  gr.GapCheck();

  deque<int> gaps = {0, 2};

  ASSERT_EQ(1, gr.graphGaps_.size());
  int i = 0;
  for (auto it = gr.graphGaps_.at(1111).GetGapListBegin();
       it != gr.graphGaps_.at(1111).GetGapListEnd(); ++it, ++i) {
    ASSERT_EQ(gaps[i], *it);
  }
}

// _____________________________________________________________________________
TEST(BuildRtreeTest, BuildRtree) {
  Network network;
  deque<deque<ID>> vertexLists = {{1, 2, 3}, {3, 4, 5, 6}, {14, 8, 7, 4},
    {17, 8, 9, 10}, {12, 11, 10, 18}, {16, 13}, {12, 13}};
  deque<ID> edgeIDsIni = {111, 222, 444, 555, 777, 888, 999};

  deque<TopoNode> edgeIDList;
  deque<ID> vertexIdList = {2, 4, 10};

  for (int i = 0; i < vertexLists.size(); i++) {
    Edge edge(edgeIDsIni[i]);
    edge.InsertVertex(vertexLists[i]);
    network.AddEdge(edge);
    edgeIDList.push_back(TopoNode(edgeIDsIni[i], -2));
  }

  deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 16, 17, 18};
  deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
    GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
    GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00006, 0.00010),
    GeoCoordinate(0.00006, 0.00014), GeoCoordinate(0.00008, 0.00008),
    GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
    GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016),
    GeoCoordinate(0.00018, 0.00018), GeoCoordinate(0.00020, 0.00020),
    GeoCoordinate(0.00014, 0.00010), GeoCoordinate(0.00022, 0.00022),
    GeoCoordinate(0.00010, 0.00016), GeoCoordinate(0.00018, 0.00024)};
  for (int i = 0; i < vertexIDs.size(); i++) {
    Vertex vertex(vertexIDs[i], coords[i]);
    network.AddVertex(vertex);
  }

  Graph graph(1111);
  graph.PushBackEdge(edgeIDList);
  graph.PushBackVertex(vertexIdList);
  network.AddGraph(graph);

  GapRepair gr(&network);
  gr.GapCheck();
  gr.BuildRTree();
  vector<Value> result;
  gr.geometryRTree_.query(bgi::nearest(GeoCoordinate(0.00018, 0.00018), 2),
                          std::back_inserter(result));
  ASSERT_EQ(2, result.size());
  int a(0), b(0);
  for (int i = 0; i < result.size(); i++) {
    if (777 == result[i].second.GetID()) {
      a++;
    } else if (999 == result[i].second.GetID() ) {
      b++;
    }
  }
  ASSERT_EQ(1, a);
  ASSERT_EQ(1, b);
}

// _____________________________________________________________________________
TEST(FindNeighboursTest, FindNeighbours) {
  Network network;
  deque<deque<ID>> vertexLists = {{1, 2, 3}, {3, 4, 5, 6}, {14, 8, 7, 4},
    {17, 8, 9, 10}, {12, 11, 10, 18}, {16, 13}, {12, 13}};
  deque<ID> edgeIDsIni = {111, 222, 444, 555, 777, 888, 999};

  deque<TopoNode> edgeIDList;
  deque<ID> vertexIdList = {2, 4, 10};

  for (int i = 0; i < vertexLists.size(); i++) {
    Edge edge(edgeIDsIni[i]);
    edge.InsertVertex(vertexLists[i]);
    network.AddEdge(edge);
    edgeIDList.push_back(TopoNode(edgeIDsIni[i], -2));
  }

  deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
    11, 12, 13, 14, 16, 17, 18};
  deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
    GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
    GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00006, 0.00010),
    GeoCoordinate(0.00006, 0.00014), GeoCoordinate(0.00008, 0.00008),
    GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
    GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016),
    GeoCoordinate(0.00018, 0.00018), GeoCoordinate(0.00020, 0.00020),
    GeoCoordinate(0.00014, 0.00010), GeoCoordinate(0.00022, 0.00022),
    GeoCoordinate(0.00010, 0.00016), GeoCoordinate(0.00018, 0.00024)};
  for (int i = 0; i < vertexIDs.size(); i++) {
    Vertex vertex(vertexIDs[i], coords[i]);
    network.AddVertex(vertex);
  }

  Graph graph(1111);
  graph.PushBackEdge(edgeIDList);
  graph.PushBackVertex(vertexIdList);
  network.AddGraph(graph);

  GapRepair gr(&network);
  gr.GapCheck();
  gr.BuildRTree();


  Edge* edgePtr = network.GetEdgePtr(777);
  LineString line = network.GetLine(edgePtr);
  MegaNode current(777, line);
  vector<MegaNode> result = gr.FindNeighbors(current);
  ASSERT_EQ(2, result.size());
  int a(0), b(0);
  for (int i = 0; i < result.size(); i++) {
    if (999 == result[i].GetID()) {
      a++;
    } else if (555 == result[i].GetID()) {
      b++;
    }
  }
  ASSERT_EQ(1, a);
  ASSERT_EQ(1, b);
}

// _____________________________________________________________________________
TEST(RepairTest, Repair) {
  Network network;
  deque<deque<ID>> vertexLists = {{1, 2, 3}, {3, 4}, {4, 5, 6},
    {7, 8, 9}, {6, 7}};
  deque<ID> edgeIDsIni = {111, 222, 333, 444, 555};

  deque<TopoNode> edgeIDList;
  deque<ID> vertexIdList = {2, 4, 8};

  for (int i = 0; i < vertexLists.size(); i++) {
    Edge edge(edgeIDsIni[i]);
    edge.InsertVertex(vertexLists[i]);
    network.AddEdge(edge);
    if (i < vertexLists.size() - 1) {
      edgeIDList.push_back(TopoNode(edgeIDsIni[i], -2));
    }
  }

  deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
    GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
    GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00008, 0.00008),
    GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
    GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016)};
  for (int i = 0; i < vertexIDs.size(); i++) {
    Vertex vertex(vertexIDs[i], coords[i]);
    network.AddVertex(vertex);
  }

  Graph graph(1111);
  graph.PushBackEdge(edgeIDList);
  graph.PushBackVertex(vertexIdList);
  network.AddGraph(graph);


  GapRepair gr(&network);
  gr.GapCheck();
  gr.BuildRTree();
  gr.Repair();

  deque<deque<ID>> vertexListsForCheck = {{1, 2, 3}, {3, 4}, {4, 5, 6}, {6, 7},
    {7, 8, 9}};
  deque<ID> edgeIDs = {111, 222, 333, 555, 444};
  deque<int> stateList = {-2, -2, -2, -2, -2};

  Graph* graphPtr = network.GetGraphPtr(1111);
  for (int i = 0; i < vertexListsForCheck.size(); i++) {
    ASSERT_EQ(edgeIDs[i], graphPtr->GetEdgeID(i));
    ASSERT_EQ(stateList[i], graphPtr->GetEdgeState(i));

    Edge* ed = network.GetEdgePtr(edgeIDs[i]);
    for (int j = 0; j < vertexListsForCheck[i].size(); j++) {
      ASSERT_EQ(vertexListsForCheck[i][j], ed->GetVertexID(j));
    }
  }

  ASSERT_EQ(0, gr.graphGaps_.size());
}
