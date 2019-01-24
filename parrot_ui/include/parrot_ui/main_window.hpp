/**
 * @file /include/parrot_ui/main_window.hpp
 *
 * @brief Qt based gui for parrot_ui.
 *
 * @date November 2010
 **/
#ifndef parrot_ui_MAIN_WINDOW_H
#define parrot_ui_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui/QMainWindow>
#include "ui_main_window.h"
#include "qnode.hpp"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define DEST_PORT 8000
#define DSET_IP_ADDRESS  "127.0.0.1"

/*****************************************************************************
** Namespace
*****************************************************************************/

namespace parrot_ui {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(int argc, char** argv, QWidget *parent = 0);
	~MainWindow();

	void ReadSettings(); // Load up qt program settings at startup
	void WriteSettings(); // Save qt program settings when closing

	void closeEvent(QCloseEvent *event); // Overloaded function
	void showNoMasterMessage();

public Q_SLOTS:
	/******************************************
	** Auto-connections (connectSlotsByName())
	*******************************************/
	void on_actionAbout_triggered();
	void on_button_connect_clicked(bool check );
	void on_checkbox_use_environment_stateChanged(int state);

    /******************************************
    ** Manual connections
    *******************************************/
    void updateLoggingView(); // no idea why this can't connect automatically


    void on_pushButton_go_clicked();
    void on_pushButton_enable_atti_ctrl_clicked();
    void on_pushButton_disable_atti_ctrl_clicked();
    void on_pushButton_disarm_clicked();
    void on_pushButton_arm_clicked();
    void on_pushButton_go_2_clicked();

private:
	Ui::MainWindowDesign ui;
	QNode qnode;

};

}  // namespace parrot_ui

#endif // parrot_ui_MAIN_WINDOW_H
