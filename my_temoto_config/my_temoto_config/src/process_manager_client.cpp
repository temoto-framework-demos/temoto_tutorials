#include "ros/ros.h"
#include "temoto_process_manager/process_manager_interface.hpp"

void resourceFailureCallback(temoto_process_manager::LoadProcess load_resource_msg, temoto_resource_registrar::Status status_msgs)
{
  ROS_WARN_STREAM("The following resource stopped unexpectedly\n" << load_resource_msg.request);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "process_client_node");
  ros::AsyncSpinner spinner(4); // Use 4 threads
  spinner.start();
  //ros::waitForShutdown();

  /*
   * Create Process Manager Interface object that provides a simplified
   * API for communicating with the Process Manager. The boolean "true", that's passed
   * to the constructor of ERM interface tells it whether it should be initialised immediately,
   * or that's done later by the user.
   */
  temoto_process_manager::ProcessManagerInterface pmi(true);

  /*
   * You can register a custom routine (not required) where resource failures are reported.
   */
  // pmi.registerUpdateCallback(resourceFailureCallback);

  /*
   * Load a ROS program an example of a ROS executable (regularly invoked via 'rosrun'). The first
   * parameter indicates the ROS package name and the second indicates the executable. Additional
   * arguments can also be passed as a third std::string variable. The same method can be used to
   * load ROS launch files
   */
  ROS_INFO("Loading usb_cam");
  pmi.loadRosResource("my_temoto_config", "usb_cam_remappable.launch");
  ros::Duration(15).sleep();
  
  /*
   * Note that this time the "unloadResource" was not invoked, as the destructor of "pmi" automatically
   * unloads all loaded resources.
   */ 
  return 0;
}