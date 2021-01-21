#include "stdafx.h"
#include "NetMsgImp.h"


void CNetMsgImp::ServerListen(NET_SERVER_LISTEN_CONNECTION_DATA * pConn)
{
	pConn->sock = OpenSocket();
	pConn->sockType = SOCK_TYPE_SERVER_LISTEN;
	BindSocket(pConn->sock , m_serverCfg.bindPort);
	CreateIoCompletionPort((HANDLE)pConn->sock , m_hIocp , (ULONG_PTR)pConn , 0);
	listen(pConn->sock , 10);
}

void CNetMsgImp::ServerQueueAccept(NET_SERVER_LISTEN_CONNECTION_DATA * pConn)
{
	NET_COMM_CONNECTION_DATA * p = m_serverConnTab;
	for(int i = 0 ; i < MAX_SERVER_CONNECTION ; ++i , ++p)
	{
		ServerQueueOneAccept(p);
	}
}

void CNetMsgImp::ServerQueueOneAccept(NET_COMM_CONNECTION_DATA * p)
{
	NET_SERVER_LISTEN_CONNECTION_DATA * pConn = &m_serverListen;
	if(p->sock == 0)
	{
		p->sock = OpenSocket();
		p->sockType = SOCK_TYPE_SERVER_CONN;
		p->sockStatus = SOCK_STATUS_UNCONNECTED;
		p->stationID = -1;
		lpfnAcceptEx(pConn->sock , p->sock , pConn->acceptBuf , 0 , sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 
			&p->sendOvlp.dwBytes , &p->sendOvlp);
	}
}

void CNetMsgImp::CloseServerConn(NET_COMM_CONNECTION_DATA * pConn)
{
	if(pConn->sock)
	{
		closesocket(pConn->sock);
		m_hook->OnServerStationDisconnected(m_userData , pConn->stationID);
		pConn->sock = 0;
		pConn->stationID = -1;
		pConn->sockStatus = SOCK_STATUS_CLOSED;
		pConn->lastCloseTime = GetTime();
		
	}
}

int CNetMsgImp::SendMsgToClient(int stationID , NET_MSG_PACKET_HEAD * msg , BYTE * packetData , int flag)
{
	NET_COMM_CONNECTION_DATA * pConn = m_serverConnTab;
	int count = 0;
	for(int i = 0 ; i < MAX_SERVER_CONNECTION ; ++i , ++pConn)
	{
		if(pConn->sockStatus == SOCK_STATUS_CONNECTED)
		{
			if(stationID == -1 || stationID == pConn->stationID)
			{
				if((flag & NET_MSG_SEND_MSG_FLAG_NO_DATA_COPY) == 0)
				{
					pConn->sendPacket = *msg;
					if(msg->dataLen > pConn->sendBufLen)
					{
						pConn->sendBuf = (BYTE *)realloc(pConn->sendBuf , msg->dataLen);
						pConn->sendBufLen = msg->dataLen;
					}
					memcpy(pConn->sendBuf , packetData , msg->dataLen);
					pConn->sendPacketPtr = &pConn->sendPacket;
					pConn->sendData = pConn->sendBuf;
				}
				else
				{
					pConn->sendPacketPtr = msg;
					pConn->sendData = packetData;
				}
				SendPacket(pConn);
				count++;
			}
		}
	}
	return count;
}
