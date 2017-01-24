// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include "./NetworkBuilder.h"
#include <string>
#include <utility>
#include <unordered_set>

using std::pair;
using std::unordered_set;
using std::make_pair;
using std::string;

// _____________________________________________________________________________
NetworkBuilder::NetworkBuilder(Network* networkPtr) {
  nPtr_ = networkPtr;
}

// _____________________________________________________________________________
void NetworkBuilder::StoreRoot(xmlTextReaderPtr reader, xmlChar** name) {
  root_.first = ConvertToC(*name);
  int m = xmlTextReaderAttributeCount(reader);
  for (int i = 0; i < m; i++) {
    xmlTextReaderMoveToAttributeNo(reader, i);
    root_.second.push_back(make_pair(ConvertToC(xmlTextReaderName(reader)),
                                     ConvertToC(xmlTextReaderValue(reader))));
  }
  xmlTextReaderMoveToElement(reader);
  for (int i = 0; i < 2; i++) {
    xmlTextReaderRead(reader);
  }

  xmlNodePtr node;
  node = xmlTextReaderExpand(reader);
  bounds_.first = ConvertToC(node->name);
  xmlAttrPtr attr;
  attr = node->properties;

  // write all the attributes
  while (attr != NULL) {
    bounds_.second.push_back(make_pair(ConvertToC(attr->name),
                                       ConvertToC(attr->children->content)));
    attr = attr->next;
  }
  xmlFreeProp(attr);

  for (int i = 0; i < 2; i++) {
    xmlTextReaderNext(reader);
  }
  *name = xmlTextReaderName(reader);
}

// _____________________________________________________________________________
void NetworkBuilder::WriteRootAndBounds(xmlTextWriterPtr writer) {
  // begin to write the root information
  xmlTextWriterStartElement(writer, ConvertToUC(root_.first.c_str()));

  // write all the attributes
  for (int i = 0; i < root_.second.size(); i++) {
    xmlTextWriterWriteAttribute(writer,
                                ConvertToUC(root_.second[i].first.c_str()),
                                ConvertToUC(root_.second[i].second.c_str()));
    xmlTextWriterEndAttribute(writer);
  }

  // write the "/n"
  xmlTextWriterWriteString(writer, ConvertToUC("\n\t"));

  // begin to write the root information
  xmlTextWriterStartElement(writer, ConvertToUC(bounds_.first.c_str()));

  // write all the attributes
  for (int i = 0; i < bounds_.second.size(); i++) {
    xmlTextWriterWriteAttribute(writer,
                                ConvertToUC(bounds_.second[i].first.c_str()),
                                ConvertToUC(bounds_.second[i].second.c_str()));
    xmlTextWriterEndAttribute(writer);
  }

  xmlTextWriterEndElement(writer);
}

// _____________________________________________________________________________
void NetworkBuilder::WriteAttr(xmlNodePtr node, xmlTextWriterPtr writer) {
  xmlAttrPtr attr;
  attr = node->properties;

  // write all the attributes
  while (attr != NULL) {
    xmlTextWriterWriteAttribute(writer,
                                attr->name,
                                attr->children->content);
    xmlTextWriterEndAttribute(writer);
    attr = attr->next;
  }
  xmlFreeProp(attr);
}

// _____________________________________________________________________________
int NetworkBuilder::WriteNode(xmlTextReaderPtr reader,
                              xmlTextWriterPtr writer) {
  xmlNodePtr node;
  node = xmlTextReaderExpand(reader);

  xmlTextWriterStartElement(writer, node->name);
  // write all the attributes
  WriteAttr(node, writer);
  if (xmlTextReaderIsEmptyElement(reader)) {
    xmlTextWriterEndElement(writer);
    return 0;
  }
  node = node->children;
  node = node->next;
  while (node != NULL) {
    xmlTextWriterWriteString(writer, ConvertToUC("\n\t\t"));
    xmlTextWriterStartElement(writer, node->name);
    WriteAttr(node, writer);
    xmlTextWriterEndElement(writer);
    node = node->next;
    node = node->next;
  }
  xmlFreeNode(node);
  xmlTextWriterWriteString(writer, ConvertToUC("\n\t"));
  xmlTextWriterEndElement(writer);
  return 1;
}

