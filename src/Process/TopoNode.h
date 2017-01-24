// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_PROCESS_TOPONODE_H_
#define SRC_PROCESS_TOPONODE_H_

#include "../OSMBasic.h"

class TopoNode : public BasicType<ID> {
 public:
  /*
   * constructor
   *
   * @param id
   * @param state
   * @param father
   * @return
   */
  TopoNode(const ID& id, const int& state, const size_t& father = 0);

  /*
   * get the value of father
   *
   * @param
   * @return the value of father
   */
  size_t GetFather() const;

  /*
   * get the state
   *
   * @param
   * @return state
   */
  int GetState() const;

  /*
   * change the state
   *
   * @param state: new state
   * @return
   */
  void ChangeState(const int& state);

  /*
   * assign the father with a given value
   *
   * @param index: new father value
   * @return
   */
  void AssignFather(const size_t& index);

 private:
  // indicates the father's index in the edge list
  size_t father_;

  // indicates the current edge is in which form of state: inverse, normal...
  int state_;
};

// Inline methods
// _____________________________________________________________________________
TopoNode::TopoNode(const ID& id, const int& state, const size_t& father) {
  id_ = id;
  state_ = state;
  father_ = father;
}

// _____________________________________________________________________________
size_t TopoNode::GetFather() const {
  return father_;
}

// _____________________________________________________________________________
int TopoNode::GetState() const {
  return state_;
}

// _____________________________________________________________________________
void TopoNode::ChangeState(const int& state) {
  state_ = state;
}

// _____________________________________________________________________________
void TopoNode::AssignFather(const size_t& index) {
  father_ = index;
}

#endif  // SRC_PROCESS_TOPONODE_H_
