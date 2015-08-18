/*!
  \file        gtest_euclidean_pplm.cpp
  \author      Arnaud Ramey <arnaud.a.ramey@gmail.com>
                -- Robotics Lab, University Carlos III of Madrid
  \date        2014/2/2

________________________________________________________________________________

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
________________________________________________________________________________
Some tests for EuclideanPPLM
 */
#include "people_utils/pplm_testing.h"
#include "people_recognition_vision/euclidean_pplm.h"
#include "vision_utils/filename_prefix2imgs.h"

TEST(TestSuite, create) {
  if (!rosmaster_alive()) return;
  EuclideanPPLM matcher;
}

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, test_sizes) {
  if (!rosmaster_alive()) return;
  EuclideanPPLM matcher;
  pplm_testing::test_sizes(matcher);
}

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, test_same_msg) {
  if (!rosmaster_alive()) return;
  EuclideanPPLM matcher;
  for (unsigned int nusers = 0; nusers < 10; ++nusers)
    pplm_testing::test_same_msg(matcher, nusers);
}

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, test_two_frames_matching) {
  if (!rosmaster_alive()) return;
  EuclideanPPLM matcher;
  pplm_testing::test_two_frames_matching
      (matcher, IMG_DIR "depth/david_arnaud1", IMG_DIR "depth/david_arnaud2");
  pplm_testing::test_two_frames_matching
      (matcher, IMG_DIR "depth/david_arnaud1", IMG_DIR "depth/david_arnaud3");
  pplm_testing::test_two_frames_matching
      (matcher, IMG_DIR "depth/david_arnaud2", IMG_DIR "depth/david_arnaud3");
}

////////////////////////////////////////////////////////////////////////////////

void test_pplm_benchmark(const std::string & filename_regex) {
  if (!rosmaster_alive()) return;
  EuclideanPPLM matcher;
  FilenamePrefix2Imgs db_player;
  ASSERT_TRUE(db_player.from_file(filename_regex));
  pplm_testing::pplm_benchmark(matcher, db_player, 1);
}

TEST(TestSuite, pplm_benchmark) {
  test_pplm_benchmark(IMG_DIR "breast/*_rgb.png");
  test_pplm_benchmark(IMG_DIR "depth/*_rgb.png");
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv){
  ros::init(argc, argv, "gtest_EuclideanPPLM");
  // Run all the tests that were declared with TEST()
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
