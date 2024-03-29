#include "ros/ros.h"
#include "temoto_component_manager/component_manager_interface.h"

void componentFailureCallback(temoto_component_manager::LoadComponent load_component_msg
, temoto_resource_registrar::Status status_msgs)
{
  ROS_WARN_STREAM("The following component stopped unexpectedly\n" << load_component_msg.request);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "component_manager_client");
  ros::AsyncSpinner spinner(4);
  spinner.start();

  /*
   * Create Component Manager Interface object that provides a simplified
   * API for communicating with the Component Manager. The boolean "true", that's passed
   * to the constructor of ERM interface tells it whether it should be initialised immediately,
   * or that's done later by the user.
   */
  temoto_component_manager::ComponentManagerInterface cmi(true);

  /*
   * You can register a custom routine (not required) where resource failures are reported.
   */
  cmi.registerComponentStatusCallback(componentFailureCallback);

  ROS_INFO("Loading 2d_camera");
  auto responded_topics = cmi.startComponent("2d_camera");

  // Print out the topics
  for (const auto& topic : responded_topics.getOutputTopics())
  {
    ROS_INFO_STREAM("* " << topic.first << ": " << topic.second);
  }

  ros::Duration(30).sleep();

  ROS_INFO("Unloading 2d_camera");
  cmi.stopComponent("2d_camera", "", "");

  return 0;
}