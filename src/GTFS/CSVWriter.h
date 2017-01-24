// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>

#ifndef SRC_GTFS_CSVWRITER_H_
#define SRC_GTFS_CSVWRITER_H_

#include <gtest/gtest_prod.h>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <vector>
#include "./Route.h"
#include "./Shape.h"
#include "./Trip.h"

using std::string;
using std::to_string;
using std::vector;
using std::ofstream;

enum FileType {
  StopsFile = 1,
  StopTimesFile = 2,
  ShapesFile = 3,
  TripsFile = 4,
  RoutesFile = 5,
  AgencyFile = 6,
  CalendarFile = 7,
  FeedInfoFile = 8,
  NoFile = 9
};

/* CSVWriter for outputting GTFS feeds */
class CSVWriter {
 public:
  /*
   * constructor
   *
   * @param fileType: which file needs to be generated
   * @return
   */
  explicit CSVWriter(const FileType& fileType = NoFile);

  /*
   * close the writer
   *
   * @param
   * @return
   */
  void Close();

  /*
   * how to output a list of string
   *
   * @param strList
   * @return
   */
  void WriteStrList(const vector<string>& strList);

  /*
   * write routes.txt
   *
   * @param route
   * @return
   */
  void WriteRoute(Route route);

  /*
   * write shapes.txt
   *
   * @param shape
   * @return
   */
  void WriteShape(Shape shape);

  /*
   * write stops.txt
   *
   * @param stop
   * @return
   */
  void WriteStop(Stop stop);

  /*
   * write stop_times.txt
   *
   * @param trip
   * @return
   */
  void WriteStopTime(Trip trip);

  /*
   * write trips.txt
   *
   * @param trip
   * @return
   */
  void WriteTrip(Trip trip);

  /*
   * write the agency.txt
   *
   * @param
   * @return
   */
  void WriteAgency();

  /*
   * write calendar.txt
   *
   * @param
   * @return
   */
  void WriteCalendar();

  /*
   * write feed_info.txt
   *
   * @param
   * @return
   */
  void WriteFeedInfo();

  /*
   * defines the output form
   *
   * @param str
   * @return
   */
  string StrPreprocess(const string& str) const;
  FRIEND_TEST(StrPreprocessTest, StrPreprocess);

 private:
  ofstream file_;
};

// Inline methods
// _____________________________________________________________________________
CSVWriter::CSVWriter(const FileType& fileType) {
  if (fileType < NoFile) {
    system("mkdir -p ./GTFSFeeds");
    string fileName;
    vector<string> titles;
    switch (fileType) {
      case StopsFile:
        fileName = "./GTFSFeeds/stops.txt";
        titles = {"stop_id", "stop_name", "stop_lat", "stop_lon",
                  "location_type", "parent_station"};
        break;

      case StopTimesFile:
        fileName = "./GTFSFeeds/stop_times.txt";
        titles = {"trip_id", "arrival_time", "departure_time", "stop_id",
                   "stop_sequence"};
        break;

      case ShapesFile:
        fileName = "./GTFSFeeds/shapes.txt";
        titles = {"shape_id", "shape_pt_lat", "shape_pt_lon",
                   "shape_pt_sequence"};
        break;

      case TripsFile:
        fileName = "./GTFSFeeds/trips.txt";
        titles = {"route_id", "service_id", "trip_id", "shape_id"};
        break;

      case RoutesFile:
        fileName = "./GTFSFeeds/routes.txt";
        titles = {"route_id", "agency_id", "route_short_name",
                  "route_long_name", "route_type", "route_color"};
        break;

      case AgencyFile:
        fileName = "./GTFSFeeds/agency.txt";
        titles = {"agency_id", "agency_name", "agency_url",
                  "agency_timezone", "agency_phone", "agency_lang"};
        break;

      case CalendarFile:
        fileName = "./GTFSFeeds/calendar.txt";
        titles = {"service_id", "monday", "tuesday", "wednesday",
                  "thursday", "friday", "saturday", "sunday",
                  "start_date", "end_date"};
        break;

      case FeedInfoFile:
        fileName = "./GTFSFeeds/feed_info.txt";
        titles = {"feed_publisher_name", "feed_publisher_url", "feed_lang",
                  "feed_start_date", "feed_end_date", "feed_version"};
        break;

      default:
        break;
    }
    file_.open(fileName);
    WriteStrList(titles);
  }
}