// _____________________________________________________________________________
void NetworkBuilder::CreateFile(xmlTextReaderPtr reader, xmlChar** name,
                         const char* nodeName) {
  xmlTextWriterPtr writer;
  string fileName = nodeName;
  fileName += ".osm";

  writer = xmlNewTextWriterFilename(fileName.c_str(), 0);
  xmlTextWriterStartDocument(writer, "1.0", "UTF-8", NULL);
  fileList_.push_back(fileName);

  WriteRootAndBounds(writer);

  while (xmlStrEqual(*name, ConvertToUC(nodeName))) {
    xmlTextWriterWriteString(writer, ConvertToUC("\n\t"));
    WriteNode(reader, writer);
    xmlTextReaderNext(reader);
    xmlTextReaderNext(reader);
    *name = xmlTextReaderName(reader);
  }
  xmlTextWriterWriteString(writer, ConvertToUC("\n"));
  xmlTextWriterFullEndElement(writer);
  xmlTextWriterEndDocument(writer);
  xmlFreeTextWriter(writer);
}

// _____________________________________________________________________________
bool NetworkBuilder::IsRelatedRelation(xmlTextReaderPtr reader) {
  if (xmlTextReaderIsEmptyElement(reader)) {
    return false;
  }
  xmlNodePtr node;
  node = xmlTextReaderExpand(reader);
  node = node->children;
  while (node != NULL) {
    const xmlChar* key = xmlGetProp(node, ConvertToUC("k"));
    const xmlChar* value = xmlGetProp(node, ConvertToUC("v"));
    if (xmlStrEqual(key, ConvertToUC("route"))
        && (xmlStrEqual(value, ConvertToUC("tracks"))
        || xmlStrEqual(value, ConvertToUC("railway"))
        || xmlStrEqual(value, ConvertToUC("train"))
        || xmlStrEqual(value, ConvertToUC("rail"))
        || xmlStrEqual(value, ConvertToUC("monorail"))
        || xmlStrEqual(value, ConvertToUC("light_rail")))) {
      return true;
    }
    node = node->next;
  }
  xmlFreeNode(node);
  return false;
}

// _____________________________________________________________________________
void NetworkBuilder::ProcessRelation(xmlTextReaderPtr reader) {
  // to check if the route is a track, abandoned or historic.
  unordered_set<string> keyValueSet;

  xmlNodePtr node;
  if (!IsRelatedRelation(reader)) return;

  node = xmlTextReaderExpand(reader);
  ID id = ConvertToID(xmlGetProp(node, ConvertToUC("id")));
  Graph graph(id);
  node = node->children;
  node = node->next;
  while (node != NULL) {
    if (xmlStrEqual(node->name, ConvertToUC("member"))) {
      const unsigned char* type = xmlGetProp(node, ConvertToUC("type"));
      const unsigned char* role = xmlGetProp(node, ConvertToUC("role"));
      if (xmlStrEqual(type, ConvertToUC("way"))
          && !xmlStrEqual(role, ConvertToUC("plattform"))) {
        ID edgeId = ConvertToID(xmlGetProp(node, ConvertToUC("ref")));
        graph.PushBackEdge(edgeId);
        nPtr_->AddEdge(edgeId);
      } else if (xmlStrEqual(type, ConvertToUC("node"))) {
        ID vertexId = ConvertToID(xmlGetProp(node, ConvertToUC("ref")));
        graph.PushBackVertex(vertexId);
        nPtr_->AddVertex(vertexId);
      }
    } else {
      string key = ConvertToStr(xmlGetProp(node, ConvertToUC("k")));
      string value = ConvertToStr(xmlGetProp(node, ConvertToUC("v")));
      key = GetLowercase(key);
      if (!OriginalStr.count(key)) {
        value = GetLowercase(value);
      }
      graph.AddToAttributes(key, value);
      keyValueSet.insert(key);
      keyValueSet.insert(value);
    }
    node = node->next;
    node = node->next;
  }

  string graphType = graph.GetAttribute("route");
  if (!graphType.compare("monorail") || !graphType.compare("light_rail")) {
    if (graph.ContainAttribute("service")) {
      string local("regional;");
      local += graph.GetAttribute("service");
      graph.ChangeAttribute("service", local);
    } else {
      graph.AddToAttributes("service", "regional");
    }
  }
  // if a graph don't contain any ways (it could be a route master)
  // then we don't need to add it into the map, because we couldn't generate
  // a GTFS feed for it anyway
  if (graph.GetEdgeListSize()
      && graphType.compare("tracks")
      && !keyValueSet.count("abandoned")
      && !keyValueSet.count("historic")) {
    nPtr_->AddGraph(graph);
  }
}

