#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include "Comm.h"
#include "cameraobject.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QThread camera_thread;
    CameraObject *cameraObject;
    RobotComm ScrewsRobot;

    QTimer* _readTimer;
    QTimer* _motionModeTimer;
/********************************variable for function**********************************/
    UINT Step;                      //the state mechine needed

    double locationOfHole[1][3];    //the information of location
    bool bCameraReady;      //judge if the camera has reached the target point
    bool bConnectDone;      //judge if the camera has connect with the screws
    bool bCollapse;         //judge if the antomoiton precess has collapse
    bool bSingleStepDone;   //judeg if the screws has been screwed

    double currentXPosi = 0.0;
    double currentYPosi = 0.0;
    double currentZPosi = 0.0;


    enum modeFlag{
        AllNone = 1,
        XMoveUP,
        XMoveDown,
        YMoveUp,
        YMOveDown,
        ZMoveUP,
        ZMoveDown,
        ThreeAxisMove,
        NljMovePlus,
        NljMoveNeg,
    } controlFlag;

    enum PosiMode{
        bePosiLimitMode = 10,
        notBePosiLimitMode = -10,
    }RangeBeLimited;

    bool isNeedUpdataVar = true;

    // Mat CameraMatrix;
    // this variable cause the TCP 10063 error

private:
    void StateMachine();
    void writeCMD_ofLink();
    void getTheRobotState();

signals:
    void StartCamera();
    void startGetLocation();

private slots:
    void on_GetImage_Btn_clicked();

    void HandleImageResult(QImage *image);

    void on_ConnectBtn_clicked();

    void on_Btn_Reset_clicked();

    void _onReadTimeout();

    void _onHeartTimeout();

    void _onMotionModeTimout();

    void UI_GetLocation(int, int, QImage*);

    void on_GetLocation_Btn_pressed();

    void on_Btn_LinkReset_clicked();

    void on_Btn_LinkHome_clicked();

    void on_Btn_LinkStop_clicked();

    void on_Btn_LinkHalt_clicked();

    void on_Btn_Power_clicked();

    void on_Btn_Perapre_clicked();

    void on_Btn_CameraReady_clicked();

    void on_Btn_ConnectDone_clicked();

    void on_Btn_Collapse_clicked();

    void on_Btn_SingleStepDone_clicked();

    void on_Btn_Auto_clicked();

    void on_Btn_Handle_clicked();

    void on_Btn_Moveup_X_pressed();

    void on_Btn_Moveup_X_released();

    void on_Btn_Moveup_Y_pressed();

    void on_Btn_Moveup_Y_released();

    void on_Btn_MoveDown_X_pressed();

    void on_Btn_MoveDown_X_released();

    void on_Btn_MoveDown_Y_pressed();

    void on_Btn_MoveDown_Y_released();

    void on_Btn_Moveup_Z_pressed();

    void on_Btn_Moveup_Z_released();

    void on_Btn_MoveDown_Z_pressed();

    void on_Btn_MoveDown_Z_released();

    void on_Btn_Moveup_NLJ_pressed();

    void on_Btn_Moveup_NLJ_released();

    void on_Btn_MoveDown_NLJ_pressed();

    void on_Btn_MoveDown_NLJ_released();

    void on_Btn_PosLimit_clicked();

    void on_Btn_AxisHandle_clicked();

    void on_Btn_XBackword_pressed();
    void on_Btn_XBackword_released();
    void on_Btn_XForward_pressed();
    void on_Btn_XForward_released();
    void on_Btn_YLeftBackword_pressed();
    void on_Btn_YLeftBackword_released();
    void on_Btn_YLeftForward_pressed();
    void on_Btn_YLeftForward_released();
    void on_Btn_YRightBackward_pressed();
    void on_Btn_YRightBackward_released();
    void on_Btn_YRightForward_pressed();
    void on_Btn_YRightForward_released();
    void on_Btn_ZLeftBackward_pressed();
    void on_Btn_ZLeftBackward_released();
    void on_Btn_ZLeftForward_pressed();
    void on_Btn_ZLeftForward_released();
    void on_Btn_ZRightBackward_pressed();
    void on_Btn_ZRightBackward_released();
    void on_Btn_ZRightForward_pressed();
    void on_Btn_ZRightForward_released();
    void on_Btn_Moveup_Z_2_pressed();
    void on_Btn_Moveup_Z_2_released();
};

#endif // MAINWINDOW_H
