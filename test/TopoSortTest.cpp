// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include <gtest/gtest.h>
#include <unordered_set>
#include <deque>
#include "../src/Process/TopoSort.h"
#include "../src/Network/Network.h"
#include "../src/Network/Network.cpp"

using std::unordered_set;
using std::deque;

// _____________________________________________________________________________
TEST(TopoGraphTest, TopoGraph) {
  {
    Network og;
    // edges
    deque<deque<ID>> ndLists = {{2, 1}, {2, 3}, {4, 3}, {4, 5}, {6, 5}};
    deque<ID> edgeIDsIni = {111, 222, 333, 444, 555};
    // route
    deque<TopoNode> edgeList;
    deque<ID> edgeIDs = {111, 222, 333, 444, 333, 222, 333, 444, 555};
    deque<int> states = {-1, -2, -1, -4, -2, -3, -1, -2, -1};
    deque<ID> nodeIdList = {2, 4, 6};

    for (int i = 0; i < ndLists.size(); i++) {
      Edge edge(edgeIDsIni[i]);
      edge.InsertVertex(ndLists[i]);
      og.AddEdge(edge);
    }
    for (int i = 0; i < edgeIDs.size(); i++) {
      edgeList.push_back(TopoNode(edgeIDs[i], states[i]));
    }
    Graph route(1111);
    route.PushBackEdge(edgeList);
    route.PushBackVertex(nodeIdList);
    og.AddGraph(route);

    Graph* re = og.GetGraphPtr(1111);
    TopoGraph topoGraph(re);

    deque<ID> edgeIDList = {111, 222, 333, 444, 555};
    for (int i = 0; i < edgeIDList.size(); i++) {
      unordered_set<ID> adj = topoGraph.GetAdjacencyList(edgeIDList[i]);
      if (edgeIDList[i] == 111) {
        ASSERT_EQ(1, adj.size());
        ASSERT_TRUE(adj.count(222));
      } else if (edgeIDList[i] == 222) {
        ASSERT_EQ(2, adj.size());
        ASSERT_TRUE(adj.count(111));
        ASSERT_TRUE(adj.count(333));
      } else if (edgeIDList[i] == 333) {
        ASSERT_EQ(2, adj.size());
        ASSERT_TRUE(adj.count(222));
        ASSERT_TRUE(adj.count(444));
      } else  if (edgeIDList[i] == 444) {
        ASSERT_EQ(2, adj.size());
        ASSERT_TRUE(adj.count(555));
        ASSERT_TRUE(adj.count(333));
      } else {
        ASSERT_EQ(1, adj.size());
        ASSERT_TRUE(adj.count(444));
      }
    }
  }
  {
    Network og;
    // edges
    deque<deque<ID>> ndLists = {{2, 1}, {2, 3}, {3, 7}, {4, 3}, {4, 5}, {6, 5}};
    deque<ID> edgeIDsIni = {111, 222, 666, 333, 444, 555};
    // route
    deque<TopoNode> edgeList;
    deque<ID> edgeIDs = {111, 222, 666, 333, 444, 333, 222, 333, 444, 555};
    deque<int> states = {-1, -2, -4, -1, -4, -2, -3, -1, -2, -1};
    deque<ID> nodeIdList = {2, 4, 6};

    for (int i = 0; i < ndLists.size(); i++) {
      Edge edge(edgeIDsIni[i]);
      edge.InsertVertex(ndLists[i]);
      og.AddEdge(edge);
    }
    for (int i = 0; i < edgeIDs.size(); i++) {
      edgeList.push_back(TopoNode(edgeIDs[i], states[i]));
    }
    Graph route(1111);
    route.PushBackEdge(edgeList);
    route.PushBackVertex(nodeIdList);
    og.AddGraph(route);

    Graph* re = og.GetGraphPtr(1111);
    TopoGraph topoGraph(re);

    deque<ID> edgeIDList = {111, 222, 333, 444, 555, 666};
    for (int i = 0; i < edgeIDList.size(); i++) {
      unordered_set<ID> adj = topoGraph.GetAdjacencyList(edgeIDList[i]);
      if (edgeIDList[i] == 111) {
        ASSERT_EQ(1, adj.size());
        ASSERT_TRUE(adj.count(222));
      } else if (edgeIDList[i] == 222) {
        ASSERT_EQ(3, adj.size());
        ASSERT_TRUE(adj.count(111));
        ASSERT_TRUE(adj.count(333));
        ASSERT_TRUE(adj.count(666));
      } else if (edgeIDList[i] == 333) {
        ASSERT_EQ(3, adj.size());
        ASSERT_TRUE(adj.count(222));
        ASSERT_TRUE(adj.count(444));
        ASSERT_TRUE(adj.count(666));
      } else if (edgeIDList[i] == 444) {
        ASSERT_EQ(2, adj.size());
        ASSERT_TRUE(adj.count(555));
        ASSERT_TRUE(adj.count(333));
      } else if (edgeIDList[i] == 555)  {
        ASSERT_EQ(1, adj.size());
        ASSERT_TRUE(adj.count(444));
      } else {
        ASSERT_EQ(2, adj.size());
        ASSERT_TRUE(adj.count(222));
        ASSERT_TRUE(adj.count(333));
      }
    }
  }
}

