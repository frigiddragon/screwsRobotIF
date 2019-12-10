#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <strstream>
#include <string>

double recordObjPosiX = 0.0;
double recordObjPosiY = 0.0;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /************initial Variable***********/
    Step = 0;
    bCameraReady = FALSE;
    bConnectDone = FALSE;
    bCollapse = FALSE;
    bSingleStepDone = FALSE;
    controlFlag = AllNone;
    RangeBeLimited = bePosiLimitMode;

    //get location information from .txt
    string locationMatrixPath= "C:/Users/Bruce02/Desktop/Screws/ScrewsRobotIF/ScrewsRobotIF/locationParas/locationMatrix.txt";
    ifstream locationOfHoleFile(locationMatrixPath.c_str());
    if(locationOfHoleFile.is_open())
    {
        for(int i=0;i<1;i++)
            for(int j=0;j<3;j++)
                locationOfHoleFile>>locationOfHole[i][j];
       locationOfHoleFile.close();
    }
    else
    {
        QMessageBox::warning(0,"Warning","Can't read locationMatrix");
    }

    {
        //*****timer1 for get status from lower mechine******//
        _readTimer = new QTimer(this);
        connect(_readTimer, SIGNAL(timeout()), this, SLOT(_onReadTimeout()));
        _readTimer->start(25);

        //******timer3, in motionmode the timer need to send controlWord regularly*****//
        _motionModeTimer = new QTimer(this);
        connect(_motionModeTimer, SIGNAL(timeout()), this, SLOT(_onMotionModeTimout()));
        _motionModeTimer->start(200);

        cameraObject = new CameraObject();
        cameraObject->moveToThread(&camera_thread);
    }

    connect(&camera_thread,SIGNAL(finished()),&camera_thread,SLOT(deleteLater()));
    connect(this,SIGNAL(startGetLocation()),this,SLOT(on_GetLocation_Btn_pressed()));
    connect(&camera_thread,SIGNAL(finished()),cameraObject,SLOT(deleteLater()));
    connect(this,SIGNAL(StartCamera()),cameraObject,SLOT(StartContinueCamera()));
    connect(cameraObject,SIGNAL(ImageResultReady(QImage *)), this, SLOT(HandleImageResult(QImage *)));
    connect(cameraObject,SIGNAL(GetLocationDone(int, int, QImage*)), this, SLOT(UI_GetLocation(int, int, QImage*)));

    camera_thread.start();
}


MainWindow::~MainWindow()
{
    delete ui;
    delete _readTimer;
    camera_thread.quit();
    camera_thread.wait();
}


void MainWindow::_onReadTimeout()
{
    if(true == this->ScrewsRobot.m_CommState)
    {
        ui->Label_bConnected->setText("TRUE");
        ui->Label_bConnected->setStyleSheet("color: green");
        ScrewsRobot.RecvData();
        getTheRobotState();
    }
    else
    {
        ui->Label_bConnected->setText("FALSE");
        ui->Label_bConnected->setStyleSheet("color: red");
        return;
    }
    StateMachine();//the state mechine
}


void MainWindow::_onHeartTimeout()
{
}


void MainWindow::HandleImageResult(QImage *image)
{
    QPixmap pix = QPixmap::fromImage(*image);
    ui->Image_Label->setPixmap(pix);
    ui->Image_Label->show();
}


void MainWindow::on_GetImage_Btn_clicked(){
    if(ui->GetImage_Btn->text()=="Open Camera")
    {
        cameraObject->SetLableSize(ui->Image_Label->size());
        emit StartCamera();
        ui->GetImage_Btn->setText("Close Camera");
    }
    else if(ui->GetImage_Btn->text()=="Close Camera")
    {
        cameraObject->StopContinueImage();
        ui->GetImage_Btn->setText("Open Camera");
    }
}



