// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include <gtest/gtest.h>
#include <deque>
#include "../src/Process/OrderRepair.h"
#include "../src/Process/OrderRepair.cpp"
#include "../src/Network/Network.h"
#include "../src/Network/Network.cpp"

using std::deque;

// _____________________________________________________________________________
TEST(OrderRepairTest, OrderRepair) {
  {
    Network og;
    deque<deque<ID>> ndLists = {{9, 10, 11}, {1, 2, 3}, {5, 6, 7}, {3, 4, 5},
      {7, 8, 9}, {11, 12, 13}};
    deque<ID> edgeIDsIni = {555, 111, 333, 222, 444, 666};

    deque<TopoNode> edgeIDList;
    deque<ID> vertexIdList = {2, 4, 10};

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

    deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
      GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
      GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00008, 0.00008),
      GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
      GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016),
      GeoCoordinate(0.00018, 0.00018), GeoCoordinate(0.00020, 0.00020),
      GeoCoordinate(0.00022, 0.00022), GeoCoordinate(0.00024, 0.00024),
      GeoCoordinate(0.00026, 0.00026)};
    deque<deque<ID>> adjLists = {{111}, {}, {111, 222}, {}, {222, 333}, {},
      {333, 444}, {}, {444, 555}, {}, {555, 666}, {}, {666}};
    for (int i = 0; i < vertexIDs.size(); i++) {
      Vertex vertex(vertexIDs[i], coords[i]);
      for (int j = 0; j < adjLists[i].size() ; j++) {
        vertex.AddToAdjacenyList(adjLists[i][j]);
      }
      og.AddVertex(vertex);
    }
    Graph* re = og.GetGraphPtr(1111);

    OrderRepair ord(&og);
    vector<MegaNode> edgeList;
    ord.BuildRTree(re, &edgeList);
    ord.Repair(re, edgeList);

    deque<deque<ID>> vertexLists = {{1, 2, 3}, {3, 4, 5}, {5, 6, 7}, {3, 4, 5},
      {5, 6, 7}, {7, 8, 9}, {9, 10, 11}, {11, 12, 13}};
    deque<ID> edgeIDs = {111, 222, 333, 222, 333, 444, 555, 666};
    deque<int> stateList = {-2, -2, -2, -2, -2, -2, -2, -2};

    for (int i = 0; i < vertexLists.size(); i++) {
      ASSERT_EQ(edgeIDs[i], re->GetEdgeID(i));
      ASSERT_EQ(stateList[i], re->GetEdgeState(i));

      Edge* ed = og.GetEdgePtr(edgeIDs[i]);
      for (int j = 0; j < vertexLists[i].size(); j++) {
        ASSERT_EQ(vertexLists[i][j], ed->GetVertexID(j));
      }
    }
  }
  {
    Network og;
    deque<deque<ID>> ndLists = {{9, 10, 11}, {1, 2, 3}, {5, 6, 7},
      {7, 8, 9}, {11, 12, 13}};
    deque<ID> edgeIDsIni = {555, 111, 333, 444, 666};

    deque<TopoNode> edgeIDList;
    deque<ID> vertexIdList = {2, 4, 10};

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

    deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
      GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
      GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00008, 0.00008),
      GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
      GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016),
      GeoCoordinate(0.00018, 0.00018), GeoCoordinate(0.00020, 0.00020),
      GeoCoordinate(0.00022, 0.00022), GeoCoordinate(0.00024, 0.00024),
      GeoCoordinate(0.00026, 0.00026)};
    deque<deque<ID>> adjLists = {{111}, {}, {111}, {333}, {},
      {333, 444}, {}, {444, 555}, {}, {555, 666}, {}, {666}};
    for (int i = 0; i < vertexIDs.size(); i++) {
      Vertex vertex(vertexIDs[i], coords[i]);
      for (int j = 0; j < adjLists[i].size() ; j++) {
        vertex.AddToAdjacenyList(adjLists[i][j]);
      }
      og.AddVertex(vertex);
    }
    Graph* re = og.GetGraphPtr(1111);

    OrderRepair ord(&og);
    vector<MegaNode> edgeList;
    ord.BuildRTree(re, &edgeList);
    ord.Repair(re, edgeList);

    deque<deque<ID>> vertexLists = {{1, 2, 3}, {5, 6, 7},
      {7, 8, 9}, {9, 10, 11}, {11, 12, 13}};
    deque<ID> edgeIDs = {111, 333, 444, 555, 666};
    deque<int> stateList = {-2, -2, -2, -2, -2};

    for (int i = 0; i < vertexLists.size(); i++) {
      ASSERT_EQ(edgeIDs[i], re->GetEdgeID(i));
      ASSERT_EQ(stateList[i], re->GetEdgeState(i));

      Edge* ed = og.GetEdgePtr(edgeIDs[i]);
      for (int j = 0; j < vertexLists[i].size(); j++) {
        ASSERT_EQ(vertexLists[i][j], ed->GetVertexID(j));
      }
    }
  }
  {
    Network og;
    deque<deque<ID>> ndLists = {{9, 10, 11}, {1, 2, 3}, {5, 6, 7},
      {11, 12, 13}};
    deque<ID> edgeIDsIni = {555, 111, 333, 666};

    deque<TopoNode> edgeIDList;
    deque<ID> vertexIdList = {2, 4, 10};

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

    deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
      GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
      GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00008, 0.00008),
      GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
      GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016),
      GeoCoordinate(0.00018, 0.00018), GeoCoordinate(0.00020, 0.00020),
      GeoCoordinate(0.00022, 0.00022), GeoCoordinate(0.00024, 0.00024),
      GeoCoordinate(0.00026, 0.00026)};
    deque<deque<ID>> adjLists = {{111}, {}, {111}, {333}, {},
      {333}, {555}, {}, {555, 666}, {}, {666}};
    for (int i = 0; i < vertexIDs.size(); i++) {
      Vertex vertex(vertexIDs[i], coords[i]);
      for (int j = 0; j < adjLists[i].size() ; j++) {
        vertex.AddToAdjacenyList(adjLists[i][j]);
      }
      og.AddVertex(vertex);
    }
    Graph* re = og.GetGraphPtr(1111);

    OrderRepair ord(&og);
    vector<MegaNode> edgeList;
    ord.BuildRTree(re, &edgeList);
    ord.Repair(re, edgeList);

    deque<deque<ID>> vertexLists = {{1, 2, 3}, {5, 6, 7},
      {9, 10, 11}, {11, 12, 13}};
    deque<ID> edgeIDs = {111, 333, 555, 666};
    deque<int> stateList = {-2, -2, -2, -2};

    for (int i = 0; i < vertexLists.size(); i++) {
      ASSERT_EQ(edgeIDs[i], re->GetEdgeID(i));
      ASSERT_EQ(stateList[i], re->GetEdgeState(i));

      Edge* ed = og.GetEdgePtr(edgeIDs[i]);
      for (int j = 0; j < vertexLists[i].size(); j++) {
        ASSERT_EQ(vertexLists[i][j], ed->GetVertexID(j));
      }
    }
  }
  {
    Network og;
    deque<deque<ID>> ndLists = {{1, 2, 3}, {5, 6, 7}, {11, 12, 13}, {7, 8, 9}};
    deque<ID> edgeIDsIni = {111, 333, 666, 444};

    deque<TopoNode> edgeIDList;
    deque<ID> vertexIdList = {2, 4, 10};

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

    deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
      GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
      GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00008, 0.00008),
      GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
      GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016),
      GeoCoordinate(0.00018, 0.00018), GeoCoordinate(0.00020, 0.00020),
      GeoCoordinate(0.00022, 0.00022), GeoCoordinate(0.00024, 0.00024),
      GeoCoordinate(0.00026, 0.00026)};
    deque<deque<ID>> adjLists = {{111}, {}, {111}, {333}, {},
      {333, 444}, {}, {444}, {666}, {}, {666}};
    for (int i = 0; i < vertexIDs.size(); i++) {
      Vertex vertex(vertexIDs[i], coords[i]);
      for (int j = 0; j < adjLists[i].size() ; j++) {
        vertex.AddToAdjacenyList(adjLists[i][j]);
      }
      og.AddVertex(vertex);
    }
    Graph* re = og.GetGraphPtr(1111);

    OrderRepair ord(&og);
    vector<MegaNode> edgeList;
    ord.BuildRTree(re, &edgeList);
    ord.Repair(re, edgeList);

    deque<deque<ID>> vertexLists = {{1, 2, 3}, {5, 6, 7},
      {7, 8, 9}, {11, 12, 13}, {7, 8, 9}};
    deque<ID> edgeIDs = {111, 333, 444, 666, 444};
    deque<int> stateList = {-2, -2, -2, -2, -2};

    for (int i = 0; i < vertexLists.size(); i++) {
      ASSERT_EQ(edgeIDs[i], re->GetEdgeID(i));
      ASSERT_EQ(stateList[i], re->GetEdgeState(i));

      Edge* ed = og.GetEdgePtr(edgeIDs[i]);
      for (int j = 0; j < vertexLists[i].size(); j++) {
        ASSERT_EQ(vertexLists[i][j], ed->GetVertexID(j));
      }
    }
  }
  {
    Network og;
    deque<deque<ID>> ndLists = {{1, 2, 3}, {9, 10, 11}, {5, 6, 7},
      {11, 12, 13}};
    deque<ID> edgeIDsIni = {111, 555, 333, 666};

    deque<TopoNode> edgeIDList;
    deque<ID> vertexIdList = {2, 4, 10};

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

    deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    deque<GeoCoordinate> coords = {GeoCoordinate(0, 0),
      GeoCoordinate(0.00002, 0.00002), GeoCoordinate(0.00004, 0.00004),
      GeoCoordinate(0.00006, 0.00006), GeoCoordinate(0.00008, 0.00008),
      GeoCoordinate(0.00010, 0.00010), GeoCoordinate(0.00012, 0.00012),
      GeoCoordinate(0.00014, 0.00014), GeoCoordinate(0.00016, 0.00016),
      GeoCoordinate(0.00018, 0.00018), GeoCoordinate(0.00020, 0.00020),
      GeoCoordinate(0.00022, 0.00022), GeoCoordinate(0.00024, 0.00024),
      GeoCoordinate(0.00026, 0.00026)};
    deque<deque<ID>> adjLists = {{111}, {}, {111}, {333}, {},
      {333}, {555}, {}, {555, 666}, {}, {666}};
    for (int i = 0; i < vertexIDs.size(); i++) {
      Vertex vertex(vertexIDs[i], coords[i]);
      for (int j = 0; j < adjLists[i].size() ; j++) {
        vertex.AddToAdjacenyList(adjLists[i][j]);
      }
      og.AddVertex(vertex);
    }
    Graph* re = og.GetGraphPtr(1111);

    OrderRepair ord(&og);
    vector<MegaNode> edgeList;
    ord.BuildRTree(re, &edgeList);
    ord.Repair(re, edgeList);

    deque<deque<ID>> vertexLists = {{1, 2, 3}, {9, 10, 11}, {5, 6, 7}, 
      {9, 10, 11}, {11, 12, 13}};
    deque<ID> edgeIDs = {111, 555, 333, 555, 666};
    deque<int> stateList = {-2, -2, -2, -2, -2};

    for (int i = 0; i < vertexLists.size(); i++) {
      ASSERT_EQ(edgeIDs[i], re->GetEdgeID(i));
      ASSERT_EQ(stateList[i], re->GetEdgeState(i));

      Edge* ed = og.GetEdgePtr(edgeIDs[i]);
      for (int j = 0; j < vertexLists[i].size(); j++) {
        ASSERT_EQ(vertexLists[i][j], ed->GetVertexID(j));
      }
    }
  }
}

