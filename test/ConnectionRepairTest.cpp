// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include <gtest/gtest.h>
#include <deque>
#include <vector>
#include "../src/Process/ConnectionRepair.h"
#include "../src/Process/ConnectionRepair.cpp"
#include "../src/Network/Network.h"
#include "../src/Network/Network.cpp"

using std::vector;
using std::deque;

// _____________________________________________________________________________
TEST(CreateNewEdgesTest, CreateNewEdges) {
  Network network;
  network.AddEdge(11);
  network.AddEdge(12);
  network.AddEdge(14);
  network.AddEdge(15);
  network.AddEdge(19);
  ConnectionRepair cr(&network);
  vector<ID> result = cr.CreateNewEdges(11);
  ASSERT_EQ(13, result[0]);
  ASSERT_EQ(16, result[1]);

  result = cr.CreateNewEdges(16, true);
  ASSERT_EQ(17, result[0]);
  ASSERT_EQ(18, result[1]);
  ASSERT_EQ(20, result[2]);
}

// _____________________________________________________________________________
TEST(SeparateEdgeTest, SeparateEdge) {
  Network network;
  // edges
  deque<deque<ID>> vertexLists = {{1, 2, 3}, {3, 4, 5, 6}, {14, 8, 7, 4},
    {17, 8, 9, 10}, {12, 11, 10, 18}, {12, 13}};
  deque<ID> edgeIDsIni = {111, 222, 444, 555, 777, 888};
  // graph
  deque<TopoNode> edgeIDList;
  deque<int> states = {-2, -2, -1, 1, 2, -2};
  deque<ID> vertexIdList = {2, 4, 10};

  for (int i = 0; i < vertexLists.size(); i++) {
    Edge edge(edgeIDsIni[i]);
    edge.InsertVertex(vertexLists[i]);
    network.AddEdge(edge);
    edgeIDList.push_back(TopoNode(edgeIDsIni[i], states[i]));
  }
  Graph graph(1111);
  graph.PushBackEdge(edgeIDList);
  graph.PushBackVertex(vertexIdList);
  network.AddGraph(graph);

  deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17, 18};
  deque<deque<ID>> adjLists = {{111}, {}, {111, 222}, {444}, {}, {222}, {}, {}, {}, {555},
    {}, {777, 888}, {888}, {444}, {555}, {777}};
  for (int i = 0; i < vertexIDs.size(); i++) {
    Vertex vertex(vertexIDs[i]);
    for (int j = 0; j < adjLists[i].size() ; j++) {
      vertex.AddToAdjacenyList(adjLists[i][j]);
    }
    network.AddVertex(vertex);
  }

  /*
   *
   * old edgeMap: (edgeID, state) : vertexList
   * (111, -2) : {1, 2, 3}
   * (222, -2) : {3, 4, 5, 6}
   * (444, -1) : {14, 8, 7 ,4}
   * (555, 1) : {17, 8, 9, 10}
   * (777, 2) : {12, 11, 10, 18}
   * (888, -2) : {12, 13}
   * the graph is {111, 222, 444, 555, 777, 888}
   *
   * after the seperate the new edgeMap should be
   * (111, -2) : {1, 2, 3}
   * (222, -2) : {3, 4, 5, 6}
   * (223, -2) : {3, 4}
   * (224, 0) : {4, 5, 6}
   * (444, -1) : {14, 8, 7 ,4}
   * (445, 0) : {14, 8}
   * (446, -1) : {8, 7, 4}
   * (555, 1) : {17, 8, 9, 10}
   * (556, 0) : {17, 8}
   * (557, -2) : {8, 9, 10}
   * (777, 2) : {12, 11, 10, 18}
   * (778, -1) : {12, 11, 10}
   * (779, 0) : {10, 18}
   * (888, -2) : {12, 13}
   * the graph is {111, 223, 224, 445, 446, 556, 557, 778, 779, 888}
   *
   */
  ConnectionRepair cr(&network);
  Graph* graphPtr = network.GetGraphPtr(1111);
  cr.SeparateEdge(graphPtr, 1, 1);
  cr.SeparateEdge(graphPtr, 3, 1);
  cr.SeparateEdge(graphPtr, 5, -2);
  cr.SeparateEdge(graphPtr, 7, -1);

  deque<deque<ID>> vertexListsForCheck = {{1, 2, 3}, {3, 4}, {4, 5, 6}, {8, 7, 4},
    {14, 8}, {17, 8}, {8, 9, 10}, {10, 18}, {12, 11, 10}, {12, 13}};
  deque<ID> edgeIDs = {111, 223, 224, 446, 445, 556, 557, 779, 778, 888};
  deque<int> stateList = {-2, -2, -4, -1, -3, -3, -2, -4, -1, -2};

  Graph* re = network.GetGraphPtr(1111);
  for (int i = 0; i < vertexListsForCheck.size(); i++) {
    ASSERT_EQ(edgeIDs[i], re->GetEdgeID(i));
    ASSERT_EQ(stateList[i], re->GetEdgeState(i));

    Edge* ed = network.GetEdgePtr(edgeIDs[i]);
    for (int j = 0; j < vertexListsForCheck[i].size(); j++) {
      ASSERT_EQ(vertexListsForCheck[i][j], ed->GetVertexID(j));
    }
  }
}

