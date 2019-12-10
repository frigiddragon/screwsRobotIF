/****************************************************************/
/*���ܣ��ƶ�������
/*�����ߣ�AT
/*�ش�Ķ���ʱ�䣺
2017.5.14��  ����
*****************************************************************/
#pragma once
#include <stdlib.h>
#include "Axis.h"
#pragma pack(push)
#pragma pack(8) 

class CLink
{
public:
	CLink(int linkindex, int freedom);
	~CLink();
public:
	ERR_ID Init(int JointIndex, stJointPara JointPar[]);											//�����ʼ��
	void GetStatus(_IN st_AxisGroupRead* stAxisGroupStatus);
	void StateMachine();				 									//״̬�������к�����
	void SetAction(OUT st_AxisGroupSet*	stAxisGroupSet);					//������ָ��
	
	//LINK�����ӿ�----------------------------------------------------------------------//
	void CommdReset();
	void CommdPower(bool enable);
	void CommdHome();
	void CommdStop();
	void CommdHalt();
	void CommdMove();

protected:
	void JugeLinkState();
	void LinkStateMachine();
	

	//�����غ���----------------------------------------------------------------------//
	//����
	virtual void LinkForwardKin() const {}			//Link ����
	//���
	virtual void MotionLineAbsolute() const{}	//��ֱ���˶�������λ��	
	virtual void MotionLineVelocity() const{}	//ĩ���ٶ��˶�
	virtual void MotionLineRelative() const {}	//���λ��ģʽ
	virtual void MotionMode1() const {}			//�Զ���1
	virtual void MotionMode2() const {}			//�Զ���2
	virtual void MotionMode3() const {}			//�Զ���3
	virtual void MotionMode4() const {}			//�Զ���4
	virtual void MotionMode5() const {}			//�Զ���5
	virtual void MotionMode6() const {}			//�Զ���6
	virtual void MotionMode7() const {}			//�Զ���7
	virtual void MotionMode8() const {}			//�Զ���8
	virtual void MotionMode9() const {}			//�Զ���9
	
	virtual void SetLinkSelfAction() const{}		//Link��������ĩ�˹��ߣ�ˮ��ǹ�߼�����

private:
	virtual void ActionReset();
	virtual void ActionPower();
	virtual void ActionHome();
	virtual void ActionStop();
	virtual void ActionHalt();
	virtual void ActionMove();

public:
	CAxis*			m_Joints[MAX_FREEDOM_LINK];
	int				m_Index;			//��˳���������
	int				m_Freedom;

	stLinkStatus	m_Status;
	stLinkCommand	m_Command;
	stAction		m_SetAction;		//PLCִ��
	stAction		m_ActAction;		//PLC��ǰ����
private:
	stLinkCommand	m_preCommand;
};
#pragma pack(pop)