// _____________________________________________________________________________
TEST(OrderCheckTest, OrderCheck) {
  Network og;
  deque<deque<ID>> ndLists = {{310197161, 310197140},
    {433557526, 309739203}, {433557527, 310197140}};
  deque<ID> edgeIDsIni = {28953054, 28199925, 320398602};

  deque<TopoNode> edgeIDList;
  deque<ID> vertexIdList = {310197140, 309739203};

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

  deque<ID> vertexIDs = {310197161, 310197140, 433557526, 309739203,
    433557527};
  deque<GeoCoordinate> coords = {
    GeoCoordinate(52.5072, 13.4783), GeoCoordinate(52.5069, 13.4787),
    GeoCoordinate(52.507, 13.4788), GeoCoordinate(52.5072, 13.4783),
    GeoCoordinate(52.5068, 13.479)};
  deque<deque<ID>> adjLists = {{28953054}, {28953054, 320398602},
    {28199925}, {28199925}, {320398602}};
  for (int i = 0; i < vertexIDs.size(); i++) {
    Vertex vertex(vertexIDs[i], coords[i]);
    for (int j = 0; j < adjLists[i].size() ; j++) {
      vertex.AddToAdjacenyList(adjLists[i][j]);
    }
    og.AddVertex(vertex);
  }

  vector<int> gapList;
  OrderRepair ord(&og);
  ord.OrderCheck(&graph, &gapList);
  ASSERT_EQ(gapList.size(), 1);
  ASSERT_EQ(gapList[0], 1);
}

