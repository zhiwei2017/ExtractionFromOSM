// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_PROCESS_CLASSIFICATION_H_
#define SRC_PROCESS_CLASSIFICATION_H_

#include <gtest/gtest_prod.h>
#include <list>
#include <string>
#include <unordered_map>
#include <limits>
#include "../../svm.h"
#include "./Process.h"
#define Malloc(type, n) reinterpret_cast<type*>(malloc((n) * sizeof(type)))
#define DIMap unordered_map<double, int>

using std::string;
using std::list;
using std::unordered_map;

struct TrainingNode {
  TrainingNode(int l, double sc, double len) {
    label = l;
    score = sc;
    length = len;
  }

  int label;
  double score;
  double length;
};

/* this class is mainly responsible for the classification of graph */
class Classification : public Process {
 public:
  explicit Classification(Network* nPtr);

  double Score(string str);

  template<typename T>
  void AssignType(Graph* graphPtr, T type, bool replace = false);

  double GetEdgeLength(Edge* edgePtr);

  double GetGraphLength(Graph* gPtr);

  void CheckTrainType(DIMap* longDistanceMapPtr, DIMap* localMapPtr,
                      list<TrainingNode>* trainingNodes,
                      list<Graph*>* labeled);

  void JointMap(DIMap* longDistanceMapPtr,
                DIMap* localMapPtr, DIMap* jointMapPtr);

  void AdjustTraining(DIMap* jointMapPtr,
                      list<TrainingNode>* trainingNodes,
                      list<Graph*>* labeledGraphs);

  void AdjustPredict(DIMap* jointMapPtr, list<TrainingNode>* trainingNodes);

  void Pretraining(DIMap* jointMapPtr, list<TrainingNode>* trainingNodes);

  void SetParam(bool probability = false);

  void Training(list<TrainingNode>* trainingNodes);

  void Predict(DIMap* longDistanceMaPtrp, DIMap* localMapPtr,
               list<TrainingNode>* trainingNodes,
               bool probability = false);

  void Run();

 private:
  list<Graph*> unknownGraphs_;

  int charHashPrimeNr[156];

  struct svm_parameter param;
  struct svm_model* model;
};

#endif  // SRC_PROCESS_CLASSIFICATION_H_
