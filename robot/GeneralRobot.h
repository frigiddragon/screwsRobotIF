/************************************************************/
/*�������ඨ��
/*                                                          */
/*                                                          */
/************************************************************/
#pragma once
#include "LinkObject.h"
#pragma pack(push)
#pragma pack(8) 
//------------------------------------------------------------------------------//
//Robot class
//------------------------------------------------------------------------------//
class CGeneralRobot 
{
public:
	CGeneralRobot();
	~CGeneralRobot();

	ERR_ID Init(OUT st_InitParameter *pParmeter );															//�����˳�ʼ��
	void Run(st_AxisGroupRead *MC_status, st_AxisGroupSet *MC_SetCmd, Recvbuff *RecvData, Sendbuff *SendData);															//����������

protected:
	void GetStatus();					//��ȡ������״̬
	void GetCommd();					//�����˿���ָ��
	void StateMachine();				//������״̬��
	void SetFeedback();					//�����˷���״̬
	void SetAction();					//�������˶�ָ��

	void JugeRobotState();				//�жϻ�����״̬
	void SetRobotAction() {}
	void RobotStateMachine();

private:
	virtual void ActionReset();
	virtual void ActionPower();
	virtual void ActionHome();
	virtual void ActionStop();
	virtual void ActionHalt();
	virtual void ActionMove();

	virtual void Clear();

public:
	
	CLink*			m_links[6];

	int				m_LinksNum;
	int				m_JointsNum;

	stAction		m_SetAction;
	stAction		m_ActAction;
	stRobotCommand	m_Commd;
	stRobotStatus	m_Status;

protected:
	DINT			m_Heartbeat;
	LONG			m_Lost;
	
	st_AxisGroupRead	m_stAxisGroupStatus;
	st_AxisGroupSet		m_stAxisGroupSet;
	Recvbuff		m_RecvData;
	Sendbuff		m_SendData;

private:
	stJointPara		m_JointPar[MAX_FREEDOM_ROBOT];
	
};

#pragma pack(pop)