#include"Comm.h"
int CComm::ConnectToServer(char *IpAdr, int port)
{
	unsigned long ul = 1;
	int i = 0;
	while (i++ < CONNECT_TIMES)
	{
		WSAStartup(MAKEWORD(2, 2), &wsaData);
		m_SockClient = socket(AF_INET, SOCK_STREAM, 0);
		//ioctlsocket(m_SockClient, FIONBIO, (unsigned long *)&ul); //设置非阻塞模式
		inet_pton(AF_INET, IpAdr, &(m_AdrServer.sin_addr));
		m_AdrServer.sin_family = AF_INET;
		m_AdrServer.sin_port = htons(port);;

		int re;
		re = connect(m_SockClient, (SOCKADDR*)&m_AdrServer, sizeof(SOCKADDR));
		if (-1 == re)
		{
			int err = WSAGetLastError();
			printf("%d:%d\n", re, WSAGetLastError());
			if (err == WSAETIMEDOUT)
			{
				printf("connect time out,trying again!\n ");
				Sleep(1000);
				continue;
			}
			else
			{
				printf("connect err,give up\n ");
				close();
				break;
			}
			
		}
		else
		{
			int SendTimeout = 1000;   //1000ms
			int RecvTimeout = 1000;   //1000ms
			setsockopt(m_SockClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&RecvTimeout, sizeof(int));
			setsockopt(m_SockClient, SOL_SOCKET, SO_SNDTIMEO, (char *)&SendTimeout, sizeof(int));
			m_CommState = true;
			return 0;//连接成功
		}
	}
	return -1; //连接超时

}

int  CComm::Sendbuffer()
{
	if (false == m_CommState)
	{
		printf("no connection!\n");
		return -1;
	}
	//put data to buffer
    int sendnum = 0;
	ULONG datalen = 0;
	char sendbuff[DATA_SIZE];
          while ( datalen< m_SendSize)
	{
                memcpy( sendbuff, m_SendData + datalen,m_SendSize-datalen);
                sendnum = send(m_SockClient, sendbuff, m_SendSize - datalen, 0);
                datalen = datalen + sendnum;
		if (-1 == sendnum)
		{
			int err = WSAGetLastError();
			printf("%d\n", err);
			if (err == EAGAIN || err == EWOULDBLOCK || err == EINPROGRESS)
			{
				continue;
			}
			else if (err == WSAETIMEDOUT)
			{
				printf("Send time out");
			}
			else
			{
				close();
				return -1;
			}
			continue;
		}
		else if (0 == sendnum)
		{
			close();
			return -1;
		}

	}
	return 0;//发送完成
}
DINT  CComm::Recvbuffer()
{
	if (false == m_CommState)
	{
		printf("no connection!\n");
		return -1;
	}

	char recvbuff[DATA_SIZE];
	int recvnum = 0;
	ULONG datalen = 0;
	int cnt = 0;
	while (datalen < m_RecvSize)
	{
		recvnum = recv(m_SockClient, recvbuff, m_RecvSize - datalen, 0);

		if (-1 == recvnum)
		{
			int err = WSAGetLastError();
			printf("%d\n", err);
			if (err == EAGAIN || err == EWOULDBLOCK || err == EINTR)
			{
				continue;
			}
			else if(err == WSAETIMEDOUT)
			{
				cnt++;
				printf("recv time out \n");
				if (cnt > TIMEOUT_LIMIT)
				{
					printf("give up recieve once");
					return -2;
				}				
			}
			else
			{
				close();
				m_CommState = false;
				return -1;
			}
			continue;
		}
		else if (0 == recvnum)
		{
			close();
			return -1;
		}
		memcpy(m_RecvData + datalen, recvbuff, recvnum);
		datalen = datalen + recvnum;
	}
	return 0; //接收到一帧完整数据。

}

void CComm::close()
{
	printf("socket is closed \n");
	closesocket(m_SockClient);
	WSACleanup();
	m_CommState = false;
}

