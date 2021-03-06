/*!
  \file        gtest_phs_pplm.cpp
  \author      Arnaud Ramey <arnaud.a.ramey@gmail.com>
                -- Robotics Lab, University Carlos III of Madrid
  \date        2014/2/3

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
Some tests for PHSPPLM
 */
#include "people_recognition_vision/pplm_testing.h"
#include "people_recognition_vision/phs_pplm.h"

TEST(TestSuite, create) {
  if (!vision_utils::rosmaster_alive()) return;
  PHSPPLM matcher;
}

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, test_sizes) {
  if (!vision_utils::rosmaster_alive()) return;
  PHSPPLM matcher;
  vision_utils::test_sizes(matcher);
}

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, test_same_msg) {
  if (!vision_utils::rosmaster_alive()) return;
  PHSPPLM matcher;
  for (unsigned int nusers = 0; nusers < 10; ++nusers)
    vision_utils::test_same_msg(matcher, nusers, 1E-1);
}

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, test_same_msg_david_arnaud) {
  if (!vision_utils::rosmaster_alive()) return;
  PHSPPLM matcher;
  for (unsigned int nusers = 0; nusers < 10; ++nusers)
    vision_utils::test_same_msg(matcher, nusers, 1E-1, vision_utils::IMG_DIR() +  "depth/david_arnaud1");
  for (unsigned int nusers = 0; nusers < 10; ++nusers)
    vision_utils::test_same_msg(matcher, nusers, 1E-1, vision_utils::IMG_DIR() +  "depth/david_arnaud2");
}

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, test_two_frames_matching) {
  if (!vision_utils::rosmaster_alive()) return;
  PHSPPLM matcher;
  vision_utils::test_two_frames_matching
      (matcher, vision_utils::IMG_DIR() +  "depth/david_arnaud1", vision_utils::IMG_DIR() +  "depth/david_arnaud2");
  vision_utils::test_two_frames_matching
      (matcher, vision_utils::IMG_DIR() +  "depth/david_arnaud1", vision_utils::IMG_DIR() +  "depth/david_arnaud3");
  vision_utils::test_two_frames_matching
      (matcher, vision_utils::IMG_DIR() +  "depth/david_arnaud2", vision_utils::IMG_DIR() +  "depth/david_arnaud3");
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv){
  ros::init(argc, argv, "gtest_PHSPPLM");
  // Run all the tests that were declared with TEST()
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
