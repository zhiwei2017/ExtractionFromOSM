// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#ifndef SRC_PROCESS_ORDERREPAIR_H_
#define SRC_PROCESS_ORDERREPAIR_H_

#include <gtest/gtest_prod.h>
#include <vector>
#include <deque>
#include "./PathFinding.h"

using std::deque;
using std::vector;

/* this class is mainly responsible for repairing the order of edges in graph */
class OrderRepair : public PathFinding {
 public:
  /*
   * constructor
   *
   * @param nPtr: a pointer of network
   * @return
   */
  explicit OrderRepair(Network* nPtr);

  /*
   * build a r-tree for repairing the order on the basis of all the edges
   * in the given graph
   *
   * @param graphPtr
   * @param edgeListPtr: store the MegaNode of each edge for order check
   * @return
   */
  void BuildRTree(Graph* graphPtr, vector<MegaNode>* edgeListPtr);

  /*
   * build a r-tree for repairing the order on the basis of all the edges
   * in the given graph
   *
   * @param graphPtr
   * @param wrongOrderListPtr
   * @return
   */
  void OrderCheck(Graph* graphPtr, vector<int>* wrongOrderListPtr);
  FRIEND_TEST(OrderCheckTest, OrderCheck);

  /*
   * after repairing the wrong orders, we locate the head edge in this graph
   *
   * @param listPtr
   * @return
   */
  void FindHeadEdge(deque<TopoNode>* listPtr);

  /*
   * repair the order of Edge-ids in a Graph
   *
   * @param graphPtr
   * @param edgeList
   * @return
   */
  size_t Repair(Graph* graphPtr, const vector<MegaNode>& edgeList);
  FRIEND_TEST(OrderRepairTest, OrderRepair);

  /*
   * run the order repair system
   *
   * @param list
   * @return
   */
  void Run();
};

#endif  // SRC_PROCESS_ORDERREPAIR_H_