// _____________________________________________________________________________
bool NetworkBuilder::IsRelatedWay(xmlTextReaderPtr reader, const ID& id) {
  xmlNodePtr node;
  node = xmlTextReaderExpand(reader);
  node = node->children;
  node = node->next;
  while (node != NULL) {
    const unsigned char* key = xmlGetProp(node, ConvertToUC("k"));
    const unsigned char* value = xmlGetProp(node, ConvertToUC("v"));
    const unsigned char* subvalue = xmlStrsub(value, xmlStrlen(value) - 5, 4);
    if (xmlStrEqual(key, ConvertToUC("railway"))
        && (xmlStrEqual(subvalue, ConvertToUC("rail"))
            || xmlStrEqual(value, ConvertToUC("narrow_gauge")))) {
          nPtr_->AddEdge(id);
          return true;
        }
    node = node->next;
    node = node->next;
  }
  return false;
}

// _____________________________________________________________________________
void NetworkBuilder::ProcessWay(xmlTextReaderPtr reader) {
  xmlNodePtr node;
  node = xmlTextReaderExpand(reader);

  ID id = ConvertToID(xmlGetProp(node, ConvertToUC("id")));
  if (!nPtr_->ContainEdge(id) && !IsRelatedWay(reader, id)) return;
  node = node->children;
  node = node->next;
  while (node != NULL && xmlStrEqual(node->name, ConvertToUC("nd"))) {
    ID vertexId = ConvertToID(xmlGetProp(node, ConvertToUC("ref")));
    nPtr_->GetEdgePtr(id)->PushBackVertex(vertexId);
    nPtr_->AddVertex(vertexId);
    node = node->next;
    node = node->next;
  }
  ID firstVertexID = nPtr_->GetEdgePtr(id)->GetFrom();
  ID lastVertexID = nPtr_->GetEdgePtr(id)->GetTo();
  nPtr_->GetVertexPtr(firstVertexID)->AddToAdjacenyList(id);
  nPtr_->GetVertexPtr(lastVertexID)->AddToAdjacenyList(id);
}

// _____________________________________________________________________________
void NetworkBuilder::ExtendWay(xmlTextReaderPtr reader) {
  xmlNodePtr node;
  node = xmlTextReaderExpand(reader);

  ID id = ConvertToID(xmlGetProp(node, ConvertToUC("id")));
  if (nPtr_->ContainEdge(id)) return;
  Edge edge(id);

  node = node->children;
  node = node->next;
  while (node != NULL && xmlStrEqual(node->name, ConvertToUC("nd"))) {
    ID vertexId = ConvertToID(xmlGetProp(node, ConvertToUC("ref")));
    if (!nPtr_->ContainVertex(vertexId)) return;
    edge.PushBackVertex(vertexId);
    node = node->next;
    node = node->next;
  }
  ID firstVertexID = edge.GetFrom();
  ID lastVertexID = edge.GetTo();
  nPtr_->GetVertexPtr(firstVertexID)->AddToAdjacenyList(id);
  nPtr_->GetVertexPtr(lastVertexID)->AddToAdjacenyList(id);

  nPtr_->AddEdge(edge);
}

