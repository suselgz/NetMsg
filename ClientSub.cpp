#include "stdafx.h"
#include "NetMsgImp.h"

int CNetMsgImp::OpenConnection(LPCTSTR serverIP , int serverPort , int userID)
{
	NET_COMM_CONNECTION_DATA * p = m_clientConnTab;
	for(int i = 0 ; i < MAX_CLIENT_CONNECTION ; ++i , ++p)
	{
		if(p->sockStatus == SOCK_STATUS_EMPTY)
		{
			p->sockStatus = SOCK_STATUS_UNCONNECTED;
			p->connUserID = userID;
			p->peerAddr.sin_family = AF_INET;
			p->peerAddr.sin_addr.s_addr = inet_addr(serverIP);
			p->peerAddr.sin_port = htons(serverPort);
			p->heartBeatPacket.stationID = m_clientCfg.stationID;
			p->heartBeatPacket.type = NET_MSG_PACKET_TYPE_HEART_BEAT;
			p->lastCloseTime = GetTime() - m_clientCfg.reconnectDelay;
			p->cid = i + 1;
			return p->cid;
		}
	}
	return 0;
}

void CNetMsgImp::CloseConnection(int cid)
{
	int index = cid - 1;
	if((unsigned)index < MAX_CLIENT_CONNECTION)
	{
		NET_COMM_CONNECTION_DATA * p = m_clientConnTab + index;
		if(p->sockStatus != SOCK_STATUS_EMPTY)
		{
			p->sockStatus = SOCK_STATUS_EMPTY;
			CloseClient(p , GetTime());
		}
	}
}

void CNetMsgImp::ConnectClientToServer(NET_COMM_CONNECTION_DATA * pConn)
{
	pConn->sock = OpenSocket();
	CreateIoCompletionPort((HANDLE)pConn->sock , m_hIocp , (ULONG_PTR)pConn , 0);
	m_hook->OnClientConnectingToServer(m_userData , pConn->cid , pConn->connUserID);
	pConn->sendOvlp.op = OPERATION_TYPE_CONNECT;
	pConn->sockStatus = SOCK_STATUS_CONNECTING;
	pConn->connectingTime = GetTime();
	BindSocket(pConn->sock, 0);
	fnConnectEx(pConn->sock, (SOCKADDR*) &pConn->peerAddr, sizeof(pConn->peerAddr) , 0 , 0 , 0 , &pConn->sendOvlp);
}

void CNetMsgImp::CloseClient(NET_COMM_CONNECTION_DATA * pConn , DWORD closeTime)
{
	if(pConn->sock)
	{
		closesocket(pConn->sock);
		pConn->sock = 0;
		if (pConn->sockStatus == SOCK_STATUS_CONNECTING)
		{
			pConn->sockStatus = SOCK_STATUS_UNCONNECTED;
			pConn->lastCloseTime = closeTime;
		}
		else
		{
			pConn->sockStatus = SOCK_STATUS_UNCONNECTED;
			pConn->lastCloseTime = closeTime;
			m_hook->OnClientDisconnectedFromServer(m_userData , pConn->cid , pConn->connUserID);
		}
	}
}

int  CNetMsgImp::SendMsgToServer(int cid , NET_MSG_PACKET_HEAD * msg  , BYTE * packetData , int flag) 
{
	int index = cid - 1;
	if((unsigned)index < MAX_CLIENT_CONNECTION)
	{
		NET_COMM_CONNECTION_DATA * pConn = m_clientConnTab + index;
		if(pConn->sockStatus == SOCK_STATUS_CONNECTED)
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
			return 0;
		}
	}
	return -1;
}