// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#ifndef SRC_PROCESS_PATHFINDING_H_
#define SRC_PROCESS_PATHFINDING_H_

#include <gtest/gtest_prod.h>
#include <unordered_map>
#include <vector>
#include <deque>
#include <utility>
#include "./Process.h"

using std::pair;
using std::deque;
using std::unordered_map;
using std::vector;

/* this class is mainly responsible for path finding algorithms */
class PathFinding : public Process {
 public:
  /*
   * find the neighbors of the given MegaNode
   *
   * @param current: current MegaNode
   * @return a list of MegaNode
   */
  vector<MegaNode> FindNeighbors(const MegaNode& current);
  FRIEND_TEST(FindNeighboursTest, FindNeighbours);

  /*
   * compute the complete path for bidirectional A*
   *
   * @param source: source-MegaNode's ID
   * @param start: last MegaNode's ID in the half path of source part
   * @param target: target-MegaNode's ID
   * @param end: last MegaNode's ID in the half path of target part
   * @param cameFromS
   * @param cameFromT
   * @return a list of TopoNode
   */
  deque<TopoNode> ComputePath(const ID& source, const ID& start,
                              const ID& target, const ID& end,
                              unordered_map<ID, ID> cameFromS,
                              unordered_map<ID, ID> cameFromT);

  /*
   * compute the complete path for A* and Dijkstra's algorithm
   *
   * @param start: source-MegaNode's ID
   * @param end: last MegaNode's ID in the path
   * @param cameFromS
   * @param cameFromT
   * @return a list of TopoNode
   */
  deque<TopoNode> ComputePath(const ID& start, const ID& end,
                              unordered_map<ID, ID> cameFrom);

  /*
   * bidirectional A*
   *
   * @param source
   * @param target
   * @return a list of TopoNode
   */
  deque<TopoNode> BidirectionalAStar(const MegaNode& source,
                                     const MegaNode& target);

  /*
   * A*
   *
   * @param source
   * @param target
   * @return a list of TopoNode
   */
  deque<TopoNode> AStar(const MegaNode& source,
                        const MegaNode& target);

  /*
   * Dijkstra's algorithm
   *
   * @param source
   * @param target
   * @return a list of TopoNode
   */
  deque<TopoNode> Dijkstra(const MegaNode& source,
                           const MegaNode& target);
};

// _____________________________________________________________________________
vector<MegaNode> PathFinding::FindNeighbors(const MegaNode& current) {
  vector<MegaNode> result;
  // find out all the bounding boxes with the distance smaller than 1.5 meter.
  LineString currentLine = current.GetLine();

  for (auto it = geometryRTree_.qbegin(bgi::nearest(currentLine, 100));
       it != geometryRTree_.qend(); it++) {
    Box box = it->first;
    if (CalculateDistance(currentLine, box) > 1.5) break;

    MegaNode mNode = it->second;
    LineString sightingLine = mNode.GetLine();
    double distance = CalculateDistance(currentLine, sightingLine);
    if (distance > 1.5 || mNode.GetID() == current.GetID()) continue;

    Edge* currentEdgePtr = nPtr_->GetEdgePtr(current.GetID());
    Edge* edgePtr = nPtr_->GetEdgePtr(mNode.GetID());
    if (AreConjoint(currentEdgePtr, edgePtr)) {
      // set the priority value as the distance of linestrings
      mNode.SetPriority(distance);
      result.push_back(mNode);
    }
  }
  return result;
}

// _____________________________________________________________________________
deque<TopoNode> PathFinding::ComputePath(const ID& source, const ID& start,
                                         const ID& target, const ID& end,
                                         unordered_map<ID, ID> cameFromS,
                                         unordered_map<ID, ID> cameFromT) {
  deque<TopoNode> edges;
  ID father = start;
  while (father != source) {
    edges.insert(edges.begin(), TopoNode(father, -2));
    father = cameFromS[father];
  }
  father = end;
  while (father != target) {
    edges.push_back(TopoNode(father, -2));
    father = cameFromT[father];
  }
  return edges;
}