// _____________________________________________________________________________
void CSVWriter::Close() {
  file_.close();
}

// _____________________________________________________________________________
void CSVWriter::WriteStrList(const vector<string>& strList) {
  for (int i = 0; i < strList.size() - 1; i++) {
    file_ << strList[i] << ",";
  }
  file_ << strList.back() << "\n";
}

// _____________________________________________________________________________
void CSVWriter::WriteRoute(Route route) {
  file_ << route.GetID() << ","
        << route.GetAgencyID() << ","
        << StrPreprocess(route.GetShortName()) << ","
        << StrPreprocess(route.GetName()) << ","
        << route.GetRouteType() << ","
        << route.GetRouteColor() << "\n";
}

// _____________________________________________________________________________
void CSVWriter::WriteShape(Shape shape) {
  string shapeID = shape.GetID();
  vector<string> shapePtLats = shape.GetShapePtLats();
  vector<string> shapePtLons = shape.GetShapePtLons();
  vector<string> shapePtSeq = shape.GetShapePtSequence();
  for (int i = 0; i < shapePtLats.size(); i++) {
    file_ << shapeID << ","
          << shapePtLats[i] << ","
          << shapePtLons[i] << ","
          << shapePtSeq[i] << "\n";
  }
}

// _____________________________________________________________________________
void CSVWriter::WriteStop(Stop stop) {
  file_ << stop.GetID() << ","
        << StrPreprocess(stop.GetName()) << ","
        << stop.GetLat() << ","
        << stop.GetLon() << ","
        << stop.GetLocationType() << ","
        << stop.GetParentStation() << "\n";
}

// _____________________________________________________________________________
void CSVWriter::WriteStopTime(Trip trip) {
  string tripID = trip.GetID();
  vector<Stop*> stops = trip.GetStops();
  vector<string> arrivalTimes = trip.GetArrivalTimes();
  vector<string> departureTimes = trip.GetDepartureTimes();
  for (int i = 0; i < stops.size(); i++) {
    string stopID = stops[i]->GetID();
    file_ << tripID << ","
          << arrivalTimes[i] << ","
          << departureTimes[i] << ","
          << stopID << ","
          << to_string(i + 1) << "\n";
  }
}

// _____________________________________________________________________________
void CSVWriter::WriteTrip(Trip trip) {
  file_ << trip.GetRouteID() << ","
        << trip.GetServiceID() << ","
        << trip.GetID() << ","
        << trip.GetShapeID() << "\n";
}

// _____________________________________________________________________________
void CSVWriter::WriteAgency() {
  file_ << "6866" << ","
        << "Deutsche Bahn AG" << ","
        << "https://www.bahn.com" << ","
        << "Europe/Berlin" << ","
        << "(49) 1806-101111" << ","
        << "en" << "\n";
}

// _____________________________________________________________________________
void CSVWriter::WriteCalendar() {
  file_ << "1" << ","
        << "1" << ","
        << "1" << ","
        << "1" << ","
        << "1" << ","
        << "1" << ","
        << "1" << ","
        << "1" << ","
        << "20150101" << ","
        << "20291231" << "\n";
}

// _____________________________________________________________________________
void CSVWriter::WriteFeedInfo() {
  file_ << "ZhiweiZhang" << ","
        << "http://omnibus.uni-freiburg.de/~zz20/" << ","
        << "en" << ","
        << "20160711" << ","
        << "20291231" << ","
        << "20160711" << "\n";
}

// _____________________________________________________________________________
string CSVWriter::StrPreprocess(const string& str) const {
  string result(str);
  size_t pos1 = str.find_first_of("\"");
  size_t pos2 = str.find_first_of(",");
  if (pos1 == string::npos && pos2 == string::npos) {
    return result;
  } else if (pos1 == string::npos) {
    result = "\"" + result + "\"";
  } else {
    while (pos1 != string::npos) {
      result.insert(pos1, "\"");
      pos1 = result.find_first_of("\"", pos1 + 2);
    }
    result = "\"" + result + "\"";
  }
  return result;
}

#endif  // SRC_GTFS_CSVWRITER_H_
