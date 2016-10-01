/*!
  \file        gtest_ppl_gating.cpp
  \author      Arnaud Ramey <arnaud.a.ramey@gmail.com>
                -- Robotics Lab, University Carlos III of Madrid
  \date        2014/12/3

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

Some tests for ppl_gating namespace
 */
#include <gtest/gtest.h>
#include "vision_utils/ppl_testing.h"
#include "people_recognition_vision/ppl_gating.h"

typedef people_msgs_rl::PeoplePose PP;
typedef people_msgs_rl::PeoplePoseList PPL;
typedef geometry_utils::FooPoint3f Pt3f;

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, gate_pp2tracks) {
  ros::Time track_stamp = ros::Time(100), ppl_stamp = track_stamp + ros::Duration(1); // 1 second after tracks
  PP pp;
  pp.head_pose.position.x = pp.head_pose.position.y = pp.head_pose.position.z = 0;
  pp.header.stamp = ppl_stamp;
  PPL tracks;
  tracks.header.stamp = track_stamp;
  double human_walking_speed = 10;
  ASSERT_TRUE(ppl_gating::gate_pp2tracks(pp, tracks, human_walking_speed) == assignment_utils::UNASSIGNED);

  // test walking speed
  std::vector<Pt3f> pts;
  for (unsigned int x = 0; x <= 2 * human_walking_speed; ++x) {
    pts.clear();
    pts.push_back(Pt3f(x + 0.1, 0, 0));
    pts.push_back(Pt3f(x + 1  , 0, 0));
    pts.push_back(Pt3f(x + 2  , 0, 0));
    ppl_utils::ppl_factory(tracks, pts, 0, track_stamp);
    if (x < human_walking_speed) // in gates
      ASSERT_TRUE(ppl_gating::gate_pp2tracks(pp, tracks, human_walking_speed) == 0);
    else // out of gates
      ASSERT_TRUE(ppl_gating::gate_pp2tracks(pp, tracks, human_walking_speed)
                  == assignment_utils::UNASSIGNED);
  } // end for x

  // test real matching
  pts.clear();
  for (int x = -5; x <= 5; ++x)
    pts.push_back(Pt3f(x, 0, 0));
  ppl_utils::ppl_factory(tracks, pts, 0, track_stamp);
  ASSERT_TRUE(ppl_gating::gate_pp2tracks(pp, tracks, human_walking_speed) == 5);
}

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, gate_ppl) {
  ros::Time track_stamp = ros::Time(100), ppl_stamp = track_stamp + ros::Duration(1); // 1 second after tracks
  PPL ppl, tracks;
  std::vector<PP> unassociated_poses_from_new_ppl;
  double human_walking_speed = 10;
  ASSERT_TRUE(ppl_gating::gate_ppl(ppl, tracks, unassociated_poses_from_new_ppl, human_walking_speed));
  ASSERT_TRUE(unassociated_poses_from_new_ppl.empty());

  std::vector<Pt3f> track_pts, ppl_pts;
  for (int x = 0; x <= 5; ++x)
    track_pts.push_back(Pt3f(x, 0, 0));
  for (int x = -20; x <= 20; ++x)
    ppl_pts.push_back(Pt3f(x, 0, 0));
  ppl_utils::ppl_factory(tracks, track_pts, 0, track_stamp);
  ppl_utils::ppl_factory(ppl, ppl_pts, 0, ppl_stamp);
  ASSERT_TRUE(ppl_gating::gate_ppl(ppl, tracks, unassociated_poses_from_new_ppl, human_walking_speed));
  ASSERT_TRUE(unassociated_poses_from_new_ppl.size() == 15); // -20 ... -11, 16 .. 20
}