// _____________________________________________________________________________
TEST(AreConjointTest, AreConjoint) {
  {
    Network og;
    deque<deque<ID>> ndLists = {{1, 2, 3, 4, 5, 6, 7, 8, 9},
      {19, 10, 11, 12, 13, 14, 15, 16, 17}, {9, 18}};
    deque<ID> edgeIDsIni = {111, 222, 333};

    deque<TopoNode> edgeIDList;
    deque<ID> vertexIdList = {2, 4, 10};

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

    deque<ID> vertexIDs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
      14, 15, 16, 17, 18, 19};
    deque<GeoCoordinate> coords = {
      GeoCoordinate(49.8094, 11.645), GeoCoordinate(49.8095, 11.6429),
      GeoCoordinate(49.8096, 11.6421), GeoCoordinate(49.8097, 11.6403),
      GeoCoordinate(49.8098, 11.639), GeoCoordinate(49.81, 11.6379),
      GeoCoordinate(49.8104, 11.6365), GeoCoordinate(49.8107, 11.6355),
      GeoCoordinate(49.8109, 11.6349), GeoCoordinate(49.8107, 11.6354),
      GeoCoordinate(49.8104, 11.6362), GeoCoordinate(49.8104, 11.6364),
      GeoCoordinate(49.81, 11.6379), GeoCoordinate(49.8098, 11.639),
      GeoCoordinate(49.8097, 11.6403), GeoCoordinate(49.8096, 11.6415),
      GeoCoordinate(49.8095, 11.6421), GeoCoordinate(49.811, 11.6347),
      GeoCoordinate(49.8109, 11.6349)};
    deque<deque<ID>> adjLists = {{111}, {}, {}, {}, {}, {}, {}, {},
      {111, 333}, {}, {}, {}, {}, {}, {}, {}, {}, {333}, {222}};
    for (int i = 0; i < vertexIDs.size(); i++) {
      Vertex vertex(vertexIDs[i], coords[i]);
      for (int j = 0; j < adjLists[i].size() ; j++) {
        vertex.AddToAdjacenyList(adjLists[i][j]);
      }
      og.AddVertex(vertex);
    }

    OrderRepair ord(&og);
    ASSERT_TRUE(ord.AreConjoint(og.GetEdgePtr(111), og.GetEdgePtr(222)));
    ASSERT_TRUE(ord.AreConjoint(og.GetEdgePtr(111), og.GetEdgePtr(333)));
    ASSERT_TRUE(ord.AreConjoint(og.GetEdgePtr(222), og.GetEdgePtr(333)));
  }
  {
    Network og;
    deque<deque<ID>> ndLists = {{310197161, 310197140},
      {433557526, 309739203}, {433557527, 310197140}};
    deque<ID> edgeIDsIni = {28953054, 28199925, 320398602};

    deque<TopoNode> edgeIDList;
    deque<ID> vertexIdList = {310197140, 309739203};

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

    deque<ID> vertexIDs = {310197161, 310197140, 433557526, 309739203,
      433557527};
    deque<GeoCoordinate> coords = {
      GeoCoordinate(52.5072, 13.4783), GeoCoordinate(52.5069, 13.4787),
      GeoCoordinate(52.507, 13.4788), GeoCoordinate(52.5072, 13.4783),
      GeoCoordinate(52.5068, 13.479)};
    deque<deque<ID>> adjLists = {{28953054}, {28953054, 320398602},
      {28199925}, {28199925}, {320398602}};
    for (int i = 0; i < vertexIDs.size(); i++) {
      Vertex vertex(vertexIDs[i], coords[i]);
      for (int j = 0; j < adjLists[i].size() ; j++) {
        vertex.AddToAdjacenyList(adjLists[i][j]);
      }
      og.AddVertex(vertex);
    }

    OrderRepair ord(&og);
    ASSERT_TRUE(ord.AreConjoint(og.GetEdgePtr(28953054), og.GetEdgePtr(28199925)));
    ASSERT_TRUE(ord.AreConjoint(og.GetEdgePtr(28953054), og.GetEdgePtr(320398602)));
    ASSERT_FALSE(ord.AreConjoint(og.GetEdgePtr(28199925), og.GetEdgePtr(320398602)));
  }
}

