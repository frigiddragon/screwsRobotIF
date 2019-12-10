#pragma once
#include "Link.h"
#pragma pack(push)
#pragma pack(8) 

const double MoveVel = 10; //��������Ķ��廹��Ҫ��һ������


class CLink_0 : public CLink
{
	CLink_0(int linkindex, int freedom):CLink(linkindex, freedom)
	{
		X_CameraToScrew = 0;
		Y_CameraToScrew = 0;
		Z_FirstDistance = 0;
		Z_SecondDistance = 0;
		beConnect = FALSE;

	}
	~CLink_0()
	{}
	//�����غ���====================================================//
	//����
	virtual void LinkForwardKin() ;			//Link ����
	//���
	virtual void MotionLineAbsolute() ;			//�����˶�	
	virtual void MotionLineVelocity() ;			//ĩ��λ��ģʽ
	virtual void MotionLineRelative() ;			//ĩ���ٶ�ģʽ
	
	virtual void MotionMode1();  			//X.Y.Z�˶�
	virtual void MotionMode2();				//�ֶ�ģʽ
	virtual void MotionMode3() const {}			//�Զ���3
	virtual void MotionMode4() const {}			//�Զ���4
	virtual void MotionMode5() const {}			//�Զ���5
	virtual void MotionMode6() const {}			//�Զ���6
	virtual void MotionMode7() const {}			//�Զ���7
	virtual void MotionMode8() const {}			//�Զ���8
	virtual void MotionMode9() const {}			//�Զ���9

	virtual void SetLinkSelfAction() const;		//Link��������ĩ�˹��ߣ�ˮ��ǹ

public:
	double X_CameraToScrew;
	double Y_CameraToScrew;
	double Z_FirstDistance;
	double Z_SecondDistance;
	BOOL beConnect;


};

class CLink_1 : public CLink
{
	CLink_1(int linkindex, int freedom):CLink(linkindex, freedom)
	{}
	~CLink_1()
	{}
	//�����غ���====================================================//
	//����
	virtual void LinkForwardKin() const;			//Link ����
	//���
	virtual void MotionLineAbsolute() const;			//�����˶�	
	virtual void MotionLineVelocity() const;			//ĩ��λ��ģʽ
	virtual void MotionLineRelative() const;			//ĩ���ٶ�ģʽ
	
	virtual void MotionMode1() const {}			//�Զ���1
	virtual void MotionMode2() const {}			//�Զ���2
	virtual void MotionMode3() const {}			//�Զ���3
	virtual void MotionMode4() const {}			//�Զ���4
	virtual void MotionMode5() const {}			//�Զ���5
	virtual void MotionMode6() const {}			//�Զ���6
	virtual void MotionMode7() const {}			//�Զ���7
	virtual void MotionMode8() const {}			//�Զ���8
	virtual void MotionMode9() const {}			//�Զ���9

	virtual void SetLinkSelfAction() const;			//Link��������ĩ�˹��ߣ�ˮ��ǹ

};
class CLink_2 : public CLink
{
	CLink_2(int linkindex, int freedom) :CLink(linkindex, freedom)
	{}
	~CLink_2()
	{}
	//�����غ���====================================================//
	//����
	virtual void LinkForwardKin() const;			//Link ����
											//���
	virtual void MotionLineAbsolute() const;			//�����˶�	
	virtual void MotionLineVelocity() const;			//ĩ��λ��ģʽ
	virtual void MotionLineRelative() const;			//ĩ���ٶ�ģʽ
	
	virtual void MotionMode1() const {}			//�Զ���1
	virtual void MotionMode2() const {}			//�Զ���2
	virtual void MotionMode3() const {}			//�Զ���3
	virtual void MotionMode4() const {}			//�Զ���4
	virtual void MotionMode5() const {}			//�Զ���5
	virtual void MotionMode6() const {}			//�Զ���6
	virtual void MotionMode7() const {}			//�Զ���7
	virtual void MotionMode8() const {}			//�Զ���8
	virtual void MotionMode9() const {}			//�Զ���9

