// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#ifndef SRC_PROCESS_NETWORKBUILDER_H_
#define SRC_PROCESS_NETWORKBUILDER_H_

#include <gtest/gtest.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlwriter.h>
#include <utility>
#include <string>
#include <vector>
#include "../OSMBasic.h"
#include "../Network/Network.h"
#include "../Network/Network.cpp"

using std::string;
using std::pair;
using std::vector;

/* this class is mainly responsible for building the network from osm file*/
class NetworkBuilder : public Functions {
 public:
  /*
   * constructor
   *
   * @param networkPtr : pointer of network
   * @return
   */
  explicit NetworkBuilder(Network* networkPtr);

  /*
   * read the root and boundary information and store them
   *
   * @param reader
   * @param name
   * @return
   */
  void StoreRoot(xmlTextReaderPtr reader, xmlChar** name);

  /*
   * write the root and boundary information in a new osm file
   *
   * @param writer
   * @return
   */
  void WriteRootAndBounds(xmlTextWriterPtr writer);

  /*
   * write the attribute information
   *
   * @param node
   * @param writer
   * @return
   */
  void WriteAttr(xmlNodePtr node, xmlTextWriterPtr writer);

  /*
   * write the whole xml node information include its children nodes information
   *
   * @param reader
   * @param writer
   * @return
   */
  int WriteNode(xmlTextReaderPtr reader, xmlTextWriterPtr writer);

  /*
   * create a new file and write the information in it
   *
   * @param reader
   * @param name
   * @param nodeName
   * @return
   */
  void CreateFile(xmlTextReaderPtr reader,
                  xmlChar** name, const char* nodeName);

  /*
   * check whether the current relation is related to train or not.
   *
   * @param reader
   * @return
   */
  bool IsRelatedRelation(xmlTextReaderPtr reader);

  /*
   * process the relation: if it's train related, load its information
   *                        into network; otherwise ignore it
   *
   * @param reader
   * @return
   */
  void ProcessRelation(xmlTextReaderPtr reader);

  /*
   * estimate whether the current way is related to train or not
   *
   * the estimate standard: if the tag k="railway" and v="Xrail" exists then
   * it is related to railway
   *
   * @param reader
   * @param id: id of the way
   * @return true: if the current edge node is related to railway
   */
  bool IsRelatedWay(xmlTextReaderPtr reader, const ID& id);

  /*
   * process the way: if it's train related, load its information
   *                        into network; otherwise ignore it
   *
   * @reader : the xml reader pointer
   * @return
   */
  void ProcessWay(xmlTextReaderPtr reader);

  /*
   * extend the network with the railway-around ways' information
   *
   * @param reader
   * @return
   */
  void ExtendWay(xmlTextReaderPtr reader);

  /*
   * process the node: if it's included in any train-related relation or 
   *                      way, load its information into network;
   *                      otherwise ignore it
   *
   * @param reader
   * @return
   */
  void ProcessNode(xmlTextReaderPtr reader);

  /*
   * extend the network with the railway-around nodes' information
   *
   * @param reader
   * @param extendRadius: the radius
   * @return
   */
  void ExtendNode(xmlTextReaderPtr reader, const int& extendRadius);

  /*
   * pass the nodes, create a way.osm for ways and process all relations
   *
   * @param fileName
   * @return
   */
  void ReadRelation(const char* fileName);

  /*
   * pass the nodes, create a way.osm for ways and process all relations
   *
   * @param fileName
   * @return
   */
  void ReadFile(const char* fileName, const string& typeName,
                bool extend = false, const int& extendRadius = 500);

  /*
   * build a r-tree for detecting whether a node is in the radius
   *
   * @param
   * @return
   */
  void BuildRTree();

  /*
   * delete the created temp files
   *
   * @param
   * @return
   */
  void DeleteTempFiles();

  /*
   * build the network
   *
   * @param fileName
   * @param originalFile
   * @param extend
   * @param extendRadius
   * @return
   */
  void Build(const char* fileName, bool originalFile,
             bool extend, const int& extendRadius);
  FRIEND_TEST(BuildTest, Build);

 private:
  Network* nPtr_;

  bgi::rtree<Segment, bgi::linear<16> > geometryRTree_;

  // store all the files' name which are creaeted
  vector<string> fileList_;
  // store the root and boundary information
  pair<string, vector<pair<string, string> > > root_;
  pair<string, vector<pair<string, string> > > bounds_;
};

#endif  // SRC_PROCESS_NETWORKBUILDER_H_
