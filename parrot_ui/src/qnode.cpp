/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <std_msgs/Bool.h>
#include <geometry_msgs/PoseStamped.h>
#include <sstream>
#include "../include/parrot_ui/qnode.hpp"
#include "hector_uav_msgs/EnableMotors.h"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace parrot_ui {

/*****************************************************************************
** Implementation
*****************************************************************************/

QNode::QNode(int argc, char** argv ) :
	init_argc(argc),
	init_argv(argv)
	{}

QNode::~QNode() {
    if(ros::isStarted()) {
      ros::shutdown(); // explicitly needed since we use ros::start();
      ros::waitForShutdown();
    }
	wait();
}

bool QNode::init() {
	ros::init(init_argc,init_argv,"parrot_ui");
	if ( ! ros::master::check() ) {
        return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
	// Add your ros communications here.

    dest_pos_pub = n.advertise<geometry_msgs::PoseStamped>("/command/pose", 1);
    model_states_sub = n.subscribe("/gazebo/model_states", 1, &QNode::modelStatesRecCallback, this);
    enable_attitude_control_pub = n.advertise<std_msgs::Bool>("/enable_attitude_control", 1);
    enable_moter_cli = n.serviceClient<hector_uav_msgs::EnableMotors>("/enable_motors");
    local_goal_pub = n.advertise<geometry_msgs::PoseStamped>("/local_goal", 1);

	start();
	return true;
}

bool QNode::init(const std::string &master_url, const std::string &host_url) {
	std::map<std::string,std::string> remappings;
	remappings["__master"] = master_url;
	remappings["__hostname"] = host_url;
	ros::init(remappings,"parrot_ui");
	if ( ! ros::master::check() ) {
		return false;
	}
	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;
	// Add your ros communications here.

    dest_pos_pub = n.advertise<geometry_msgs::PoseStamped>("/command/pose", 1);
    model_states_sub = n.subscribe("/gazebo/model_states", 1, &QNode::modelStatesRecCallback, this);
    enable_attitude_control_pub = n.advertise<std_msgs::Bool>("/enable_attitude_control", 1);
    enable_moter_cli = n.serviceClient<hector_uav_msgs::EnableMotors>("/enable_motors");
    local_goal_pub = n.advertise<geometry_msgs::PoseStamped>("/local_goal", 1);

	start();
	return true;
}

void QNode::run() {
	while ( ros::ok() ) {
        ros::spin();
	}
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}


void QNode::log( const LogLevel &level, const std::string &msg) {
	logging_model.insertRows(logging_model.rowCount(),1);
	std::stringstream logging_model_msg;
	switch ( level ) {
		case(Debug) : {
				ROS_DEBUG_STREAM(msg);
				logging_model_msg << "[DEBUG] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Info) : {
				ROS_INFO_STREAM(msg);
				logging_model_msg << "[INFO] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Warn) : {
				ROS_WARN_STREAM(msg);
				logging_model_msg << "[WARN] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Error) : {
				ROS_ERROR_STREAM(msg);
				logging_model_msg << "[ERROR] [" << ros::Time::now() << "]: " << msg;
				break;
		}
		case(Fatal) : {
				ROS_FATAL_STREAM(msg);
				logging_model_msg << "[FATAL] [" << ros::Time::now() << "]: " << msg;
				break;
		}
	}
	QVariant new_row(QString(logging_model_msg.str().c_str()));
	logging_model.setData(logging_model.index(logging_model.rowCount()-1),new_row);
	Q_EMIT loggingUpdated(); // used to readjust the scrollbar
}




void QNode::send_des_pos_in_pos_mode(float x, float y, float z, float yaw)
{
    if(ros::ok())
    {
        geometry_msgs::PoseStamped msg;
        msg.header.frame_id = "world";
        msg.pose.position.x = x;
        msg.pose.position.y = y;
        msg.pose.position.z = z;
        Eigen::AngleAxis<float> rotation_vector(yaw*M_PI/180.0, Eigen::Vector3f::UnitZ());
        Eigen::Quaternion<float> quad(rotation_vector);
        msg.pose.orientation.x = quad.x();
        msg.pose.orientation.y = quad.y();
        msg.pose.orientation.z = quad.z();
        msg.pose.orientation.w = quad.w();
        dest_pos_pub.publish(msg);
    }
}

void QNode::send_des_pos_in_att_mode(float x, float y, float z, float yaw)
{
    if(ros::ok())
    {
        geometry_msgs::PoseStamped msg;
        msg.header.frame_id = "world";
        msg.pose.position.x = x;
        msg.pose.position.y = y;
        msg.pose.position.z = z;
        Eigen::AngleAxis<float> rotation_vector(yaw*M_PI/180.0, Eigen::Vector3f::UnitZ());
        Eigen::Quaternion<float> quad(rotation_vector);
        msg.pose.orientation.x = quad.x();
        msg.pose.orientation.y = quad.y();
        msg.pose.orientation.z = quad.z();
        msg.pose.orientation.w = quad.w();
        local_goal_pub.publish(msg);
    }
}


void QNode::enable_atti_ctrl()
{
    if(ros::ok())
    {
        std_msgs::Bool flag;
        flag.data = true;
        enable_attitude_control_pub.publish(flag);

        log(Warn, std::string("start auto flying!"));
    }
}


void QNode::disable_atti_ctrl()
{
    if(ros::ok())
    {
        send_des_pos_in_pos_mode(posX, posY, posZ, 0);
        log(Warn, std::string("stop here!"));

    }
}

void QNode::modelStatesRecCallback(const gazebo_msgs::ModelStates &model_states)
{
    if(ros::ok())
    {
        int idx = 0;
        for(int i=0; i<model_states.name.size(); i++)
        {
            if(model_states.name[i] == "quadrotor")
                idx = i;
        }
        posX = model_states.pose[idx].position.x;
        posY = model_states.pose[idx].position.y;
        posZ = model_states.pose[idx].position.z;
        oriX = model_states.pose[idx].orientation.x;
        oriY = model_states.pose[idx].orientation.y;
        oriZ = model_states.pose[idx].orientation.z;
        oriW = model_states.pose[idx].orientation.w;
    }
}


void QNode::enable_motors()
{
    if(ros::ok())
    {
        hector_uav_msgs::EnableMotors srv;
        srv.request.enable = true;
        if(enable_moter_cli.call(srv))
        {
            log(Warn, std::string("armed!"));
        }
        else
        {
            log(Warn, std::string("arming failed!"));
        }
    }
}


void QNode::disable_motors()
{
    if(ros::ok())
    {
        hector_uav_msgs::EnableMotors srv;
        srv.request.enable = false;
        if(enable_moter_cli.call(srv))
        {
            log(Warn, std::string("disarmed!"));
        }
        else
        {
            log(Warn, std::string("disarming failed!"));
        }
    }
}


}  // namespace parrot_ui
