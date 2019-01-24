/**
 * @file /include/parrot_ui/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef parrot_ui_QNODE_HPP_
#define parrot_ui_QNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/

// To workaround boost/qt4 problems that won't be bugfixed. Refer to
//    https://bugreports.qt.io/browse/QTBUG-22829
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <string>
#include <QThread>
#include <QStringListModel>
#include <vector>
#include <std_msgs/Bool.h>
#include <gazebo_msgs/ModelStates.h>

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace parrot_ui {

/*****************************************************************************
** Class
*****************************************************************************/

class QNode : public QThread {
    Q_OBJECT
public:
	QNode(int argc, char** argv );
	virtual ~QNode();
	bool init();
	bool init(const std::string &master_url, const std::string &host_url);
	void run();

	/*********************
	** Logging
	**********************/
	enum LogLevel {
	         Debug,
	         Info,
	         Warn,
	         Error,
	         Fatal
	 };

	QStringListModel* loggingModel() { return &logging_model; }
	void log( const LogLevel &level, const std::string &msg);



Q_SIGNALS:
	void loggingUpdated();
	void rosShutdown();

private:
	int init_argc;
	char** init_argv;

    ros::Publisher enable_attitude_control_pub;
    ros::Publisher dest_pos_pub;
    ros::Subscriber model_states_sub;
    ros::ServiceClient enable_moter_cli;
    ros::Publisher local_goal_pub;

	QStringListModel logging_model;

public:
    float posX, posY, posZ, oriX, oriY, oriZ, oriW;
    void modelStatesRecCallback(const gazebo_msgs::ModelStates &model_states);
    void send_des_pos_in_pos_mode(float x, float y, float z, float yaw);
    void send_des_pos_in_att_mode(float x, float y, float z, float yaw);
    void enable_atti_ctrl(void);
    void disable_atti_ctrl(void);
    void enable_motors();
    void disable_motors();
};

}  // namespace parrot_ui

#endif /* parrot_ui_QNODE_HPP_ */
