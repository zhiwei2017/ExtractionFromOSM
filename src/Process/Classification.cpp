// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#include <gtest/gtest_prod.h>
#include <list>
#include <string>
#include <unordered_map>
#include <limits>
#include "./Classification.h"

using std::string;
using std::list;
using std::unordered_map;

// _____________________________________________________________________________
Classification::Classification(Network* nPtr) {
  nPtr_ = nPtr;

  // get first 25 prime numbers for the alphabet
  int num(0);
  for (int i = 2; num < 156; i++) {
    int count(0);
    for (int j = 2; j < i / 2 + 1; j++) {
      if (i % j == 0) break;
      count++;
    }
    if (count == i / 2 - 1) {
      charHashPrimeNr[num++] = i;
    }
  }
}

// _____________________________________________________________________________
double Classification::Score(string str) {
  char* endPtr;
  char* temp(strtok_r(&str[0], " -_,.;:/+(\t", &endPtr));
  double result(1);

  if (temp == NULL) return 0;

  string pre(temp);
  for (int i = 0; i < pre.size(); i++) {
    int index;
    if (pre[i] > 64 && pre[i] < 91 && i < 6) {
      index = (pre[i] - 65) * 6 + i;
    } else if (pre[i] > 96 && pre[i] < 123 && i < 6) {
      index = (pre[i] - 97) * 6 + i;
    } else {
      if (pre[i] <= 47 || pre[i] >= 58) {
        result = std::numeric_limits<double>::max();
      }
      break;
    }
    result *= charHashPrimeNr[index];
  }
  return result;
}

// _____________________________________________________________________________
template<typename T>
void Classification::AssignType(Graph* graphPtr, T type, bool replace) {
  string graphType(type > 0 ? "long_distance" : "regional");
  if (graphPtr->ContainAttribute("service")) {
    if (!replace) {
      graphType += ";";
      graphType += graphPtr->GetAttribute("service");
    }
    graphPtr->ChangeAttribute("service", graphType);
  } else {
    graphPtr->AddToAttributes("service", graphType);
  }
}

// _____________________________________________________________________________
double Classification::GetEdgeLength(Edge* edgePtr) {
  return CalculateLength(nPtr_->GetLine(edgePtr));
}

// _____________________________________________________________________________
double Classification::GetGraphLength(Graph* gPtr) {
  double length(0);
  unordered_set<ID> edgeIds;
  for (int i = 0; i < gPtr->GetEdgeListSize(); i++) {
    ID edgeID = gPtr->GetEdgeID(i);

    if (edgeIds.count(edgeID)) continue;
    edgeIds.insert(edgeID);

    Edge* edgePtr = nPtr_->GetEdgePtr(edgeID);
    length += GetEdgeLength(edgePtr);
  }
  return length * (1 + static_cast<double>(gPtr->GetDeletedEdgeNum())
                   / edgeIds.size());
}

// _____________________________________________________________________________
void Classification::CheckTrainType(DIMap* longDistanceMapPtr,
                                    DIMap* localMapPtr,
                                    list<TrainingNode>* trainingNodes,
                                    list<Graph*>* labeled) {
  char* endPtr;
  for (auto it = nPtr_->GetGraphMapBegin();
       it != nPtr_->GetGraphMapEnd(); it++) {
    Graph* graphPtr = &it->second;

    // if don't have the tag service
    if (!graphPtr->ContainAttribute("service")) {
      unknownGraphs_.push_back(graphPtr);
      continue;
    }

    // if has the tag, but not contain any useful infor
    char* temp = strtok_r(&graphPtr->GetAttribute("service")[0], ";",
                          &endPtr);
    if (temp == NULL) {
      unknownGraphs_.push_back(graphPtr);
      continue;
    }

    // if contain the tag and has come infor, but belongs to neither
    string trainType(temp);
    int label(-1);
    if (LongDistanceKeywords.count(trainType)) {
      label = 1;
    } else if (!LocalKeywords.count(trainType)) {
      unknownGraphs_.push_back(graphPtr);
      continue;
    }

    double length = GetGraphLength(graphPtr);

    double score = graphPtr->ContainAttribute("ref");
    if (score) {
      score = Score(graphPtr->GetAttribute("ref"));
      DIMap* map = label < 0 ? localMapPtr : longDistanceMapPtr;
      if (map->count(score)) {
        map->at(score)++;
      } else {
        map->emplace(score, 1);
      }
    }
    labeled->emplace_back(graphPtr);
    trainingNodes->emplace_back(label, score, length);
  }
}

