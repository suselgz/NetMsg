#include "stdafx.h"
#include "NetMsgImp.h"


void CNetMsgImp::ProcessSocketError(NET_COMM_CONNECTION_DATA * pConn , NET_COMM_OVERLAPPED * po , DWORD dwBytes)
{
	DWORD tick = GetTime();
	switch(pConn->sockType)
	{
	case SOCK_TYPE_CLIENT_CONN:
		CloseClient(pConn , tick);
		break;
	case SOCK_TYPE_SERVER_LISTEN:
		break;
	case SOCK_TYPE_SERVER_CONN:
		CloseServerConn(pConn);
		break;
	}
}


void CNetMsgImp::ProcessTimeout()
{
	DWORD tick = GetTime();
	int difTime , i;
	NET_COMM_CONNECTION_DATA * pConn = m_clientConnTab;
	for(i = 0 ; i < MAX_CLIENT_CONNECTION ; ++i , ++pConn)
	{
		switch(pConn->sockStatus)
		{
		case SOCK_STATUS_UNCONNECTED:
			difTime = tick - pConn->lastCloseTime;
			if(difTime > m_clientCfg.reconnectDelay)
			{
				if(m_shutDownNetwork == 0)
				{
					ConnectClientToServer(pConn);
				}
			}
			break;
		case SOCK_STATUS_CONNECTING:
			difTime = tick - pConn->connectingTime;
			if(difTime > m_clientCfg.connectTimeout)
			{
				m_hook->OnClientConnectToServerTimeout(m_userData , pConn->cid , pConn->connUserID);
				CloseClient(pConn , tick);
			}
			break;
		case SOCK_STATUS_CONNECTED:
			difTime = tick - pConn->lastRecvTime;
			if(difTime > m_clientCfg.recvTimeout)
			{
				m_hook->OnClientServerReplyMsgTimeout(m_userData , pConn->cid , pConn->connUserID);
				CloseClient(pConn , tick);
			}
			else if(difTime > m_clientCfg.heartBeatTime)
			{
				SendHeartBeat(pConn);
			}
			
			break;
		}
	}

	pConn = m_serverConnTab;
	for(i = 0 ; i < MAX_SERVER_CONNECTION ; ++i , ++pConn)
	{
		if(pConn->sockStatus == SOCK_STATUS_CONNECTED)
		{
			difTime = tick - pConn->lastRecvTime;
			if(difTime > m_serverCfg.recvTimeout)
			{
				m_hook->OnServerClientTimeout(m_userData , pConn->stationID);
				CloseServerConn(pConn);
			}
		}
		else if(pConn->sockStatus == SOCK_STATUS_CLOSED)
		{
			difTime = tick - pConn->lastCloseTime;
			if(difTime > 1000)
			{
				ServerQueueOneAccept(pConn);
			}
		}
	}

}