DINT RobotComm::RecvData()
{
	DINT re = Recvbuffer();
	if (0 != re)
	{
		printf("failed to recv\n");
		return -1;
	}
	//*******************************************************************************************************************
	//锁定变量，写入接收数据
	if (false == g_RecvLock.test_and_set())
	{
		ULONG offset = 0;
		memcpy(&m_RobotHeartbeat, m_RecvData + offset, sizeof(DINT));
		offset = offset + sizeof(DINT);
		memcpy(&m_RobotStaus, m_RecvData + offset, sizeof(stRobotStatus));
		offset = offset + sizeof(stRobotStatus);
                int jointcnt = 0;
		for (int i = 0; i < 6; i++)
		{
			if (0 != LINK_FREEDOM[i])
			{
                memcpy(&(m_LinkStatus[i]), m_RecvData + offset, sizeof(stLinkStatus));
                offset = offset + sizeof(stLinkStatus);
				for (int j = 0; j < LINK_FREEDOM[i]; j++)
				{
                    memcpy(&(m_AxisStatus[jointcnt]), m_RecvData + offset, sizeof(st_ReadAxis));
                    offset = offset + sizeof(st_ReadAxis);
                    jointcnt++;
                }
			}
		}
		g_RecvLock.clear();
	}
	else
	{
		//写入失败，直接丢弃数据
		;
	}
	
	//*********************************************************************************************************************
	return GetTime() - m_RobotHeartbeat;
	
}
DINT RobotComm::SendData()
{
	
	m_UpperHeartbeat = GetTime();
	ULONG offset = 0;
	//*********************************************************************************************************************
	//锁定要发送的数据，并转移数据
	if (false == g_SendLock.test_and_set()) 
	{
		memcpy(m_SendData + offset, &m_UpperHeartbeat, sizeof(DINT));
		offset = offset + sizeof(DINT);
		memcpy(m_SendData + offset, &m_RobotCmmd, sizeof(stRobotCommand));
		offset = offset + sizeof(stRobotCommand);
                int jointcnt = 0;
		for (int i = 0; i < 6; i++)
		{
			if (0 != LINK_FREEDOM[i])
			{
                memcpy(m_SendData + offset, &(m_LinkCommd[i]), sizeof(stLinkCommand));
                offset = offset + sizeof(stLinkCommand);
				for (int j = 0; j < LINK_FREEDOM[i]; j++)
				{
                    memcpy(m_SendData + offset, &(m_AxisCommd[jointcnt]), sizeof(st_SetAxis));
                    offset = offset + sizeof(st_SetAxis);
                    jointcnt++;
				}
			}
		}
        m_UpperHeartbeat = 0;
        memset(&m_RobotCmmd, 0, sizeof(stRobotCommand));
        memset(m_LinkCommd, 0, MAX_FREEDOM_LINK * sizeof(stLinkCommand));
        memset(m_AxisCommd, 0, MAX_FREEDOM_ROBOT * sizeof(st_SetAxis));

        //完成输出转移和清空，解除锁定
        g_SendLock.clear();

	}
	else
	{
		//写入数据失败，重试
		printf("failed to write，try again！\n");
		return -2;
	}
	//*********************************************************************************************************************

	//发送数据
	int re = Sendbuffer();
	if (0 == re)
	{
		return 0;
	}
	else
	{
		printf("failed to send\n");
		return -1;
	}	
}

DINT RobotComm::GetTime()
{
	GetLocalTime(&m_SysTime);
	//计时器，记录运行小时数，初值为0
	if (m_SysTime.wMinute > 30)
	{
		m_endhour = 1;
	}
	else
	{
		m_Hour = m_Hour + m_endhour;
		m_endhour = 0;
	}
	DINT acttime = m_Hour * 3600000
		+ m_SysTime.wMinute * 60000
		+ m_SysTime.wSecond * 1000
		+ m_SysTime.wMilliseconds;
	return acttime;
}