// _____________________________________________________________________________
void Classification::JointMap(DIMap* longDistanceMapPtr, DIMap* localMapPtr,
                              DIMap* jointMapPtr) {
  // build the refMapType_ map
  auto it = longDistanceMapPtr->begin();
  while (it != longDistanceMapPtr->end()) {
    int label = 1;
    // don't contain 1 maybe
    if (it->first == 1) {
      ++it;
      continue;
    }
    if (localMapPtr->count(it->first)) {
      double proportion = it->second/(localMapPtr->at(it->first) + it->second);
      if (proportion <= 0.05) {
        label = -1;
      } else if (proportion < 0.95) {
        ++it;
        continue;
      }
      localMapPtr->erase(it->first);
    }
    jointMapPtr->emplace(it->first, label);
    it = longDistanceMapPtr->erase(it);
  }

  it = localMapPtr->begin();
  while (it != localMapPtr->end()) {
    if (longDistanceMapPtr->count(it->first)) {
      ++it;
      continue;
    }
    jointMapPtr->emplace(it->first, -1);
    it = localMapPtr->erase(it);
  }
}

// _____________________________________________________________________________
void Classification::AdjustTraining(DIMap* jointMapPtr,
                                 list<TrainingNode>* trainingNodes,
                                 list<Graph*>* labeledGraphs) {
  char* endPtr;
  for (auto it = trainingNodes->begin();
       it != trainingNodes->end(); ++it) {
    Graph* graphPtr = labeledGraphs->front();
    labeledGraphs->pop_front();
    if (!jointMapPtr->count(it->score)) continue;
    if (it->label != jointMapPtr->at(it->score)) {
      it->label = jointMapPtr->at(it->score);
      string s = it->label < 0 ? "long_distance" : "regional";
      char* temp = strtok_r(&graphPtr->GetAttribute("service")[0], ";",
                            &endPtr);
      string attr(temp);
      AssignType(graphPtr, it->label, s == attr);
    }
  }
}

// _____________________________________________________________________________
void Classification::AdjustPredict(DIMap* jointMapPtr,
                                   list<TrainingNode>* trainingNodes) {
  while (unknownGraphs_.size() > 0) {
    Graph* graphPtr = unknownGraphs_.front();
    unknownGraphs_.pop_front();
    TrainingNode tn = trainingNodes->front();
    trainingNodes->pop_front();

    int label(tn.label);
    if (jointMapPtr->count(tn.score)) label = jointMapPtr->at(tn.score);
    AssignType(graphPtr, label);
  }
  jointMapPtr->clear();
}

// _____________________________________________________________________________
void Classification::Pretraining(DIMap* jointMapPtr,
                                 list<TrainingNode>* trainingNodes) {
  // do the first around predict according to the score
  auto it = unknownGraphs_.begin();
  while (it != unknownGraphs_.end()) {
    Graph* graphPtr = *it;
    if (!graphPtr->ContainAttribute("ref")) {
      ++it;
      continue;
    }

    double score = Score(graphPtr->GetAttribute("ref"));
    if (!jointMapPtr->count(score)) {
      ++it;
      continue;
    }

    // add it into the training
    double length = GetGraphLength(graphPtr);

    AssignType(graphPtr, jointMapPtr->at(score));
    trainingNodes->emplace_back(jointMapPtr->at(score), score, length);

    it = unknownGraphs_.erase(it);
  }
  jointMapPtr->clear();
}


