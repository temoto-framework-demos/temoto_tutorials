#include <class_loader/class_loader.hpp>
#include <iostream>
#include <chrono>
#include <thread>

#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

#include "temoto_robot_manager/navigation_plugin_base.h"

using namespace temoto_robot_manager;

class MyNavigationFeature : public NavigationPluginBase
{
public:

MyNavigationFeature()
{}

bool initialize(const std::string& robot_ns)
{   
  std::string action_client = robot_ns + "/move_base";
  ac_.reset(new MoveBaseClient(action_client, true));
  std::cout << "\033[1;31m my_navigation_feature initialized succesfully\033[0m\n" << std::endl;
  progress = 0;
  stop_requested = false;
  return true;
}

bool sendGoal(RmNavigationGoal goal)
{
  bool task_finished = false;
  float max_duration = goal.goal_pose.pose.position.x;
  auto start = std::chrono::high_resolution_clock::now();

  if (!ac_->waitForServer(ros::Duration(5.0)))
  {
    std::cout << "The move_base action server did not come up" << std::endl;
  }

  move_base_msgs::MoveBaseGoal mb_goal;

  // Convert TRM_PoseStamped --> geometry_msg/PoseStamped
  mb_goal.target_pose.header.frame_id = goal.goal_pose.header.frame_id;
  mb_goal.target_pose.header.stamp = ros::Time::now();
  mb_goal.target_pose.pose.position.x = goal.goal_pose.pose.position.x;
  mb_goal.target_pose.pose.position.y = goal.goal_pose.pose.position.y;
  mb_goal.target_pose.pose.position.z = goal.goal_pose.pose.position.z;
  mb_goal.target_pose.pose.orientation.x = goal.goal_pose.pose.orientation.x;
  mb_goal.target_pose.pose.orientation.y = goal.goal_pose.pose.orientation.y;
  mb_goal.target_pose.pose.orientation.z = goal.goal_pose.pose.orientation.z;
  mb_goal.target_pose.pose.orientation.w = goal.goal_pose.pose.orientation.w;

  ac_->sendGoal(mb_goal);
  ac_->waitForResult();

  if(ac_->getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
  {
    std::cout << "\033[1;33m Hooray, the base moved\033[0m\n" <<std::endl;
    task_finished = true;
  }      
  else
  {
    std::cout << "\033[1;33m The base failed to move for some reason\033[0m\n" <<std::endl;
  }
      
  if (!task_finished)
  {
    std::cout << "\033[1;33m could not finish the task\033[0m\n" << std::endl;    
    // return false;
  }

  stop_requested = false;
  return true;
}

std::optional<RmNavigationFeedback> getFeedback()
{
  RmNavigationFeedback feedback;
  feedback.progress = progress;
  feedback.base_position.pose.position.x = 0;
  feedback.base_position.pose.position.y = 1;
  feedback.base_position.pose.position.z = 2;
  feedback.base_position.pose.orientation.x = 3;
  feedback.base_position.pose.orientation.y = 4;
  feedback.base_position.pose.orientation.z = 5;
  feedback.base_position.pose.orientation.w = 6;

  return std::optional<RmNavigationFeedback>({
    feedback
  });
}

bool cancelGoal()
{
  ac_->cancelGoal();
  stop_requested = true;
  std::cout << "Cancel Goal succesfully" << std::endl;
  return true;
}

bool deinitialize()
{
  std::cout << "\033[1;31m my_navigation_feature_1 deinitialized succesfully\033[0m\n" << std::endl;
  return true;
}

// Destructor
~MyNavigationFeature()
{}

bool stop_requested;
double progress;
PoseStamped base_position;
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

private:    
    std::unique_ptr<MoveBaseClient> ac_;


}; // MyNavigationFeature class

/* REQUIRED BY CLASS LOADER */
CLASS_LOADER_REGISTER_CLASS(MyNavigationFeature, NavigationPluginBase);
