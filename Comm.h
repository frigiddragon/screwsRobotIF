#pragma once
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <time.h>
#include <atomic>
#pragma comment(lib,"ws2_32.lib")

#include<QDebug>
#include "robot/DataStruct.h"

#define CONNECT_TIMES 1000
#define TIMEOUT_LIMIT 10			//接收超时次数上相
#define DATA_SIZE 5000



class CComm
{
public:
	CComm()
	{		
        m_CommState = false;
		memset(m_SendData, 0, DATA_SIZE); 
		memset(m_RecvData, 0, DATA_SIZE);
		m_SendSize = DATA_SIZE;
		m_RecvSize = DATA_SIZE;
	}
	~CComm()
	{
		close();
	}
	 
	int ConnectToServer(char *IpAdr, int port);
	int Sendbuffer();
	DINT Recvbuffer();
	void close();

	bool		m_CommState;
	ULONG		m_SendSize;
	ULONG		m_RecvSize;
protected:
	char		m_RecvData[DATA_SIZE];
	char		m_SendData[DATA_SIZE];
private:
	SOCKET		m_SockClient;//客户端Socket
	SOCKADDR_IN m_AdrServer;//服务端地址
	WSADATA wsaData;
};

class RobotComm:public CComm
{
public:
	RobotComm() :CComm() 
	{
		for (int i = 0; i < 6; i++)
		{
			if (0 != LINK_FREEDOM[i])
			{
				m_LinkNum++;
				m_JointNum = m_JointNum + LINK_FREEDOM[i];
			}
		}
		m_Hour = 0;
		m_endhour = 0;
		m_SysTime = { 0 };
                m_SendSize = sizeof(DINT) + sizeof(stRobotCommand) + m_LinkNum * sizeof(stLinkCommand) + m_JointNum * sizeof(st_SetAxis);
		m_RecvSize = sizeof(DINT) + sizeof(stRobotStatus) + m_LinkNum * sizeof(stLinkStatus) + m_JointNum * sizeof(st_ReadAxis);
		if (m_SendSize > DATA_SIZE || m_RecvSize>m_RecvSize)
		{
			printf("Error recv length is bigger than DATA_SIZE\n");
		}

                g_RecvLock.clear();
                g_SendLock.clear();

	}
	~RobotComm(){}

	DINT RecvData();
	DINT SendData();
	DINT GetTime();

public:
	//接收到的机器人状态
	DINT			m_RobotHeartbeat;
	stRobotStatus	m_RobotStaus;
	stLinkStatus	m_LinkStatus[MAX_FREEDOM_LINK];
	st_ReadAxis		m_AxisStatus[MAX_FREEDOM_ROBOT];
	//要发送的控制指令
	DINT			m_UpperHeartbeat;
	stRobotCommand	m_RobotCmmd;
	stLinkCommand	m_LinkCommd[MAX_FREEDOM_LINK];
	st_SetAxis		m_AxisCommd[MAX_FREEDOM_ROBOT];



private:
	ULONG			m_LinkNum;
	ULONG			m_JointNum;
	SYSTEMTIME		m_SysTime;
	ULONG			m_Hour;
	ULONG			m_endhour;
    std::atomic_flag        g_RecvLock;
    std::atomic_flag        g_SendLock;
};
