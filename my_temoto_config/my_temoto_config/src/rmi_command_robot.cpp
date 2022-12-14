#include "ros/ros.h"
#include "temoto_robot_manager/robot_manager_interface.h"

int main(int argc, char** argv)
{
  ros::init(argc, argv, "rm_load_robot");
  ros::AsyncSpinner spinner(4); // Use 4 threads
  spinner.start();
  
  /*
   * Create a Robot Manager Interface object that provides a simplified
   * API for communicating with the Robot Manager. 
   */
  temoto_robot_manager::RobotManagerInterface rmi_;
  rmi_.initialize();

  // // Initialize the robot  
  std::string robot_name = "vaultbot_sim";    // Robot name defined on the robot_description
  rmi_.loadRobot(robot_name);

  /*
  *  Navigation 
  */
  // Define a navigation goal 2D pose  
  geometry_msgs::PoseStamped target_pose;
  target_pose.pose.position.x = -3.0;
  target_pose.pose.position.y = 1.0;
  target_pose.pose.orientation.x = 0.0;
  target_pose.pose.orientation.y = 0.0;
  target_pose.pose.orientation.z = 0.0;
  target_pose.pose.orientation.w = 1.0;
  
  // Send goal pose to robot using the robot manager interface
  bool goal_reached = false;
  while (!goal_reached && ros::ok())
  try
  {
    TEMOTO_INFO_STREAM_("Sending a navigation goal to " << robot_name << " ...");
    rmi_.navigationGoal(robot_name, "map", target_pose);

    TEMOTO_INFO_STREAM_("Done navigating");
    goal_reached = true;
  }
  catch(const resource_registrar::TemotoErrorStack &e)
  {
    TEMOTO_WARN_STREAM_("Caught an error: " << e.what() << "\nRequesting the same navigation goal again ... ");
  }

  /*
  *  Manipulation 
  */
  TEMOTO_INFO_STREAM_("Current pose");
  geometry_msgs::PoseStamped eef_pose;
  eef_pose.pose = rmi_.getEndEffPose(robot_name);
  std::cout << eef_pose.pose.position.x << eef_pose.pose.position.y << eef_pose.pose.position.z << std::endl;

  TEMOTO_INFO_STREAM_("Shift manipulation target pose");
  target_pose.pose.position.x = eef_pose.pose.position.x + 0.3;
  target_pose.pose.position.y = eef_pose.pose.position.y;
  target_pose.pose.position.z = eef_pose.pose.position.z + 0.2;

  target_pose.pose.orientation = eef_pose.pose.orientation;  

  rmi_.planManipulation(robot_name, "r_arm", target_pose);  
  rmi_.executePlan(robot_name);

  TEMOTO_INFO_STREAM_("New pose");
  eef_pose.pose = rmi_.getEndEffPose(robot_name);
  std::cout << eef_pose.pose.position.x << eef_pose.pose.position.y << eef_pose.pose.position.z << std::endl;

  /*
   * Note that this time the "unloadResource" was not invoked, as the destructor of "pmi" automatically
   * unloads all loaded resources.
   */ 
  return 0;
}