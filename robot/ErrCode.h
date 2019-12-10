/*********************************************************************/
/*���ܣ��������ö������
/*�����ߣ�
/*�ش�Ķ���ʱ�䣺
2016.3.30��������
********************************************************************/
#pragma once
#pragma pack(push)
#pragma pack(8) 
#include <stdio.h>//�޸�
#include <string.h>
#include "robot/RobotModuleServices.h"

enum ERR_ID :ULONG
{
	eNoErr = 0,//�޴���

	eErrorAxisPositionLimit = 10001,
	eErrorAxisVelocityLimit,
	eErrorLinkInstance,
	eErrorJointInstance,
	eErrorUnkownLinkIndex,

	eErrorJointLimtPar,
	eErrorJointMaxVel,
	eErrorJointRadio,
	eErrorJointDirection,

	
	eWarningErrorState		= 20001,
	eWarningRobotDisabled,
	eWarningUnknownCommand,
	eWarningDisableWhenStopping,
	eWarningRobotIsStopping,
	eWarningDisableWhenHoming,
	eWarningHomingInterrupted,
	eWarningRobotIsHoming,
	eWarningDisableWhenMoving,
	eWarningRobotIsMoving	

};

//------------------------------------------------------------------------------//
//class ������в��� 
//------------------------------------------------------------------------------//
#define ERR_QUE_SIZE 100

class CErrorQueue
{
public:
	CErrorQueue();
	~CErrorQueue();

	bool Enter_Queue(ERR_ID newErr);				//���

	ULONG Out_Queue(ERR_ID ErrQue[], ULONG len = 10);
	ULONG Clean_Queue(ULONG len = 10);//���

private:
	void Set_NULL();				//�ÿն���

	bool Is_NULL();					//�ж϶����Ƿ�Ϊ��

private:

	ERR_ID m_ErrQue[ERR_QUE_SIZE];
	LONG m_front;				//��һ�������ݵ�����Ԫ��
	LONG m_rear;				//��β�ͺ��һ����λ��
};

extern CErrorQueue g_ErrQue;
#pragma pack(pop)
