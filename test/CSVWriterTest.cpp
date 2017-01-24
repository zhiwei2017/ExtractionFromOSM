// Copyright 2016, University of Freiburg,
// Author: Zhiwei Zhang <zhang.zhiwei@outlook.de>.

#include <gtest/gtest.h>
#include <string>
#include "../src/GTFS/CSVWriter.h"


using std::string;

// _____________________________________________________________________________
TEST(StrPreprocessTest, StrPreprocess) {
  CSVWriter csv;
  string testString1 = "chemnitz-hilbersdorf - niederwiesa, gleis 2";
  string testString2 = "Basel SBB, Freiburg, Offenburg, Karlsruhe Hbf (\"Rheintalbahn\")";
  ASSERT_EQ("\"chemnitz-hilbersdorf - niederwiesa, gleis 2\"", csv.StrPreprocess(testString1));
  ASSERT_EQ("\"Basel SBB, Freiburg, Offenburg, Karlsruhe Hbf (\"\"Rheintalbahn\"\")\"", csv.StrPreprocess(testString2));
}
