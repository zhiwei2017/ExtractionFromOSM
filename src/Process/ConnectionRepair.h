// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#ifndef SRC_PROCESS_CONNECTIONREPAIR_H_
#define SRC_PROCESS_CONNECTIONREPAIR_H_

#include <gtest/gtest_prod.h>
#include <vector>
#include "./Process.h"

using std::vector;

/* this class is mainly responsible for repairing the
 * connections between edges
 */
class ConnectionRepair : public Process {
 public:
  /*
   * constructor
   *
   * @param bIPtr: a pointer of network
   * @return
   */
  explicit ConnectionRepair(Network* nPtr);

  /*
   * we will find two or three available edgeIDs to replace the oldEdgeID
   *
   * @param oldEdgeID: the edgeId will be replaced.
   * @param three: if need to create three new edges
   * @return a list of new edges' IDs
   */
  vector<ID> CreateNewEdges(const ID& oldEdgeID, bool three = false);
  FRIEND_TEST(CreateNewEdgesTest, CreateNewEdges);

  /*
   * replace the oldedge by two new edges
   *
   * @param graphPtr: pointer of graph
   * @param index: position of the separated edge
   * @param newState
   * @return
   */
  void SeparateEdge(Graph* graphPtr, const int& index,
                   const int& newState);
  FRIEND_TEST(SeparateEdgeTest, SeparateEdge);

  /*
   * replace the oldedge by three new edges
   *
   * @param graphPtr: pointer of graph
   * @param index: position of the separated edge
   * @param newState
   * @return
   */
  void SeparateEdgeThree(Graph* graphPtr, const int& index,
                        const int& newState);
  FRIEND_TEST(SeparateEdgeThreeTest, SeparateEdgeThree);

  /*
   * according to the conjoint situation repair the front edge
   *
   * @param graphPtr: pointer of graph
   * @param index: position of the to be repaired edge
   * @param i: conjoint position with the back edge
   * @param lastItemIndex: index of the last node in front edge
   * @return how many edges are inserted. 
   */
  int RepairFontEdge(Graph* graphPtr, const int& index,
                     const int& conjointPos, const int& lastItemIndex);

  /*
   * according to the conjoint situation repair the back edge
   *
   * @param graphPtr: pointer of graph
   * @param index: position of the to be repaired edge
   * @param i: conjoint position with the front edge
   * @param lastItemIndex: index of the last node in back edge
   * @param increment: count how many edges are inserted at the 
   *                   position of front edge
   * @return how many edges are inserted.
   *         if -1: two edges are not conjoint.
   */
  int RepairBackEdge(Graph* graphPtr, const int& index,
                     const int& conjointPos, const int& lastItemIndex,
                     const int& increment);

  /*
   * estimate whether two edges are connected or not.
   *
   * @param graphPtr: pointer of graph
   * @param index: position of the front edge
   * @return true: if the two edges are conjoint
   */
  int Repair(Graph* graphPtr, const int& index);
  FRIEND_TEST(RepairTest, Repair);

  /*
   * repair all edges in all graphs
   *
   * @param
   * @return
   */
  void Run();
};

#endif  // SRC_PROCESS_CONNECTIONREPAIR_H_