// _____________________________________________________________________________
void NetworkBuilder::ProcessNode(xmlTextReaderPtr reader) {
  xmlNodePtr node;
  node = xmlTextReaderExpand(reader);

  ID id = ConvertToID(xmlGetProp(node, ConvertToUC("id")));
  if (!nPtr_->ContainVertex(id)) return;

  double lat = ConvertToDouble(xmlGetProp(node, ConvertToUC("lat")));
  double lon = ConvertToDouble(xmlGetProp(node, ConvertToUC("lon")));
  nPtr_->GetVertexPtr(id)->SetCoord(lat, lon);

  if (xmlTextReaderIsEmptyElement(reader)) {
    return;
  }

  node = node->children;
  node = node->next;
  while (node != NULL) {
    string key = ConvertToStr(xmlGetProp(node, ConvertToUC("k")));
    string value = ConvertToStr(xmlGetProp(node, ConvertToUC("v")));
    nPtr_->GetVertexPtr(id)->AddToAttributes(GetLowercase(key),
                                         GetLowercase(value));
    node = node->next;
    node = node->next;
  }
}

// _____________________________________________________________________________
void NetworkBuilder::ExtendNode(xmlTextReaderPtr reader,
                                const int& extendRadius) {
  xmlNodePtr node;
  node = xmlTextReaderExpand(reader);

  ID id = ConvertToID(xmlGetProp(node, ConvertToUC("id")));
  if (nPtr_->ContainVertex(id)) return;

  double lat = ConvertToDouble(xmlGetProp(node, ConvertToUC("lat")));
  double lon = ConvertToDouble(xmlGetProp(node, ConvertToUC("lon")));
  GeoCoordinate gc(lat, lon);
  auto it = geometryRTree_.qbegin(bgi::nearest(gc, 1));
  Segment seg = *it;
  double distance = CalculateDistance(seg, gc);
  if (distance > extendRadius) return;
  Vertex vertex(id, gc);

  if (xmlTextReaderIsEmptyElement(reader)) {
    nPtr_->AddVertex(vertex);
    return;
  }

  node = node->children;
  node = node->next;
  while (node != NULL) {
    string key = ConvertToStr(xmlGetProp(node, ConvertToUC("k")));
    string value = ConvertToStr(xmlGetProp(node, ConvertToUC("v")));
    vertex.AddToAttributes(GetLowercase(key), GetLowercase(value));
    node = node->next;
    node = node->next;
  }
  nPtr_->AddVertex(vertex);
}

// _____________________________________________________________________________
void NetworkBuilder::ReadRelation(const char* fileName) {
  // read file pointer
  xmlTextReaderPtr reader;
  xmlChar* name;

  reader = xmlNewTextReaderFilename(fileName);
  if (reader == NULL) {
    printf("Unable to open %s\n", fileName);
    return;
  }

  xmlTextReaderRead(reader);

  name = xmlTextReaderName(reader);

  StoreRoot(reader, &name);

  while (xmlStrEqual(name, ConvertToUC("node"))) {
    xmlTextReaderNext(reader);
    xmlTextReaderNext(reader);
    name = xmlTextReaderName(reader);
  }

  CreateFile(reader, &name, "way");

  while (xmlStrEqual(name, ConvertToUC("relation"))) {
    ProcessRelation(reader);
    xmlTextReaderNext(reader);
    xmlTextReaderNext(reader);
    name = xmlTextReaderName(reader);
  }

  xmlFreeTextReader(reader);
}