// _____________________________________________________________________________
TEST(SeparateEdgeThreeTest, SeparateEdgeThree) {
  Network network;
  // edges
  deque<deque<ID>> vertexLists = {{1, 2, 4}, {3, 4, 5, 6}, {5, 8, 7}};
  deque<ID> edgeIDsIni = {111, 222, 444};
  // graph
  deque<TopoNode> edgeIDList;
  deque<int> states = {-2, 1, -2};
  deque<ID> vertexIdList = {2, 4, 7};

  for (int i = 0; i < vertexLists.size(); i++) {
    Edge edge(edgeIDsIni[i]);
    edge.InsertVertex(vertexLists[i]);
    network.AddEdge(edge);
    edgeIDList.push_back(TopoNode(edgeIDsIni[i], states[i]));
  }
  Graph graph(1111);
  graph.PushBackEdge(edgeIDList);
  graph.PushBackVertex(vertexIdList);
  network.AddGraph(graph);

  deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8};
  deque<deque<ID>> adjLists = {{111}, {}, {222}, {111}, {444}, {222}, {444}, {}};
  for (int i = 0; i < vertexIDs.size(); i++) {
    Vertex vertex(vertexIDs[i]);
    for (int j = 0; j < adjLists[i].size() ; j++) {
      vertex.AddToAdjacenyList(adjLists[i][j]);
    }
    network.AddVertex(vertex);
  }

  ConnectionRepair cr(&network);
  Graph* graphPtr = network.GetGraphPtr(1111);
  cr.SeparateEdgeThree(graphPtr, 1, 2);

  deque<deque<ID>> vertexListsForCheck = {{1, 2, 4}, {3, 4}, {4, 5}, {5, 6}, {5, 8, 7}};
  deque<ID> edgeIDs = {111, 223, 224, 225, 444};
  deque<int> stateList = {-2, -3, -2, -4, -2};

  Graph* re = network.GetGraphPtr(1111);
  for (int i = 0; i < vertexListsForCheck.size(); i++) {
    ASSERT_EQ(edgeIDs[i], re->GetEdgeID(i));
    ASSERT_EQ(stateList[i], re->GetEdgeState(i));

    Edge* ed = network.GetEdgePtr(edgeIDs[i]);
    for (int j = 0; j < vertexListsForCheck[i].size(); j++) {
      ASSERT_EQ(vertexListsForCheck[i][j], ed->GetVertexID(j));
    }
  }
}

// _____________________________________________________________________________
TEST(RepairTest, Repair) {
  Network network;
  deque<deque<ID>> vertexLists = {{1, 2, 3}, {3, 4, 5, 6}, {14, 8, 7, 4},
    {17, 8, 9, 10}, {12, 11, 10, 18}, {12, 13}};
  deque<ID> edgeIDsIni = {111, 222, 444, 555, 777, 888};

  deque<TopoNode> edgeIDList;
  deque<ID> vertexIdList = {2, 4, 10};

  for (int i = 0; i < vertexLists.size(); i++) {
    Edge edge(edgeIDsIni[i]);
    edge.InsertVertex(vertexLists[i]);
    network.AddEdge(edge);
    edgeIDList.push_back(TopoNode(edgeIDsIni[i], -2));
  }
  Graph graph(1111);
  graph.PushBackEdge(edgeIDList);
  graph.PushBackVertex(vertexIdList);
  network.AddGraph(graph);

  deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 17, 18};
  deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
    GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
    GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00006, 0.00010),
    GeoCoordinate(0.00006, 0.00014), GeoCoordinate(0.00008, 0.00008),
    GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
    GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016),
    GeoCoordinate(0.00018, 0.00018), GeoCoordinate(0.00020, 0.00020),
    GeoCoordinate(0.00014, 0.00010), GeoCoordinate(0.00022, 0.00022),
    GeoCoordinate(0.00010, 0.00016), GeoCoordinate(0.00018, 0.00024)};
  deque<deque<ID>> adjLists = {{111}, {}, {111, 222}, {444}, {}, {222}, {}, {}, {}, {555},
    {}, {777, 888}, {888}, {444}, {555}, {777}};
  for (int i = 0; i < vertexIDs.size(); i++) {
    Vertex vertex(vertexIDs[i], coords[i]);
    for (int j = 0; j < adjLists[i].size() ; j++) {
      vertex.AddToAdjacenyList(adjLists[i][j]);
    }
    network.AddVertex(vertex);
  }

  ConnectionRepair cr(&network);
  Graph* graphPtr = network.GetGraphPtr(1111);
  cr.Repair(graphPtr, 1);
  cr.Repair(graphPtr, 3);
  cr.Repair(graphPtr, 5);
  cr.Repair(graphPtr, 7);

  deque<deque<ID>> vertexListsForCheck = {{1, 2, 3}, {3, 4}, {4, 5, 6}, {8, 7, 4},
     {14, 8}, {17, 8}, {8, 9, 10}, {10, 18}, {12, 11, 10}, {12, 13}};
  deque<ID> edgeIDs = {111, 223, 224, 446, 445, 556, 557, 779, 778, 888};
  deque<int> stateList = {-2, -2, -4, -1, -3, -3, -2, -4, -1, -2};

  Graph* re = network.GetGraphPtr(1111);
  for (int i = 0; i < vertexListsForCheck.size(); i++) {
    ASSERT_EQ(edgeIDs[i], re->GetEdgeID(i));
    ASSERT_EQ(stateList[i], re->GetEdgeState(i));

    Edge* ed = network.GetEdgePtr(edgeIDs[i]);
    for (int j = 0; j < vertexListsForCheck[i].size(); j++) {
      ASSERT_EQ(vertexListsForCheck[i][j], ed->GetVertexID(j));
    }
  }
}