// _____________________________________________________________________________
void Classification::SetParam(bool probability) {
  // default values
  param.svm_type = C_SVC;
  param.kernel_type = RBF;
  param.degree = 3;
  param.gamma = 3.0517578125e-05;  // 1/num_features
  param.coef0 = 0;
  param.nu = 0.5;
  param.cache_size = 100;
  param.C = 2;
  param.eps = 1e-3;
  param.p = 0.1;
  param.shrinking = 1;
  param.probability = probability;
  param.nr_weight = 0;
  param.weight_label = NULL;
  param.weight = NULL;

//  param.weight_label = Malloc(int, param.nr_weight);
//  param.weight_label[0] = 1;
//  param.weight_label[1] = -1;
//  param.weight = Malloc(double, param.nr_weight);
//  param.weight[0] = 5;
//  param.weight[1] = 1;
}

// _____________________________________________________________________________
void Classification::Training(list<TrainingNode>* trainingNodes) {
  struct svm_problem prob;
  prob.l = trainingNodes->size();
  prob.y = Malloc(double, prob.l);
  prob.x = Malloc(struct svm_node* , prob.l);
  struct svm_node* x_space = Malloc(struct svm_node, prob.l * 2);

  for (size_t i(0), j(0); i < prob.l; i++) {
    TrainingNode tn = trainingNodes->front();
    trainingNodes->pop_front();

    prob.x[i] = &x_space[j];
    prob.y[i] = tn.label;
    x_space[j].index = 1;
    x_space[j++].value = tn.length;
    x_space[j++].index = -1;
  }
  trainingNodes->clear();

  const char* error_msg;
  error_msg = svm_check_parameter(&prob, &param);
  if (error_msg) {
    fprintf(stderr, "ERROR: %s\n", error_msg);
    exit(1);
  }
  model = svm_train(&prob, &param);
  free(prob.y);
  free(prob.x);
  free(x_space);
}

// _____________________________________________________________________________
void Classification::Predict(DIMap* longDistanceMapPtr, DIMap* localMapPtr,
                             list<TrainingNode>* trainingNodes,
                             bool probability) {
  struct svm_node* x = Malloc(struct svm_node, 2);
  double* prob_estimates = NULL;
  if (probability) prob_estimates = Malloc(double, 2);

  // do second around predict using SVM
  auto it = unknownGraphs_.begin();
  while (it != unknownGraphs_.end()) {
    Graph* graphPtr = *it;
    double length = GetGraphLength(graphPtr);

    double predict_label;
    x[0].index = 1;
    x[0].value = length;
    x[1].index = -1;
    if (probability) {
      predict_label = svm_predict_probability(model, x, prob_estimates);
    } else {
      predict_label = svm_predict(model, x);
    }
    if (!graphPtr->ContainAttribute("ref")) {
      AssignType(graphPtr, predict_label);
      it = unknownGraphs_.erase(it);
      continue;
    }

    double score = Score(graphPtr->GetAttribute("ref"));
    int label = predict_label;
    trainingNodes->emplace_back(label, score, length);
    DIMap* mapPtr = predict_label < 0 ? localMapPtr : longDistanceMapPtr;
    if (mapPtr->count(score)) {
      mapPtr->at(score)++;
    } else {
      mapPtr->emplace(score, 1);
    }
    ++it;
  }
  if (probability) free(prob_estimates);

  svm_free_and_destroy_model(&model);
  svm_destroy_param(&param);
  free(x);
}

// _____________________________________________________________________________
void Classification::Run() {
  DIMap localMap, longDistanceMap, jointMap;
  list<TrainingNode> trainingNodes;
  list<Graph*> labeledGraphs;

  CheckTrainType(&longDistanceMap, &localMap, &trainingNodes, &labeledGraphs);

  JointMap(&longDistanceMap, &localMap, &jointMap);

  AdjustTraining(&jointMap, &trainingNodes, &labeledGraphs);

  Pretraining(&jointMap, &trainingNodes);

  SetParam();

  Training(&trainingNodes);

  Predict(&longDistanceMap, &localMap, &trainingNodes);

  JointMap(&longDistanceMap, &localMap, &jointMap);

  localMap.clear();
  longDistanceMap.clear();

  AdjustPredict(&jointMap, &trainingNodes);
}