// _____________________________________________________________________________
deque<TopoNode> PathFinding::ComputePath(const ID& start, const ID& end,
                                        unordered_map<ID, ID> cameFrom) {
  deque<TopoNode> edges;
  ID father = end;
  while (father != start) {
    edges.insert(edges.begin(), TopoNode(father, -2));
    father = cameFrom[father];
  }
  return edges;
}

// _____________________________________________________________________________
deque<TopoNode> PathFinding::BidirectionalAStar(const MegaNode& source,
                                                const MegaNode& target) {
  Heap openListS, openListT;
  unordered_map<ID, ID> cameFromS, cameFromT;
  unordered_map<ID, double> costSoFarS, costSoFarT;

  openListS.push(source);
  cameFromS.emplace(source.GetID(), source.GetID());
  costSoFarS.emplace(source.GetID(), 0);

  openListT.push(target);
  cameFromT.emplace(target.GetID(), target.GetID());
  costSoFarT.emplace(target.GetID(), 0);

  // to track the closest edge, so we can output a path if gap is not fixable
  pair<ID, ID> nearestPair(source.GetID(), target.GetID());
  double dis(CalculateDistance(source.GetLine(), target.GetLine()));
  pair<double, double> minDis(dis, dis);

  while (openListS.size() || openListT.size()) {
    if (openListS.size()) {
      MegaNode current = openListS.top();
      openListS.pop();
      for (auto neighbor : FindNeighbors(current)) {
        if (cameFromT.count(neighbor.GetID())) {
          return ComputePath(source.GetID(), current.GetID(),
                             target.GetID(), neighbor.GetID(),
                             cameFromS, cameFromT);
        }
        if (cameFromS.count(neighbor.GetID())) continue;

        double newCost = costSoFarS[current.GetID()]
                        + neighbor.GetPriority()
                        + CalculateLength(neighbor.GetLine());
        // if this neighbor is already visited and it has a lower cost
        if (costSoFarS.count(neighbor.GetID())
            && newCost > costSoFarS[neighbor.GetID()]) continue;

        double distanceToEnd = CalculateDistance(neighbor.GetLine(),
                                                 target.GetLine());
        double priority = newCost + distanceToEnd;

        costSoFarS[neighbor.GetID()] = newCost;
        neighbor.SetPriority(priority);
        openListS.push(neighbor);
        cameFromS.emplace(neighbor.GetID(), current.GetID());
        // keep in mind that the closest edge to the end edge
        if (distanceToEnd >= minDis.first) continue;
        nearestPair.first = neighbor.GetID();
        minDis.first = distanceToEnd;
      }
    }
    if (openListT.size()) {
      MegaNode current = openListT.top();
      openListT.pop();
      for (auto neighbor : FindNeighbors(current)) {
        if (cameFromS.count(neighbor.GetID())) {
          return ComputePath(source.GetID(), neighbor.GetID(),
                             target.GetID(), current.GetID(),
                             cameFromS, cameFromT);
        }
        if (cameFromT.count(neighbor.GetID())) continue;

        double newCost = costSoFarT[current.GetID()]
                        + neighbor.GetPriority()
                        + CalculateLength(neighbor.GetLine());
        // if this neighbor is already visited and it has a lower cost
        if (costSoFarT.count(neighbor.GetID())
            && newCost > costSoFarT[neighbor.GetID()]) continue;

        double distanceToBegin = CalculateDistance(neighbor.GetLine(),
                                                   source.GetLine());
        double priority = newCost + distanceToBegin;

        costSoFarT[neighbor.GetID()] = newCost;
        neighbor.SetPriority(priority);
        openListT.push(neighbor);
        cameFromT.emplace(neighbor.GetID(), current.GetID());
        // aledges keep in mind that the closest edge to the end edge
        if (distanceToBegin >= minDis.second) continue;
        nearestPair.second = neighbor.GetID();
        minDis.second = distanceToBegin;
      }
    }
  }
  return ComputePath(source.GetID(), nearestPair.first,
                     target.GetID(), nearestPair.second,
                     cameFromS, cameFromT);
}