	virtual void SetLinkSelfAction() const;			//Link��������ĩ�˹��ߣ�ˮ��ǹ

};
class CLink_3 : public CLink
{
	CLink_3(int linkindex, int freedom) :CLink(linkindex, freedom)
	{}
	~CLink_3()
	{}
	//�����غ���====================================================//
	//����
	virtual void LinkForwardKin() const;			//Link ����
											//���
	virtual void MotionLineAbsolute() const;			//�����˶�	
	virtual void MotionLineVelocity() const;			//ĩ��λ��ģʽ
	virtual void MotionLineRelative() const;			//ĩ���ٶ�ģʽ
	
	virtual void MotionMode1() const {}			//�Զ���1
	virtual void MotionMode2() const {}			//�Զ���2
	virtual void MotionMode3() const {}			//�Զ���3
	virtual void MotionMode4() const {}			//�Զ���4
	virtual void MotionMode5() const {}			//�Զ���5
	virtual void MotionMode6() const {}			//�Զ���6
	virtual void MotionMode7() const {}			//�Զ���7
	virtual void MotionMode8() const {}			//�Զ���8
	virtual void MotionMode9() const {}			//�Զ���9

	virtual void SetLinkSelfAction() const;			//Link��������ĩ�˹��ߣ�ˮ��ǹ
};
class CLink_4 : public CLink
{
	CLink_4(int linkindex, int freedom) :CLink(linkindex, freedom)
	{}
	~CLink_4()
	{}
	//�����غ���====================================================//
	//����
	virtual void LinkForwardKin() const ;			//Link ����
											//���
	virtual void MotionLineAbsolute() const;			//�����˶�	
	virtual void MotionLineVelocity() const;			//ĩ��λ��ģʽ
	virtual void MotionLineRelative() const;			//ĩ���ٶ�ģʽ
	
	virtual void MotionMode1() const {}			//�Զ���1
	virtual void MotionMode2() const {}			//�Զ���2
	virtual void MotionMode3() const {}			//�Զ���3
	virtual void MotionMode4() const {}			//�Զ���4
	virtual void MotionMode5() const {}			//�Զ���5
	virtual void MotionMode6() const {}			//�Զ���6
	virtual void MotionMode7() const {}			//�Զ���7
	virtual void MotionMode8() const {}			//�Զ���8
	virtual void MotionMode9() const {}			//�Զ���9

	virtual void SetLinkSelfAction() const;			//Link��������ĩ�˹��ߣ�ˮ��ǹ
};
class CLink_5 : public CLink
{
	CLink_5(int linkindex, int freedom) :CLink(linkindex, freedom)
	{}
	~CLink_5()
	{}
	//�����غ���====================================================//
	//����
	virtual void LinkForwardKin() const;			//Link ����
											//���
	virtual void MotionLineAbsolute() const;			//�����˶�	
	virtual void MotionLineVelocity() const;			//ĩ��λ��ģʽ
	virtual void MotionLineRelative() const;			//ĩ���ٶ�ģʽ
	
	virtual void MotionMode1() const {}			//�Զ���1
	virtual void MotionMode2() const {}			//�Զ���2
	virtual void MotionMode3() const {}			//�Զ���3
	virtual void MotionMode4() const {}			//�Զ���4
	virtual void MotionMode5() const {}			//�Զ���5
	virtual void MotionMode6() const {}			//�Զ���6
	virtual void MotionMode7() const {}			//�Զ���7
	virtual void MotionMode8() const {}			//�Զ���8
	virtual void MotionMode9() const {}			//�Զ���9

	virtual void SetLinkSelfAction() const;			//Link��������ĩ�˹��ߣ�ˮ��ǹ
};

#pragma pack(pop)