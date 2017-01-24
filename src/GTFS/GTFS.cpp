// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include "./GTFS.h"
#include <vector>
#include <utility>
#include <string>

using std::to_string;
using std::vector;
using std::pair;
using std::make_pair;

// _____________________________________________________________________________
GTFS::GTFS(Network* nPtr) {
  nPtr_ = nPtr;
  num_ = 10000;
}

// _____________________________________________________________________________
bool GTFS::IsStation(Vertex* vertexPtr) const {
  string value = vertexPtr->GetAttribute("public_transport");
  return StationKeywords.count(value);
}

// _____________________________________________________________________________
void GTFS::BuildRtree() {
  for (auto it = nPtr_->GetVertexMapBegin();
       it != nPtr_->GetVertexMapEnd(); it++) {
    Vertex* vertexPtr = &it->second;
    if (IsStation(vertexPtr)) {
      // add station vertex into rtree
      GTFSValue value = make_pair(vertexPtr->GetCoordinate(),
                                  vertexPtr->GetID());
      geometryRTree_.insert(value);

      Stop stop(vertexPtr);

      if (vertexPtr->ContainAttribute("uic_ref")) {
        string uicRef = vertexPtr->GetAttribute("uic_ref");
        stop.AddParentStation(uicRef);
        if (uicRefs_.count(uicRef)) {
          stops_.at(uicRef).AddChildCoordinate(vertexPtr->GetCoordinate());
        } else {
          Stop parentStation(vertexPtr, uicRef);
          stops_.emplace(uicRef, parentStation);
        }
        // insert uic_ref into a set of uic_ref
        uicRefs_.insert(uicRef);
      }
      stops_.emplace(to_string(vertexPtr->GetID()), stop);
    }
  }
}

// _____________________________________________________________________________
void GTFS::EqualStationCheck(Vertex* vertexPtr) {
  GeoCoordinate vertexCoord = vertexPtr->GetCoordinate();
  auto nearest = geometryRTree_.qbegin(bgi::nearest(vertexCoord, 2));
  if (nearest->second == vertexPtr->GetID()) {
    nearest++;
  }

  if (CalculateDistance(nearest->first, vertexCoord) > MaxStationLength) return;

  Stop* queryStop = &stops_.at(to_string(vertexPtr->GetID()));
  Stop* nearestStop = &stops_.at(to_string(nearest->second));
  string uicRef = nearestStop->GetParentStation();
  if (uicRef.empty()) {
    // generate a new uic_ref
    uicRef = GenerateUicRef();

    // add the uic_ref into these two stops
    queryStop->AddParentStation(uicRef);
    nearestStop->AddParentStation(uicRef);

    // add the uis_ref in the set of uic_ref
    uicRefs_.insert(uicRef);

    // create the stop for this uic_ref and add it into the stops_ map
    Stop parentStation(vertexPtr, uicRef);
    parentStation.AddChildCoordinate(nearest->first);
    stops_.emplace(uicRef, parentStation);
  } else {
    queryStop->AddParentStation(uicRef);
    stops_.at(uicRef).AddChildCoordinate(vertexCoord);
  }
}

// _____________________________________________________________________________
string GTFS::GenerateRouteName(Stop* first, Stop* last) const {
  return first->GetName() + " - " + last->GetName();
}

// _____________________________________________________________________________
string GTFS::GenerateUicRef() {
  string uicRef = to_string(++num_);
  while (uicRefs_.count(uicRef)) {
    uicRef = to_string(++num_);
  }
  uicRefs_.insert(uicRef);
  return uicRef;
}