void MainWindow::on_ConnectBtn_clicked()
{
    if(ui->ConnectBtn->text()=="Connect")
    {
        //the next is the operation
        //Connect to the Server
        QString Tempipaddress = ui->LineEdit_IPAdress->text();
        QByteArray temp = Tempipaddress.toLatin1();
        char *ip_address = temp.data();
        int bConnect = ScrewsRobot.ConnectToServer(ip_address, 4999);
        if (bConnect == 0)
        {

            ui->Label_bConnected->setText("DONE");
            ui->Label_bConnected->setStyleSheet("color: black");
            ui->ConnectBtn->setText("Disconnect");
            ui->ConnectBtn->setStyleSheet("color: red");
        }
        else
        {
            ui->Label_bConnected->setText("FALSE");
            ui->Label_bConnected->setStyleSheet("color: red");
        }
    }
    else if(ui->ConnectBtn->text()=="Disconnect")
    {
        ui->ConnectBtn->setText("Connect");
        ui->ConnectBtn->setStyleSheet("color: black");
        //the next is the operation
        ScrewsRobot.close();
        ui->Label_bConnected->setText("FASLE");
        ui->Label_bConnected->setStyleSheet("color: red");
    }
}


void MainWindow::on_Btn_Reset_clicked()
{
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
    ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionLineAbsolute;
    ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 0;
    ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 0;
    ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = 0;
    ScrewsRobot.SendData();
}


void MainWindow::UI_GetLocation(int x, int y, QImage* image)
{
    QPixmap tempPix = QPixmap::fromImage(*image);
    ui->Image_Label_result->setPixmap(tempPix);
    ui->Image_Label_result->show();
    int MidX = 448;
    int MidY = 448;
    double DisX = double(MidX-x)*0.0393;
    double DisY = double(MidY-y)*0.0393;
    ui->label_CameraX->setText(QString::number(DisX, 10, 2));
    ui->label_CameraY->setText(QString::number(DisY, 10, 2));
}


void MainWindow::StateMachine()
{
    //上位机的状态机
    switch (Step) {
    case 0:                   //初始状态，判断是否已经复位完成
        if ((ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[0] == 0) && (ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[1] == 0)
                && (ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[2] == 0))
        {
            Step = 0;               //程序跑的时候屏蔽了上位机
        }
        break;
    case 5:                  //控制相机按照表格运动到指定的位置，并且拍照和获取定位
        if(ScrewsRobot.m_LinkStatus[0].stLinkActKin.bDone && bCameraReady == FALSE)
        {
            emit startGetLocation();
        }
        else if(ScrewsRobot.m_LinkStatus[0].stLinkActKin.bDone && bCameraReady)
        {
            Step = 10;
            bCameraReady = FALSE;
        }
        else
        {
            if(!ScrewsRobot.m_LinkStatus[0].stLinkActKin.bBusy){
                ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionLineAbsolute;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = locationOfHole[0][0];
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = locationOfHole[0][1];
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = locationOfHole[0][2];
                ScrewsRobot.SendData();
            }
        }
        break;
    case 10:                //在定位信息获取了之后，控制拧紧电机移动到相应的位置
        if(ScrewsRobot.m_LinkStatus[0].stLinkActKin.bDone)
        {
            Step = 15;
        }
        else
        {
            if(!ScrewsRobot.m_LinkStatus[0].stLinkActKin.bBusy){
                ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionLineRelative;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 0;  //相机获得的X轴的相对位置
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 0;  //相机获得的Y轴的相对位置
                ScrewsRobot.SendData();
            }
        }
        break;
    case 15:                //拧紧电机的第一段位移
        if(ScrewsRobot.m_LinkStatus[0].stLinkActKin.bDone)
        {
            Step = 20;
        }
        else
        {
            if(!ScrewsRobot.m_LinkStatus[0].stLinkActKin.bBusy){
                ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionLineAbsolute;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[0];
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[1];
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = 0;  //Z轴第一段的位置
                ScrewsRobot.SendData();
            }
        }
        break;
    case 20:                //第一段位移完成之后，控制电机完成第二段位移，这段位移的同时，电机的轴以非常小的速度转动
        if(ScrewsRobot.m_LinkStatus[0].stLinkActKin.bDone && bConnectDone)
        {
            Step = 25;
            bConnectDone = FALSE;
        }
        else if(bCollapse) //bConn....是一个由人判断的，是否出现故障的标志位
        {
            if(!ScrewsRobot.m_LinkStatus[0].stLinkActKin.bBusy){
                ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode1;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = 0;  //需要指定一个相应的速度
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = 0;  //Z轴第二段的位置，回去
                ScrewsRobot.SendData();
                bCollapse = FALSE;
            }
        }
        else
        {
            if(!ScrewsRobot.m_LinkStatus[0].stLinkActKin.bBusy){
                ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode1;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = 0;  //需要指定一个相应的速度
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = 0;  //Z轴第二段的位置，下去
                ScrewsRobot.SendData();
            }
        }
        break;
    case 25:                //对接上之后，控制电机直接下降，下压螺丝刀到相应的距离
        if(ScrewsRobot.m_LinkStatus[0].stLinkActKin.bDone && bConnectDone)   //bConnectDone是一个由人判断的标志位
        {
            Step = 30;
            bConnectDone = FALSE;
        }
        else
        {
            if(!ScrewsRobot.m_LinkStatus[0].stLinkActKin.bBusy){
                ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode1;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = 0;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = 0;  //Z轴第二段的位置
                ScrewsRobot.SendData();
            }
        }
        break;
    case 30:                    //下压成功之后，开始拧紧
        if(bSingleStepDone)     //bSingleStepDone是一个由人判断的标志位
        {
            Step = 5;
            bSingleStepDone = FALSE;
        }
        else
        {
            if(!ScrewsRobot.m_LinkStatus[0].stLinkActKin.bBusy){
                ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode1;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = 0;  //给定一个相应的速度
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[2];
                ScrewsRobot.SendData();
            }
        }
        break;
    default:
        break;
    }
}

