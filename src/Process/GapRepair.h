// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#ifndef SRC_PROCESS_GAPREPAIR_H_
#define SRC_PROCESS_GAPREPAIR_H_

#include <gtest/gtest_prod.h>
#include <list>
#include <unordered_map>
#include "./PathFinding.h"

using std::list;
using std::unordered_map;

typedef list<int>::iterator GapsIte;

/*
 * each gap represents all the gap in one route
 */
class Gap : public BasicType<ID> {
 public:
  /*
   * constructor
   *
   * @param id : graph id
   * @return
   */
  explicit Gap(const ID& id);

  /*
   * constructor
   *
   * @param id : route id
   * @param gapPos: index where gap shows
   * @return
   */
  Gap(const ID& id, const int& gapPos);

  /*
   * get the offset variable, which represent the offset for the rest gaps
   * after adding some edges to fix the front gap
   *
   * @param
   * @return offset
   */
  int GetOffset() const;

  /*
   * increase the offset by the given size
   *
   * @param size : increment of offset
   * @return
   */
  void UpdateOffset(const int& size);

  /*
   * get the size of the Gaps in one route
   *
   * @param
   * @return size
   */
  size_t GetSize() const;

  /*
   * add the gap position into the gap list
   *
   * @param gapPos: index where gap shows
   * @return
   */
  void PushBackGap(const int& gapPos);

  /*
   * get the the begin iterator
   *
   * @param
   * @return begin iterator
   */
  GapsIte GetGapListBegin();

  /*
   * get the end iterator
   *
   * @param
   * @return end iterator
   */
  GapsIte GetGapListEnd();

  /*
   * delete the given position items in gap list
   *
   * @param
   * @return the next position after deleting
   */
  GapsIte DeleteGap(GapsIte pos);

 private:
  // store the indexes where gaps show
  list<int> gaps_;

  // offset for the positions of gaps
  int offset_;
};

/* this class is mainly responsible for repairing the gaps */
class GapRepair : public PathFinding {
 public:
  /*
   * constructor
   *
   * @param nPtr: a pointer of network
   * @return
   */
  explicit GapRepair(Network* nPtr);

  /*
   * do the gap check for all the graphs
   *
   * @param
   * @return
   */
  void GapCheck();
  FRIEND_TEST(GapCheckTest, GapCheck);

  /*
   * build a R-Tree data structure and store all the edges in
   * the network int it.
   *
   * @param
   * @return
   */
  void BuildRTree();
  FRIEND_TEST(BuildRtreeTest, BuildRtree);

  /*
   * build a R-Tree data structure and store all the edges in
   * the network int it.
   *
   * @param graphId: an id of graph
   * @param gapIndex: index where gap shows
   * @param gapSet: record which gap is fixed
   * @return number of inserted edges to fix the gap
   */
  int GapFix(const ID& graphId, const int& gapIndex,
             unordered_set<Key, KeyHash, KeyEqual>* gapSet);

  /*
   * fix the gap
   *
   * @param
   * @return
   */
  void Repair();
  FRIEND_TEST(RepairTest, Repair);

  /*
   * get the number of gaps
   *
   * @param
   * @return the number of gaps
   */
  int GetSize() const;

  /*
   * run the gap repair system
   *
   * @param
   * @return
   */
  void Run();

 private:
  // graph ID maps to a list of indexes
  unordered_map<ID, Gap> graphGaps_;

  // count the number of gaps
  int gapNum_;
};

// Inline methods
// _____________________________________________________________________________
Gap::Gap(const ID& id) {
  id_ =  id;
  offset_ = 0;
}

// _____________________________________________________________________________
Gap::Gap(const ID& id, const int& gapPos) {
  id_ =  id;
  offset_ = 0;
  gaps_.push_back(gapPos);
}

// _____________________________________________________________________________
int Gap::GetOffset() const {
  return offset_;
}

// _____________________________________________________________________________
void Gap::UpdateOffset(const int& size) {
  offset_ += size;
}

// _____________________________________________________________________________
size_t Gap::GetSize() const {
  return gaps_.size();
}

// _____________________________________________________________________________
void Gap::PushBackGap(const int& gapPos) {
  gaps_.push_back(gapPos);
}

// _____________________________________________________________________________
GapsIte Gap::GetGapListBegin() {
  return gaps_.begin();
}

// _____________________________________________________________________________
GapsIte Gap::GetGapListEnd() {
  return gaps_.end();
}

// _____________________________________________________________________________
GapsIte Gap::DeleteGap(GapsIte pos) {
  return gaps_.erase(pos);
}

#endif  // SRC_PROCESS_GAPREPAIR_H_
