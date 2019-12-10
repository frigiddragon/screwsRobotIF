/***************************************************************************
/*���ܣ��������ݽṹ��
/*�ش�Ķ���ʱ�䣺
2016.3.30�� ����
****************************************************************************/
#pragma once
#pragma pack(push)
#pragma pack(8) 
//�޸�

#include"Parameter.h"
#include "ErrCode.h"

#define myDEBUG

#define OUT
#define _IN	const
#define INOUT
#define  Pi 3.1415926

#define  sq(x) ((x)*(x))

#define D_SafeDelete(p)		if(p)		{ delete p; p = NULL; }

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __myLOC__ __FILE__ "("__STR1__(__LINE__)")"
#define _warning(iCode, msgStr) message( __myLOC__ " : warning C" __STR1__(iCode) ": " msgStr )


//------------------------------------------------------------------------------//
//Link ����ģʽ
//------------------------------------------------------------------------------//
enum E_MotionMode
{
	eMotionLineAbsolute = 0,
	eMotionLineVelocity,
	eMotionLineRelative,

	eMotionMode1,
	eMotionMode2,
	eMotionMode3,
	eMotionMode4,
	eMotionMode5,
	eMotionMode6,
	eMotionMode7,
	eMotionMode8,
	eMotionMode9,
};

//------------------------------------------------------------------------------//
//Link State
//------------------------------------------------------------------------------//
enum E_LinkState
{
	eLINK_ERRORSTOP = 0,
	eLINK_DISABLED,
	eLINK_STANDSTILL,
	eLINK_STOPPING,
	eLINK_HOMING,
	eLINK_MOVING
};
//------------------------------------------------------------------------------//
//Link kinetic
//------------------------------------------------------------------------------//
typedef struct ParameterofKinetic
{
	E_MotionMode	eActMotionMode;
	bool			bBusy;
	bool			bDone;
	bool			bEnable;
	double			LinkPos[MAX_FREEDOM_LINK];
	double			LinkVel[MAX_FREEDOM_LINK];
}stKineticPar;
//------------------------------------------------------------------------------//
//Link Reserve data
//------------------------------------------------------------------------------//
typedef struct DataofReserve
{
	bool			bReserve[4];
	LONG			lReserve[4];
	double			fReserve[4];
}stReserveData;

//------------------------------------------------------------------------------//
//Link Motion Commd
//------------------------------------------------------------------------------//
enum E_LinkCommd
{
	eLINK_NONE = 0,
	eLINK_RESET,
	eLINK_HOME,
	eLINK_POWER,
	eLINK_STOP,
	eLINK_HALT,
	eLINK_MOV
};
//------------------------------------------------------------------------------//
//Link Input CMD
//------------------------------------------------------------------------------//
typedef struct CommandtoLink
{
	E_LinkCommd		eLinkCommd;
	stKineticPar	stLinkKinPar;
	stReserveData	stResverInput;
}stLinkCommand;

//------------------------------------------------------------------------------//
//Link Status
//------------------------------------------------------------------------------//
typedef struct StatusofLink
{
	E_LinkState		eLinkActState;	//Link״̬
	stKineticPar	stLinkActKin;	//�˶�����
	stReserveData	stResverOutput;
}stLinkStatus;



//------------------------------------------------------------------------------//
//Robot State
//------------------------------------------------------------------------------//
enum E_RobotState
{
	eROBOT_ERRORSTOP = 0,
	eROBOT_DISABLED,
	eROBOT_STANDSTILL,
	eROBOT_STOPPING,
	eROBOT_HOMING,
	eROBOT_MOVING
};

//------------------------------------------------------------------------------//
//Robot Motion Commd
//------------------------------------------------------------------------------//
enum E_RobotCommd
{
	eROBOT_NONE = 0,
	eROBOT_RESET,
	eROBOT_HOME,
	eROBOT_POWER,
	eROBOT_STOP,
	eROBOT_HALT,
	eROBOT_MOV
};
//------------------------------------------------------------------------------//
//Robot Input CMD
//------------------------------------------------------------------------------//
typedef struct CommandtoRobot
{
	E_RobotCommd	eCommd;
	stKineticPar	stKinPar;
	ULONG			ErrNum;
	stReserveData	stResverInput;
}stRobotCommand;

//------------------------------------------------------------------------------//
//Robot Status
//------------------------------------------------------------------------------//
typedef struct StatusofRobot
{
	E_LinkState		eActState;	//Link״̬
	stKineticPar	stActKin;	//�˶�����	
	ERR_ID			ErrList[10];	//�����б�
	ULONG			ErrNum;			//��������
	stReserveData	stResverOutput;
}stRobotStatus;


//------------------------------------------------------------------------------//
//JOINT PARAMETER
//------------------------------------------------------------------------------//
typedef struct ParameterofJoint
{
	double		Limit_Positive;
	double		Limit_Negtive;
	double		Max_Vel;
	double		Ration;
	LONG		Direction;
	double		EncoderCorr;
}stJointPara;

//---------------------------------------------�����������
class AnyType
{
public:
	template<typename RealDataType>
	AnyType(RealDataType* data)										//���캯������ʼ��˽�б���m_RealDataΪdata
		: m_RealData(data)
	{}
	template<typename RealDataType>									//����RealDataTypeģ��
	RealDataType& GetRealDataType()
	{
		//////D_CHECK(m_RealData);
		return *reinterpret_cast<RealDataType*>(m_RealData);		//��m_RealDataָ�����������޸�ΪRealDataType
	}

	template<typename RealDataType>
	const RealDataType& GetRealDataType() const
	{
		//////D_CHECK(m_RealData);
		return *reinterpret_cast<const RealDataType*>(m_RealData);
	}

	template<typename RealDataType>
	AnyType& operator =(RealDataType* _Data)
	{
		Set<RealDataType>(_Data);
		return *this;
	}
private:
	template<typename RealDataType>
	void Set(RealDataType* _Data)
	{
		m_RealData = reinterpret_cast<void*>(_Data);
	}

private:
	void* m_RealData;
};

typedef AnyType InputParam;
typedef AnyType OutputParam;

#pragma pack(pop)