void MainWindow::getTheRobotState()
{
    //the fllowing is write the current status of the Pr2
    QString QsLinkState;
    switch (ScrewsRobot.m_LinkStatus[0].eLinkActState) {
    case eLINK_ERRORSTOP:
        QsLinkState = "eLINK_ERRORSTOP";
        break;
    case eLINK_DISABLED:
        QsLinkState = "eLINK_DISABLED";
        break;
    case eLINK_STANDSTILL:
        QsLinkState = "eLINK_STANDSTILL";
        break;
    case eLINK_STOPPING:
        QsLinkState = "eLINK_STOPPING";
        break;
    case eLINK_HOMING:
        QsLinkState = "eLINK_HOME";
        break;
    case eLINK_MOVING:
        QsLinkState = "eLINK_MOVING";
        break;
    default:
        break;
    }

    QString StateOfPr2 ="X位置:\t";
    StateOfPr2 += QString::number(ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[0],10,2);
    StateOfPr2 +="\tY位置left:\t";
    StateOfPr2 += QString::number(ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[1],10,2);
    StateOfPr2 +="\tY位置right:\t";
    StateOfPr2 += QString::number(ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[2],10,2);
    StateOfPr2 +="\nZ位置left:\t";
    StateOfPr2 += QString::number(ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[3],10,2);
    StateOfPr2 +="\tZ位置right:\t";
    StateOfPr2 += QString::number(ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[4],10,2);
    StateOfPr2 +="\n拧螺钉的速度:\t";
    StateOfPr2 += QString::number(ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkVel[0],10,2);
    StateOfPr2 +="\nLINKSTATE :\t";
    StateOfPr2 += QsLinkState;
    ui->label_stateOfMotor->setText(StateOfPr2);
    currentXPosi = ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[0];
    currentYPosi = ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[1];
    currentZPosi = ScrewsRobot.m_LinkStatus[0].stLinkActKin.LinkPos[3];
}


/***********the next is some control operation****************/
void MainWindow::on_Btn_Power_clicked()
{
    if(ui->Btn_Power->text()=="POWER")
    {
        ui->CMDLink_Box->setCurrentIndex(0);
        ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_POWER;
        ScrewsRobot.m_LinkCommd[0].stLinkKinPar.bEnable = TRUE;
        ScrewsRobot.SendData();
        ui->Btn_Power->setText("POWEROFF");
    }
    else if(ui->Btn_Power->text()=="POWEROFF")
    {
        ui->CMDLink_Box->setCurrentIndex(0);
        ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_POWER;
        ScrewsRobot.m_LinkCommd[0].stLinkKinPar.bEnable = FALSE;
        ScrewsRobot.SendData();
        ui->Btn_Power->setText("POWER");
    }

}

void MainWindow::on_Btn_LinkReset_clicked()
{
    ui->CMDLink_Box->setCurrentIndex(3);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_RESET;
    ScrewsRobot.SendData();
    Step = 0;
}

