// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_GTFS_TRIP_H_
#define SRC_GTFS_TRIP_H_

#include <string>
#include <vector>
#include <utility>
#include "./Railway.h"

using std::string;
using std::to_string;
using std::vector;
using std::pair;

const int StopInterval = 180;

/* this class corresponds to the trips.txt and 
 * stop_time.txt files in GTFS */
class Trip : public BasicType<string>, public Functions {
 public:
  /*
   * constructor
   *
   * @param railway
   * @param num: sequence number
   * @return
   */
  explicit Trip(Railway railway, const int& num = 1, const int& branchNum = 0);

  /*
   * constructor
   *
   * @param routeID
   * @param shapeID
   * @param stops: store all stations information
   * @param num
   * @return
   */
  Trip(const string& routeID, const string& shapeID,
       vector<Stop*> stops, const int& num = 2);

  /*
   * add departure time and arrival time for each station
   *
   * @param arrivalTimes
   * @param departureTimes
   * @param beginHour
   * @return
   */
  void AddTimes(vector<string> arrivalTimes,
                vector<string> departureTimes, const int& beginHour);

  /*
   * change the corresponding shape id
   *
   * @param shapeID
   * @return
   */
  void ChangeShapeID(const string& shapeID);

  /*
   * get all the stations
   *
   * @param
   * @return list of stations
   */
  vector<Stop*> GetStops() const;

  /*
   * get the corresponding route id
   *
   * @param
   * @return route id
   */
  string GetRouteID() const;

  /*
   * get the corresponding shape id
   *
   * @param
   * @return shape id
   */
  string GetShapeID() const;

  /*
   * get service id
   *
   * @param
   * @return service id
   */
  string GetServiceID() const;

  /*
   * get arival time lise
   *
   * @param
   * @return list of arrival time
   */
  vector<string> GetArrivalTimes() const;

  /*
   * get departure time lise
   *
   * @param
   * @return list of departure time
   */
  vector<string> GetDepartureTimes() const;

  /*
   * transform the time in second to time in string of form 
   * second/minute/hour
   *
   * @param time
   * @return time in form of string
   */
  string ConvertToTime(const int& secondSum);

  /*
   * calculate the time
   * we
   * @param time
   * @param beginHour
   * @return athe value of begin time plus run time
   */
  string IncreaseHour(const string& time, const int& beginHour);

 private:
  // which route this trip belongs to
  string routeID_;

  // the service id
  string serviceID_;

  // its shape id
  string shapeID_;

  // the stop list of this trip
  vector<Stop*> stops_;

  // a list of arrival time for each stop
  vector<string> arrivalTimes_;

  // a list of departure time for each stop
  vector<string> departureTimes_;
};

// Inline methods
// _____________________________________________________________________________
Trip::Trip(Railway railway, const int& num, const int& branchNum) {
  routeID_ = railway.GetID();
  id_ = routeID_ + "-" + to_string(num);
  serviceID_ = "1";
  pair<StopPtrListIte, StopPtrListIte> tails = railway.GetStops();
  stops_.assign(tails.first, tails.second);

  arrivalTimes_.push_back(ConvertToTime(100 * branchNum));
  departureTimes_.push_back(ConvertToTime(100 * branchNum + StopInterval));
  int arrival(0), depature(StopInterval);
  int i = 1;
  while (i < stops_.size()) {
    double distance = CalculateDistance(stops_[i - 1]->GetCoordinate(),
                                        stops_[i]->GetCoordinate());
    if (distance <= 300) {
      stops_.erase(stops_.begin() + i);
      continue;
    }
    int secondSum;
    if (railway.GetRouteType()) {
      secondSum = static_cast<int>(ceil((distance * 3) / 250));
    } else {
      secondSum = static_cast<int>(ceil((distance * 9) / 400));
    }
    arrival += StopInterval + secondSum;
    depature += StopInterval + secondSum;
    arrivalTimes_.push_back(ConvertToTime(arrival));
    departureTimes_.push_back(ConvertToTime(depature));
    i++;
  }
}

// _____________________________________________________________________________
Trip::Trip(const string& routeID, const string& shapeID,
           vector<Stop*> stops, const int& num) {
  routeID_ = routeID;
  id_ = routeID_ + "-" + to_string(num);
  shapeID_ = shapeID;
  serviceID_ = "1";
  stops_.assign(stops.begin(), stops.end());
}

// _____________________________________________________________________________
void Trip::AddTimes(vector<string> arrivalTimes,
                    vector<string> departureTimes, const int& beginHour) {
  for (int i = 0; i < arrivalTimes.size(); i++) {
    arrivalTimes_.push_back(IncreaseHour(arrivalTimes[i], beginHour));
    departureTimes_.push_back(IncreaseHour(departureTimes[i], beginHour));
  }
}

// _____________________________________________________________________________
void Trip::ChangeShapeID(const string& shapeID) {
  shapeID_ = shapeID;
}

// _____________________________________________________________________________
vector<Stop*> Trip::GetStops() const {
  return stops_;
}

// _____________________________________________________________________________
string Trip::GetRouteID() const {
  return routeID_;
}

// _____________________________________________________________________________
string Trip::GetShapeID() const {
  return shapeID_;
}

// _____________________________________________________________________________
string Trip::GetServiceID() const {
  return serviceID_;
}

// _____________________________________________________________________________
vector<string> Trip::GetArrivalTimes() const {
  return arrivalTimes_;
}

// _____________________________________________________________________________
vector<string> Trip::GetDepartureTimes() const {
  return departureTimes_;
}

// _____________________________________________________________________________
string Trip::ConvertToTime(const int& secondSum) {
  string result;
  int hour = secondSum / 3600;
  int minute = (secondSum - hour * 3600) / 60;
  int second = secondSum - hour * 3600 - minute * 60;
  result = (hour > 9 ? "" : "0") + to_string(hour) + ":"
  + (minute > 9 ? "" : "0") + to_string(minute) + ":"
  + (second > 9 ? "" : "0") + to_string(second);
  return result;
}

// _____________________________________________________________________________
string Trip::IncreaseHour(const string& time, const int& beginHour) {
  size_t pos = time.find_first_of(":");
  string result = time.substr(pos);
  string hourStr = time.substr(0, pos);
  int hour = atoi(hourStr.c_str()) + beginHour;
  result = (hour > 9 ? "" : "0") + to_string(hour) + result;
  return result;
}

#endif  // SRC_GTFS_TRIP_H_
