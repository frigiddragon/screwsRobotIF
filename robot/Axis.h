/*************************************************************************/
/*���ܣ����������
/*������:AT
/*�ش�Ķ���ʱ��:
2017 5 12

*************************************************************************/
#pragma once
#include "robot/RobotModuleServices.h"

#include "DataStruct.h"
#pragma pack(push)
#pragma pack(8) 

class CAxis
{

public:
	CAxis();
	~CAxis();

	ERR_ID Init(int index, _IN stJointPara JointPar[]);							//���ʼ��
	void GetStatus(_IN st_AxisGroupRead* stAxisGroupStatus);
	void StateMachine();				 				//���״̬����PLC���
	void SetAction(OUT st_AxisGroupSet*	stAxisGroupSet);								//����ָ��

	//Axis�����ӿ�----------------------------------------------------------------------//
	void CommdReset();
	void CommdPower(bool power);
	void CommdHome(double distance = 0);
	void CommdStop();
	void CommdHalt();
	void CommdMove(st_SetAxis Axiscommd);

private:
	bool DealwithLimit(st_SetAxis Axiscommd);


public:
	int				m_index;
	bool			m_homed;
	st_ReadAxis		m_Status;
	st_SetAxis		m_Commd;		//cmd from upper

	st_SetAxis		m_SetAction;	//action to pcl_nc
private:
	double			m_PositiveLimit;
	double			m_NegtiveLimit;
	double			m_VelocityLimit;
	LONG			m_Direction;			//����������᷽��һ����
	double			m_Radio;
	double			m_EncoderCorr;


};
#pragma pack(pop)