// _____________________________________________________________________________
void NetworkBuilder::ReadFile(const char* fileName, const string& typeName,
                       bool extend, const int& extendRadius) {
  // read file pointer
  xmlTextReaderPtr reader;

  reader = xmlNewTextReaderFilename(fileName);
  if (reader == NULL) {
    printf("Unable to open %s\n", fileName);
    return;
  }

  //  jump the root and boundary information
  for (int i = 0; i < 5; i++) {
    xmlTextReaderRead(reader);
  }

  if (!extend && !typeName.compare("relation")) {
    while (xmlStrEqual(xmlTextReaderName(reader), ConvertToUC("relation"))) {
      ProcessRelation(reader);
      xmlTextReaderNext(reader);
      xmlTextReaderNext(reader);
    }
  } else if (!extend && !typeName.compare("way")) {
    while (xmlStrEqual(xmlTextReaderName(reader), ConvertToUC("way"))) {
      ProcessWay(reader);
      xmlTextReaderNext(reader);
      xmlTextReaderNext(reader);
    }
  } else if (!extend && !typeName.compare("node")) {
    while (xmlStrEqual(xmlTextReaderName(reader), ConvertToUC("node"))) {
      ProcessNode(reader);
      xmlTextReaderNext(reader);
      xmlTextReaderNext(reader);
    }
  } else if (extend && !typeName.compare("node")) {
    while (xmlStrEqual(xmlTextReaderName(reader), ConvertToUC("node"))) {
      ExtendNode(reader, extendRadius);
      xmlTextReaderNext(reader);
      xmlTextReaderNext(reader);
    }
  } else if (extend && !typeName.compare("way")) {
    while (xmlStrEqual(xmlTextReaderName(reader), ConvertToUC("way"))) {
      ExtendWay(reader);
      xmlTextReaderNext(reader);
      xmlTextReaderNext(reader);
    }
  }
  xmlFreeTextReader(reader);
}

// _____________________________________________________________________________
void NetworkBuilder::BuildRTree() {
  for (auto it = nPtr_->GetEdgeMapBegin(); it != nPtr_->GetEdgeMapEnd(); it++) {
    Edge* edge = &it->second;
    if (edge->GetVertexListSize() > 0) {
      for (int i = 1; i < edge->GetVertexListSize(); i++) {
        ID vertexId1 = edge->GetVertexID(i - 1);
        ID vertexId2 = edge->GetVertexID(i);
        Vertex* vertex1 = nPtr_->GetVertexPtr(vertexId1);
        Vertex* vertex2 = nPtr_->GetVertexPtr(vertexId2);
        Segment segment(vertex1->GetCoordinate(), vertex2->GetCoordinate());
        geometryRTree_.insert(segment);
      }
    }
  }
}

// _____________________________________________________________________________
void NetworkBuilder::DeleteTempFiles() {
  for (int i = 0; i < fileList_.size(); i++) {
    if (remove(fileList_[i].c_str()) != 0)
      perror("Error deleting file");
    else
      printf("File %s successfully deleted.\n", fileList_[i].c_str());
  }
}

// _____________________________________________________________________________
void NetworkBuilder::Build(const char* fileName, bool originalFile,
                           bool extend, const int& extendRadius) {
  string nodeFile(fileName);
  if (originalFile) {
    ReadRelation(fileName);
  } else {
    string file = fileName;
    if (file != "") {
      fileList_ = {"./" + file + "/way.osm",
        "./" + file + "/node.osm",
        "./" + file + "/relation.osm"};
    } else {
      fileList_ = {"./way.osm", "./node.osm", "./relation.osm"};
    }
    ReadFile(fileList_[2].c_str(), "relation");
    nodeFile = fileList_[1];
  }
  ReadFile(fileList_[0].c_str(), "way");
  ReadFile(nodeFile.c_str(), "node");
  if (extend) {
    BuildRTree();
    ReadFile(nodeFile.c_str(), "node", extend, extendRadius);
    ReadFile(fileList_[0].c_str(), "way", extend);
  }
  if (originalFile) DeleteTempFiles();
}