// _____________________________________________________________________________
void GTFS::TransformIntoGTFS() {
  int numShape = 1;
  for (auto it = nPtr_->GetGraphMapBegin();
       it != nPtr_->GetGraphMapEnd(); it++) {
    Graph* gPtr = &it->second;
    if (!gPtr->GetEdgeListSize()) {
      continue;
    }

    Route groute(gPtr);
    vector<Railway> railways = Separate(gPtr);
    int numTrip(1), longestTrip(0);
    for (int index = 0; index < railways.size(); index++) {
      vector<TopoNode*> edges = railways.at(index).GetEdges();
      if (edges.size() >= railways.at(longestTrip).GetEdgesAmount()) {
        longestTrip = index;
      }
      for (int j = 0; j < edges.size(); j++) {
        ID edgeID = edges[j]->GetID();
        Edge* edge = nPtr_->GetEdgePtr(edgeID);
        int edgeState = edges[j]->GetState();
        if (edgeState == Normal) {
          for (int i = (j != 0); i < edge->GetVertexListSize(); i++) {
            ID vertexID = edge->GetVertexID(i);
            Vertex* vertexPtr = nPtr_->GetVertexPtr(vertexID);
            railways.at(index).AddCoordinate(vertexPtr->GetCoordinate());

            // check if the current vertex is station or not
            if (IsStation(vertexPtr)) {
              Stop* stop = &stops_.at(to_string(vertexPtr->GetID()));
              string uicRef = stop->GetParentStation();
              if (uicRef.empty()) EqualStationCheck(vertexPtr);
              railways.at(index).AddStop(stop);
            }
          }
        } else {
          for (int i = (j == 0 ? edge->GetVertexListSize() - 1
                       : edge->GetVertexListSize() - 2); i >= 0; i--) {
            ID vertexID = edge->GetVertexID(i);
            Vertex* vertexPtr = nPtr_->GetVertexPtr(vertexID);
            railways.at(index).AddCoordinate(vertexPtr->GetCoordinate());

            // check if the current vertex is station or not
            if (IsStation(vertexPtr)) {
              Stop* stop = &stops_.at(to_string(vertexPtr->GetID()));
              string uicRef = stop->GetParentStation();
              if (uicRef.empty()) EqualStationCheck(vertexPtr);
              railways.at(index).AddStop(stop);
            }
          }
        }
      }
      Trip trip(railways.at(index), numTrip++, index);
      Shape shape(railways.at(index), numShape++);
      trip.ChangeShapeID(shape.GetID());
      trips_.push_back(trip);
      shapes_.push_back(shape);
      // generate some same trips but with different times
      int amount = numTrip;
      for (; numTrip < amount + 23; numTrip++) {
        Trip tripDup(trip.GetRouteID(), trip.GetShapeID(), trip.GetStops(),
                     numTrip);
        tripDup.AddTimes(trip.GetArrivalTimes(), trip.GetDepartureTimes(),
                         numTrip - amount + 1);
        trips_.push_back(tripDup);
      }
    }

    if (groute.GetName() == "") {
      string name("DB Name-Uknown Train");
      if (railways.at(longestTrip).GetStopsAmount()) {
        pair<StopPtrListIte, StopPtrListIte> tails
            = railways.at(longestTrip).GetStops();
        name = GenerateRouteName(*tails.first, *(tails.second - 1));
      }
      groute.ChangeName(name);
    }
    routes_.push_back(groute);
  }
}

// _____________________________________________________________________________
vector<Railway> GTFS::Separate(Graph* gPtr) {
  string trainType = gPtr->GetAttribute("service");
  bool routetype = LongDistanceKeywords.count(trainType);

  vector<Railway> result;
  vector<TopoNode*> trip({gPtr->GetTopoNodePtr(0)});
  for (int i = 1; i < gPtr->GetEdgeListSize(); i++) {
    TopoNode* edge = gPtr->GetTopoNodePtr(i);
    size_t father = edge->GetFather();
    if (father != i - 1 && i < gPtr->GetEdgeListSize() - 1) {
      Railway railway(gPtr->GetID(), trip, routetype);
      result.push_back(railway);
      // delete the TopoNodes after the father index
      ID fatherID = gPtr->GetEdgeID(father);
      int j(0);
      while (j < trip.size()) {
        if (trip[j]->GetID() == fatherID) {
          break;
        }
        j++;
      }
      trip.erase(trip.begin() + j + 1, trip.end());
    }
    trip.push_back(edge);
  }
  Railway railway(gPtr->GetID(), trip, routetype);
  result.push_back(railway);
  return result;
}

// _____________________________________________________________________________
void GTFS::GenerateFeed() {
  // write agency.txt
  CSVWriter csvAgency(FileType::AgencyFile);
  csvAgency.WriteAgency();
  csvAgency.Close();

  // write calendar.txt
  CSVWriter csvCalendar(FileType::CalendarFile);
  csvCalendar.WriteCalendar();
  csvCalendar.Close();

  // write feed_info.txt
  CSVWriter csvFeedInfo(FileType::FeedInfoFile);
  csvFeedInfo.WriteFeedInfo();
  csvFeedInfo.Close();

  // write routes.txt
  CSVWriter csvRoute(FileType::RoutesFile);
  for (int i = 0; i < routes_.size(); i++) {
    csvRoute.WriteRoute(routes_[i]);
  }
  csvRoute.Close();

  // write stops.txt
  CSVWriter csvStop(FileType::StopsFile);
  for (auto it = stops_.begin(); it != stops_.end(); it++) {
    csvStop.WriteStop(it->second);
  }
  csvStop.Close();

  // write shapes.txt
  CSVWriter csvShape(FileType::ShapesFile);
  for (int i = 0; i < shapes_.size(); i++) {
    csvShape.WriteShape(shapes_[i]);
  }
  csvShape.Close();

  // write stop_times.txt and trips.txt
  CSVWriter csvStopTime(FileType::StopTimesFile);
  CSVWriter csvTrip(FileType::TripsFile);
  for (int i = 0; i < trips_.size(); i++) {
    csvStopTime.WriteStopTime(trips_[i]);
    csvTrip.WriteTrip(trips_[i]);
  }
  csvStopTime.Close();
  csvTrip.Close();
}

// _____________________________________________________________________________
void GTFS::Run() {
  BuildRtree();
  TransformIntoGTFS();
  GenerateFeed();
}