////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, match_ppl2tracks_and_clean) {
  PPL ppl, tracks;
  std::vector<PP> unassociated_poses_from_new_ppl;
  CMatrix<double> costs;
  assignment_utils::MatchList matches;
  // empty ppl and track
  ASSERT_TRUE(ppl_gating::match_ppl2tracks_and_clean(ppl, tracks, costs, unassociated_poses_from_new_ppl, matches));
  ASSERT_TRUE(unassociated_poses_from_new_ppl.empty());
  ASSERT_TRUE(matches.empty());

  for (unsigned int ntracks = 0; ntracks < 10; ++ntracks) {
    // empty tracks
    tracks.poses.clear();
    ppl.poses.resize(ntracks);
    ASSERT_TRUE(ppl_gating::match_ppl2tracks_and_clean(ppl, tracks, costs, unassociated_poses_from_new_ppl, matches));
    ASSERT_TRUE(unassociated_poses_from_new_ppl.size() == ntracks);
    ASSERT_TRUE(matches.empty());

    // simple diag cost
    tracks.poses.resize(ntracks);
    costs.resize(ntracks, ntracks);
    for (unsigned int i = 0; i < ntracks; ++i)
      for (unsigned int j = 0; j < ntracks; ++j)
        costs[i][j] = (i == j ? 1 : 10);
    unassociated_poses_from_new_ppl.clear();
    ASSERT_TRUE(ppl_gating::match_ppl2tracks_and_clean(ppl, tracks, costs, unassociated_poses_from_new_ppl, matches));
    ASSERT_TRUE(unassociated_poses_from_new_ppl.size() == 0);
    ASSERT_TRUE(matches.size() == ntracks);
    for (unsigned int x = 0; x < ntracks; ++x)
      ASSERT_TRUE(matches[x].first == matches[x].second);

    // antidiagonal
    for (unsigned int i = 0; i < ntracks; ++i)
      for (unsigned int j = 0; j < ntracks; ++j)
        costs[i][j] = (i == (ntracks-1) - j ? 1 : 10);
    unassociated_poses_from_new_ppl.clear();
    ASSERT_TRUE(ppl_gating::match_ppl2tracks_and_clean(ppl, tracks, costs, unassociated_poses_from_new_ppl, matches));
    ASSERT_TRUE(unassociated_poses_from_new_ppl.size() == 0);
    ASSERT_TRUE(matches.size() == ntracks);
    for (unsigned int x = 0; x < ntracks; ++x)
      ASSERT_TRUE(matches[x].first == ((int) ntracks-1) - matches[x].second);
  } // end for ntracks
}

////////////////////////////////////////////////////////////////////////////////

TEST(TestSuite, update_blobs_and_create_new_tracks) {
  ros::Time time_now = ros::Time(100);
  std::vector<PP> unassociated_poses_from_new_ppl;
  PPL blobs, tracks;
  unsigned int total_seen_tracks = 0;
  double human_walking_speed = ppl_gating::DEFAULT_HUMAN_WALKING_SPEED;
  ASSERT_TRUE(ppl_gating::update_blobs_and_create_new_tracks
              (time_now, unassociated_poses_from_new_ppl, blobs, tracks, total_seen_tracks, human_walking_speed));
  ASSERT_TRUE(unassociated_poses_from_new_ppl.size() == 0);

  // create a simple blob
  for (unsigned int x = 0; x < 5; ++x) {
    blobs.poses.clear();
    tracks.poses.clear();
    PP pp;
    pp.head_pose.position.x = pp.head_pose.position.y = x;
    pp.confidence = 1;
    pp.header.stamp = time_now;
    for (unsigned int i = 0; i < 10; ++i) {
      unassociated_poses_from_new_ppl.clear();
      unassociated_poses_from_new_ppl.push_back(pp);
      ASSERT_TRUE(ppl_gating::update_blobs_and_create_new_tracks
                  (time_now, unassociated_poses_from_new_ppl, blobs, tracks, total_seen_tracks, human_walking_speed));
      ASSERT_TRUE(unassociated_poses_from_new_ppl.size() == 0);
      if (!tracks.poses.empty()) // break when the first track is created
        break;
    } // end for i
    ASSERT_TRUE(blobs.poses.size()  <= 0) // the track was created, no blob left
        << "blobs:" << ppl_utils::ppl2string(blobs, 3, false);
    ASSERT_TRUE(tracks.poses.size() == 1)
        << "tracks:" << ppl_utils::ppl2string(tracks, 3, false);
    geometry_msgs::Point track_pos = tracks.poses.front().head_pose.position;
    ASSERT_TRUE(track_pos.x == x && track_pos.y == x)
        << "tracks:" << ppl_utils::ppl2string(tracks, 3, false);
  } // end for x
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv){
  ros::init(argc, argv, "gtest_ppl_gating");
  // Run all the tests that were declared with TEST()
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