void MainWindow::on_Btn_LinkHome_clicked()
{
    ui->CMDLink_Box->setCurrentIndex(4);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HOME;
    ScrewsRobot.SendData();
    Step = 0;
}

void MainWindow::on_Btn_LinkStop_clicked()
{
    ui->CMDLink_Box->setCurrentIndex(1);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_STOP;
    ScrewsRobot.SendData();
    Step = 0;
}

void MainWindow::on_Btn_LinkHalt_clicked()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
    Step = 0;
}

void MainWindow::on_Btn_Perapre_clicked()
{
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
    ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode1;
    ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 0;
    ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 0;
    ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = 0;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_CameraReady_clicked()
{
    bCameraReady = TRUE;
}

void MainWindow::on_Btn_ConnectDone_clicked()
{
    bConnectDone = TRUE;
}

void MainWindow::on_Btn_Collapse_clicked()
{
    bCollapse = TRUE;
}

void MainWindow::on_Btn_SingleStepDone_clicked()
{
    bSingleStepDone = TRUE;
}

void MainWindow::on_GetLocation_Btn_pressed()
{
    cameraObject->camera_isLocation = TRUE;
}

void MainWindow::on_Btn_Auto_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_Btn_Handle_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_Btn_AxisHandle_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}


//handle mode
void MainWindow::_onMotionModeTimout(){
    if(true == this->ScrewsRobot.m_CommState){
        switch (controlFlag) {
        case AllNone:
            break;
        case XMoveUP:
            ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode4;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = ui->LineEdit_moveVel->text().toDouble();
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 1.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 1.0;
            break;
        case XMoveDown:
            ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode4;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = ui->LineEdit_moveVel->text().toDouble();
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 1.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 2.0;
            break;
        case YMoveUp:
            ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode4;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = ui->LineEdit_moveVel->text().toDouble();
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 2.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 1.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = (int)RangeBeLimited;
            break;
        case YMOveDown:
            ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode4;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = ui->LineEdit_moveVel->text().toDouble();
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 2.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 2.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = (int)RangeBeLimited;
            break;
        case ZMoveUP:
            ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode4;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = ui->LineEdit_moveVel->text().toDouble();
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 3.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 1.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = (int)RangeBeLimited;
            break;
        case ZMoveDown:
            ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode4;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = ui->LineEdit_moveVel->text().toDouble();
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 3.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 2.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = (int)RangeBeLimited;
            break;
        case ThreeAxisMove:
            if(isNeedUpdataVar == true){
                ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode1;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = currentXPosi+13.8+ui->label_CameraX->text().toDouble();
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = currentYPosi+93.83+ui->label_CameraY->text().toDouble();
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = currentZPosi;
                recordObjPosiX = currentXPosi+13.8+ui->label_CameraX->text().toDouble();
                recordObjPosiY = currentYPosi+93.83+ui->label_CameraY->text().toDouble();
                isNeedUpdataVar = false;
            }
            else{
                ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode1;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = recordObjPosiX;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = recordObjPosiY;
                ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[2] = currentZPosi;
                if(currentXPosi == recordObjPosiX && currentYPosi == recordObjPosiY){
                isNeedUpdataVar = true;
                }
            }
            break;
        case NljMovePlus:
            ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode4;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = 0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 4.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 1.0;
            break;
        case NljMoveNeg:
            ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_MOV;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.eActMotionMode = eMotionMode4;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkVel[0] = 0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[0] = 4.0;
            ScrewsRobot.m_LinkCommd[0].stLinkKinPar.LinkPos[1] = 2.0;
            break;
        default:
            break;
        }
        ScrewsRobot.SendData();
    }
    else{
        ui->Label_bConnected->setText("FALSE");
        ui->Label_bConnected->setStyleSheet("color: red");
        return;
    }
}

void MainWindow::on_Btn_Moveup_X_pressed()
{
    controlFlag = XMoveUP;
}