// _____________________________________________________________________________
deque<TopoNode> PathFinding::AStar(const MegaNode& source,
                                  const MegaNode& target) {
  Heap openList;
  unordered_map<ID, ID> cameFrom;
  unordered_map<ID, double> costSoFar;

  openList.push(source);
  cameFrom.emplace(source.GetID(), source.GetID());
  costSoFar.emplace(source.GetID(), 0);

  // to track the cloest edge, so we can output a path if gap is not fixable
  ID nearestToEnd(source.GetID());
  double minDistanceToEnd = CalculateDistance(source.GetLine(),
                                              target.GetLine());
  double lengthBoundary(minDistanceToEnd * 1.75);

  while (openList.size()) {
    MegaNode current = openList.top();
    openList.pop();
    for (auto neighbor : FindNeighbors(current)) {
      if (neighbor.GetID() == target.GetID()) {
        return ComputePath(source.GetID(), current.GetID(), cameFrom);
      }
      if (cameFrom.count(neighbor.GetID())) continue;

      double newCost = costSoFar[current.GetID()]
                    + neighbor.GetPriority()
                    + CalculateLength(neighbor.GetLine());
      // if this neighbor is already visited and it has a lower cost
      if (newCost > lengthBoundary
          || (costSoFar.count(neighbor.GetID())
              && newCost > costSoFar[neighbor.GetID()])) continue;

      double distanceToEnd = CalculateDistance(neighbor.GetLine(),
                                               target.GetLine());
      double priority = newCost + distanceToEnd;

      costSoFar[neighbor.GetID()] = newCost;
      neighbor.SetPriority(priority);
      openList.push(neighbor);
      cameFrom.emplace(neighbor.GetID(), current.GetID());
      // keep in mind that the closest edge to the end edge
      if (distanceToEnd >= minDistanceToEnd) continue;
      nearestToEnd = neighbor.GetID();
      minDistanceToEnd = distanceToEnd;
    }
  }
  return ComputePath(source.GetID(), nearestToEnd, cameFrom);
}

// _____________________________________________________________________________
deque<TopoNode> PathFinding::Dijkstra(const MegaNode& source,
                                     const MegaNode& target) {
  Heap openList;
  unordered_map<ID, ID> cameFrom;
  unordered_map<ID, double> costSoFar;

  openList.push(source);
  cameFrom.emplace(source.GetID(), source.GetID());
  costSoFar.emplace(source.GetID(), 0);

  // to track the cloest edge, so we can output a path if gap is not fixable
  ID nearestToEnd(source.GetID());
  double minDistanceToEnd = CalculateDistance(source.GetLine(),
                                              target.GetLine());
  double lengthBoundary(minDistanceToEnd * 1.75);

  while (openList.size()) {
    MegaNode current = openList.top();
    openList.pop();
    for (auto neighbor : FindNeighbors(current)) {
      if (neighbor.GetID() == target.GetID()) {
        return ComputePath(source.GetID(), current.GetID(), cameFrom);
      }
      if (cameFrom.count(neighbor.GetID())) continue;

      double newCost = costSoFar[current.GetID()]
                    + neighbor.GetPriority()
                    + CalculateLength(neighbor.GetLine());
      // if this neighbor is already visited and it has a lower cost
      if (newCost > lengthBoundary
          || (costSoFar.count(neighbor.GetID())
              && newCost > costSoFar[neighbor.GetID()])) continue;

      costSoFar[neighbor.GetID()] = newCost;
      neighbor.SetPriority(newCost);
      openList.push(neighbor);
      cameFrom.emplace(neighbor.GetID(), current.GetID());
      // aledges keep in mind that the closest edge to the end edge
      double distanceToEnd = CalculateDistance(neighbor.GetLine(),
                                               target.GetLine());
      if (distanceToEnd >= minDistanceToEnd) continue;
      nearestToEnd = neighbor.GetID();
      minDistanceToEnd = nearestToEnd;
    }
  }
  return ComputePath(source.GetID(), nearestToEnd, cameFrom);
}

#endif  // SRC_PROCESS_PATHFINDING_H_
