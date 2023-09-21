#include "ros/ros.h"
#include "temoto_robot_manager/robot_manager_interface.h"

int main(int argc, char** argv)
try
{
  ros::init(argc, argv, "rm_nav_test");
  ros::AsyncSpinner spinner(4);
  spinner.start();

  temoto_robot_manager::RobotManagerInterface rmi(true);

  /*
   * LOAD THE ROBOT
   */
  std::string robot_name = "robotont_sim_1";
  std::cout << "Loading the robot ..." << std::endl;
  rmi.loadRobot(robot_name);
  std::cout << "Robot loaded. Config:\n" << rmi.getRobotConfig(robot_name) << std::endl << std::endl;

  ros::Duration(2).sleep();

  /*
   * INVOKE THE NAVIGATION FEATURE
   */
  temoto_robot_manager::RobotNavigationGoal navigation_request;
  navigation_request.request.robot_name = robot_name;
  navigation_request.request.target_pose.header.frame_id = "map";
  navigation_request.request.target_pose.pose.position.x = 5.0;
  navigation_request.request.target_pose.pose.orientation.w = 1.0;
    
  std::cout << "Invoking the navigation feature ..." << std::endl;
  // rmi.navigationGoal(navigation_request.request.robot_name,
  //                    navigation_request.request.target_pose);

  if (rmi.navigationGoal(navigation_request))
  {
    std::cout << "Navigation goal invoked successfully. Response:\n" << navigation_request.response << std::endl;
  }

  std::cout << "Navigation goal sent ..." << std::endl;

  ros::Duration(1).sleep();

  

  /*
   * PRE-EMPT THE CUSTOM FEATURE
   */
  std::cout << "Cancel goal ..." << std::endl;
  if (rmi.cancelNavigationGoal(robot_name))
  {
    std::cout << "Cancel goal successfully\n" << std::endl;
    ros::Duration(4).sleep();
  }
  else
  {
    std::cout << "Unable to cancel goal" << std::endl;
    return 1;
  }

  // /*
  //  * INVOKE THE SAME CUSTOM FEATURE AGAIN
  //  */
  // std::cout << "Invoking the same custom feature again ..." << std::endl;
  // if (rmi.invokeCustomFeature(custom_request))
  // {
  //   std::cout << "Custom feature invoked successfully. Response:\n" << custom_request.response << std::endl;
  //   ros::Duration(5).sleep();
  // }
  // else
  // {
  //   std::cout << "Unable to invoke custom feature because: " << custom_request.response.message << std::endl;
  //   return 1;
  // }

  return 0;
}
catch(resource_registrar::TemotoErrorStack& e)
{
  std::cout << e.what() << std::endl;
  return 1;
}