void MainWindow::on_Btn_Moveup_X_released()
{
    controlFlag = AllNone;
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_MoveDown_X_pressed()
{
    controlFlag = XMoveDown;
}

void MainWindow::on_Btn_MoveDown_X_released()
{
    controlFlag = AllNone;
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_Moveup_Y_pressed()
{
    controlFlag = YMoveUp;
}

void MainWindow::on_Btn_Moveup_Y_released()
{
    controlFlag = AllNone;
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}


void MainWindow::on_Btn_MoveDown_Y_pressed()
{
    controlFlag = YMOveDown;
}

void MainWindow::on_Btn_MoveDown_Y_released()
{
    controlFlag = AllNone;
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_Moveup_Z_pressed()
{
    controlFlag= ZMoveUP;
}

void MainWindow::on_Btn_Moveup_Z_released()
{
    controlFlag = AllNone;
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_MoveDown_Z_pressed()
{
    controlFlag = ZMoveDown;
}

void MainWindow::on_Btn_MoveDown_Z_released()
{
    controlFlag = AllNone;
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_Moveup_NLJ_pressed()
{
    controlFlag = NljMovePlus;
}

void MainWindow::on_Btn_Moveup_NLJ_released()
{
    controlFlag = AllNone;
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_MoveDown_NLJ_pressed()
{
    controlFlag = NljMoveNeg;
}

void MainWindow::on_Btn_MoveDown_NLJ_released()
{
    controlFlag = AllNone;
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_Moveup_Z_2_pressed()
{
    controlFlag = ThreeAxisMove;
}

void MainWindow::on_Btn_Moveup_Z_2_released()
{
    controlFlag = AllNone;
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_PosLimit_clicked()
{
    if(ui->Btn_PosLimit->text() == "NotPosLimit"){
        ui->Btn_PosLimit->setText("PosLimit");
        ui->label_PosLimit->setText("Be Not PosLimit, you can recover your AXIS");
        RangeBeLimited = notBePosiLimitMode;
    }
    else if(ui->Btn_PosLimit->text() == "PosLimit"){
        ui->Btn_PosLimit->setText("NotPosLimit");
        ui->label_PosLimit->setText("Be PosLimit");
        RangeBeLimited = bePosiLimitMode;
    }
}



//downside is the single AXIS
void MainWindow::on_Btn_XBackword_pressed()
{
    int currentMotor = 0;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = -(ui->LineEdit_moveVel->text().toDouble());
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_XBackword_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_XForward_pressed()
{
    int currentMotor = 0;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = ui->LineEdit_moveVel->text().toDouble();
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_XForward_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_YLeftBackword_pressed()
{
    int currentMotor = 1;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = -(ui->LineEdit_moveVel->text().toDouble());
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_YLeftBackword_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_YLeftForward_pressed()
{
    int currentMotor = 1;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = ui->LineEdit_moveVel->text().toDouble();
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_YLeftForward_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_YRightBackward_pressed()
{
    int currentMotor = 2;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = -(ui->LineEdit_moveVel->text().toDouble());
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_YRightBackward_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_YRightForward_pressed()
{
    int currentMotor = 2;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = ui->LineEdit_moveVel->text().toDouble();
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_YRightForward_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_ZLeftBackward_pressed()
{
    int currentMotor = 3;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = -(ui->LineEdit_moveVel->text().toDouble());
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_ZLeftBackward_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_ZLeftForward_pressed()
{
    int currentMotor = 3;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = ui->LineEdit_moveVel->text().toDouble();
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_ZLeftForward_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_ZRightBackward_pressed()
{
    int currentMotor = 4;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = -(ui->LineEdit_moveVel->text().toDouble());
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_ZRightBackward_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_ZRightForward_pressed()
{
    int currentMotor = 4;
    ScrewsRobot.m_AxisCommd[currentMotor].Index = currentMotor + 1;
    ScrewsRobot.m_AxisCommd[currentMotor].eMC_Motion = eMC_MOV_VEL;
    ScrewsRobot.m_AxisCommd[currentMotor].bEnable = TRUE;
    ScrewsRobot.m_AxisCommd[currentMotor].Velocity = ui->LineEdit_moveVel->text().toDouble();
    ScrewsRobot.SendData();
}

void MainWindow::on_Btn_ZRightForward_released()
{
    ui->CMDLink_Box->setCurrentIndex(2);
    ScrewsRobot.m_LinkCommd[0].eLinkCommd = eLINK_HALT;
    ScrewsRobot.SendData();
}

