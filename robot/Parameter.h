#pragma once
#pragma pack(push)
#pragma pack(8) 
#include <stdio.h>//�޸�
#include <string.h>

const LONG MAX_FREEDOM_LINK = 10;			//Link ������ɶ�
const LONG MAX_FREEDOM_ROBOT = 20;			//robot������ɶ�
const LONG LOST_COMM_THRESHOLD = 500;		//ͨѶ��ʱ������
const double MIN_VEL_LIMT = 0.001;			//��λ ��/s
const double MINIUM = exp(-10);
const double MAX = 1.7*exp(308);
const double MIN = 1.7*exp(308);
//------------------------------------------------------------------------------//
//LINK���ɶ�����
//------------------------------------------------------------------------------//
const LONG LINK_FREEDOM[6] = { 7, 0, 0, 0, 0, 0 };

//------------------------------------------------------------------------------//
//����λ
//------------------------------------------------------------------------------//
const double LINK_0_JOINT_LIMIT_POS[MAX_FREEDOM_LINK] = { 10,20,30,40,50,60,70,0,0,0 };
const double LINK_0_JOINT_LIMIT_NEG[MAX_FREEDOM_LINK] = { -10,-20,-30,-40,-50,-60,70,0,0,0 };
//const double LINK_0_JOINT_LIMIT_POS[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
//const double LINK_0_JOINT_LIMIT_NEG[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_1_JOINT_LIMIT_POS[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_1_JOINT_LIMIT_NEG[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_2_JOINT_LIMIT_POS[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_2_JOINT_LIMIT_NEG[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_3_JOINT_LIMIT_POS[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_3_JOINT_LIMIT_NEG[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_4_JOINT_LIMIT_POS[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_4_JOINT_LIMIT_NEG[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_5_JOINT_LIMIT_POS[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_5_JOINT_LIMIT_NEG[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };

//------------------------------------------------------------------------------//
//����ٶ�
//------------------------------------------------------------------------------//
const double LINK_0_JOINT_MAX_VEL[MAX_FREEDOM_LINK] = { 10,20,30,40,50,60,0,0,0,0 };
//const double LINK_0_JOINT_MAX_VEL[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_1_JOINT_MAX_VEL[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_2_JOINT_MAX_VEL[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_3_JOINT_MAX_VEL[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_4_JOINT_MAX_VEL[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_5_JOINT_MAX_VEL[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };

//------------------------------------------------------------------------------//
//���ٱ�
//------------------------------------------------------------------------------//
const double LINK_0_JOINT_RATIO[MAX_FREEDOM_LINK] = { 1,1,1,1,1,1,0,0,0,0 };
//const double LINK_0_JOINT_RATIO[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_1_JOINT_RATIO[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_2_JOINT_RATIO[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_3_JOINT_RATIO[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_4_JOINT_RATIO[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_5_JOINT_RATIO[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };

//------------------------------------------------------------------------------//
//�������
//------------------------------------------------------------------------------//
const LONG LINK_0_JOINT_DIRECTION[MAX_FREEDOM_LINK] = {1,1,1,1,1,1,0,0,0,0 };
//const LONG LINK_0_JOINT_DIRECTION[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const LONG LINK_1_JOINT_DIRECTION[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const LONG LINK_2_JOINT_DIRECTION[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const LONG LINK_3_JOINT_DIRECTION[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const LONG LINK_4_JOINT_DIRECTION[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const LONG LINK_5_JOINT_DIRECTION[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };

//------------------------------------------------------------------------------//
//������ƫ��
//------------------------------------------------------------------------------//
const double LINK_0_JOINT_ENCODER_CORR[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_1_JOINT_ENCODER_CORR[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_2_JOINT_ENCODER_CORR[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_3_JOINT_ENCODER_CORR[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_4_JOINT_ENCODER_CORR[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };
const double LINK_5_JOINT_ENCODER_CORR[MAX_FREEDOM_LINK] = { 0,0,0,0,0,0,0,0,0,0 };

#pragma pack(pop)
