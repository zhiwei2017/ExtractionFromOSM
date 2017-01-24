// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#ifndef OSMMAIN_H_
#define OSMMAIN_H_

#include <sys/stat.h>
#include <iostream>
#include "./src/OSMBasic.h"
#include "./src/Process/NetworkBuilder.h"
#include "./src/Process/NetworkBuilder.cpp"
#include "./src/Process/GapRepair.h"
#include "./src/Process/GapRepair.cpp"
#include "./src/Process/OrderRepair.h"
#include "./src/Process/OrderRepair.cpp"
#include "./src/Process/ConnectionRepair.h"
#include "./src/Process/ConnectionRepair.cpp"
#include "./src/Process/TopoSort.h"
#include "./src/Process/Classification.h"
#include "./src/Process/Classification.cpp"
#include "./src/GTFS/GTFS.h"
#include "./src/GTFS/GTFS.cpp"
#include "./src/Timer.h"

using std::cout;
using std::endl;

class OSMMain : public Functions {
 public:
  void Run(const char* fileName, bool originalFile, bool extend,
           const int& extendRadius);

 private:
  Network og;
  Timer t;
};

// Inline methods
// _____________________________________________________________________________
void OSMMain::Run(const char* fileName, bool originalFile, bool extend,
                  const int& extendRadius) {
  NetworkBuilder networkBuilder(&og);

  t.start();
  networkBuilder.Build(fileName, originalFile, extend, extendRadius);
  t.stop();
  cout << "Route Map Size: " << og.GetGraphMapSize() << " " << endl;
  cout << "Edge Map Size: " << og.GetEdgeMapSize() << endl;
  cout << "Node Map Size: " << og.GetVertexMapSize() << endl;
  cout << "Building Route-Map, Edge-Map and Node-Map"
  << (extend? " and extending Edge-Map and Node-Map" : "") << " Using Time: "
  << t.getUSecs() << " microseconds " << endl;

  OrderRepair orderRepair(&og);
  t.start();
  orderRepair.Run();
  t.stop();
  cout << "Ordering Fix Using Time: " << t.getUSecs()
  << " microseconds " << endl;

  GapRepair gapRepair(&og);

  t.start();
  gapRepair.Run();
  t.stop();
  cout << "Gap Fix Using Time: " << t.getUSecs()
  << " microseconds " << endl;

  Classification classification(&og);

  t.start();
  classification.Run();
  t.stop();
  cout << "Distinction Using Time: " << t.getUSecs()
  << " microseconds " << endl;

  ConnectionRepair connectionRepair(&og);

  t.start();
  connectionRepair.Run();
  t.stop();
  cout << "Edge Connection Using Time: " << t.getUSecs()
  << " microseconds " << endl;

  TopoSort topoSort(&og);

  t.start();
  topoSort.Run();
  t.stop();
  cout << "DFS Using Time: " << t.getUSecs() << " microseconds " << endl;

  GTFS gtfs(&og);

  t.start();
  gtfs.Run();
  t.stop();
  cout << "Generate GTFS Fedds Using Time: " << t.getUSecs()
  << " microseconds " << endl;
}

#endif  // OSMMAIN_H_