// _____________________________________________________________________________
TEST(SortingTest, Sorting) {
  {
    Network og;
    // edges
    deque<deque<ID>> ndLists = {{1, 2, 3}, {4, 3}, {3, 5}, {6, 5}, {6, 7}, {1, 6}};
    deque<ID> edgeIDsIni = {111, 222, 333, 444, 555, 666};
    // route
    deque<TopoNode> edgeIDList;
    deque<int> states = {-2, -3, -2, -1, -4, -1};
    deque<ID> vertexIdList = {2, 4, 7};

    for (int i = 0; i < ndLists.size(); i++) {
      Edge edge(edgeIDsIni[i]);
      edge.InsertVertex(ndLists[i]);
      og.AddEdge(edge);
      edgeIDList.push_back(TopoNode(edgeIDsIni[i], states[i]));
    }
    Graph route(1111);
    route.PushBackEdge(edgeIDList);
    route.PushBackVertex(vertexIdList);
    og.AddGraph(route);

    Graph* re = og.GetGraphPtr(1111);
    TopoSort topoSort(&og);
    topoSort.Run();

    for (int i = 0; i < re->GetEdgeListSize(); i++) {
      TopoNode* node = re->GetTopoNodePtr(i);
      if (node->GetID() == 222) {
        ASSERT_TRUE(node->GetFather() == 0 || node->GetFather() == 1);
        ASSERT_EQ(-1, node->GetState());
      } else if (node->GetID() == 333) {
        TopoNode* node1 = re->GetTopoNodePtr(i + 1);
        ASSERT_TRUE(node->GetFather() == 0 || node->GetFather() == 1);
        ASSERT_EQ(-2, node->GetState());
        ASSERT_EQ(444, node1->GetID());
        ASSERT_EQ(i, node1->GetFather());
        ASSERT_EQ(-1, node1->GetState());
      } else if (node->GetID() == 444) {
        TopoNode* node1 = re->GetTopoNodePtr(i - 1);
        ASSERT_EQ(i - 1, node->GetFather());
        ASSERT_EQ(-1, node->GetState());
        ASSERT_EQ(333, node1->GetID());
        ASSERT_TRUE(node1->GetFather() == 0 || node1->GetFather() == 1);
        ASSERT_EQ(-2, node1->GetState());
      } else if (node->GetID() == 555) {
        ASSERT_TRUE(node->GetFather() == 3 || node->GetFather() == 4);
        ASSERT_EQ(-2, node->GetState());
      } else if (node->GetID() == 666) {
        ASSERT_TRUE(node->GetFather() == 3 || node->GetFather() == 4);
        ASSERT_EQ(-1, node->GetState());
      }
    }
  }
  {
    Network og;
    // edges
    deque<deque<ID>> ndLists = {{1, 2}, {2, 3}, {4, 3}, {4, 5}, {6, 5}};
    deque<ID> edgeIDsIni = {111, 222, 333, 444, 555};
    // route
    deque<TopoNode> edgeList;
    deque<ID> edgeIDs = {111, 222, 333, 444, 333, 222, 333, 444, 555};
    deque<int> states = {-2, -2, -1, -4, -2, -3, -1, -2, -1};
    deque<ID> nodeIdList = {2, 4, 6};

    for (int i = 0; i < ndLists.size(); i++) {
      Edge edge(edgeIDsIni[i]);
      edge.InsertVertex(ndLists[i]);
      og.AddEdge(edge);
    }
    for (int i = 0; i < edgeIDs.size(); i++) {
      edgeList.push_back(TopoNode(edgeIDs[i], states[i]));
    }
    Graph route(1111);
    route.PushBackEdge(edgeList);
    route.PushBackVertex(nodeIdList);
    og.AddGraph(route);

    Graph* re = og.GetGraphPtr(1111);
    TopoSort topoSort(&og);
    topoSort.Run();

    deque<ID> edgeIDList = {111, 222, 333, 444, 555};
    deque<int> stateList = {-2, -2, -1, -2, -1};
    deque<size_t> fatherList = {0, 0, 1, 2, 3};
    for (int i = 0; i < re->GetEdgeListSize(); i++) {
      TopoNode* node = re->GetTopoNodePtr(i);
      ASSERT_EQ(fatherList[i], node->GetFather());
      ASSERT_EQ(stateList[i], node->GetState());
      ASSERT_EQ(edgeIDList[i], node->GetID());
    }
  }
  {
    Network og;
    // edges
    deque<deque<ID>> ndLists = {{2, 1}, {2, 3}, {4, 3}, {4, 5}, {6, 5}};
    deque<ID> edgeIDsIni = {111, 222, 333, 444, 555};
    // route
    deque<TopoNode> edgeList;
    deque<ID> edgeIDs = {111, 222, 333, 444, 333, 222, 333, 444, 555};
    deque<int> states = {-2, -2, -2, -2, -2, -2, -2, -2, -2};
    deque<ID> nodeIdList = {2, 4, 6};

    for (int i = 0; i < ndLists.size(); i++) {
      Edge edge(edgeIDsIni[i]);
      edge.InsertVertex(ndLists[i]);
      og.AddEdge(edge);
    }
    for (int i = 0; i < edgeIDs.size(); i++) {
      edgeList.push_back(TopoNode(edgeIDs[i], states[i]));
    }
    Graph route(1111);
    route.PushBackEdge(edgeList);
    route.PushBackVertex(nodeIdList);
    og.AddGraph(route);

    Graph* re = og.GetGraphPtr(1111);
    TopoSort topoSort(&og);
    topoSort.Run();

    deque<ID> edgeIDList = {111, 222, 333, 444, 555};
    deque<int> stateList = {-1, -2, -1, -2, -1};
    deque<size_t> fatherList = {0, 0, 1, 2, 3};
    for (int i = 0; i < re->GetEdgeListSize(); i++) {
      TopoNode* node = re->GetTopoNodePtr(i);
      ASSERT_EQ(fatherList[i], node->GetFather());
      ASSERT_EQ(stateList[i], node->GetState());
      ASSERT_EQ(edgeIDList[i], node->GetID());
    }
  }
}

