// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include <string>
#include <iostream>
#include "./ExtractionFromOSM.h"

using std::cerr;
using std::endl;

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 4) {
    cerr << "\nPlease use the commando format:" << endl;
    cerr << "./OSMMain [Address] [Mode] [ExtendRadius]\n" << endl;
    cerr << "Address: paricular address of osm-file" << endl;
    cerr << "Mode: if use the extend mode (Y/N). The default value is \"N\""
    << endl;
    cerr << "ExtendRadius: if use the extend mode, the nodes and ways are "
         << "loaded into graph when they are in the radius of a route."
         << "The default value is 500"
         << endl;
    return -1;
  }
  string fileName = argv[1];
  bool originalFile = (fileName.size() > 4
                       && fileName.substr(fileName.size() - 4, 4) == ".osm");
  bool extend(false);
  OSMMain osm;
  if (argc > 2) {
    string ex = osm.GetLowercase(argv[2]);
    extend = !ex.compare("y");
  }
  int extendRadius(500);
  if (argc == 4) {
    extendRadius = atoi(argv[3]);
  }
  osm.Run(argv[1], originalFile, extend, extendRadius);
  return 0;
}
