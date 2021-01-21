#include "stdafx.h"
#include "NetMsgImp.h"



void CNetMsgImp::ProcessServerListen(NET_SERVER_LISTEN_CONNECTION_DATA * pConn , NET_LISTEN_OVERLAPPED * po , DWORD dwBytes)
{
	if(m_shutDownNetwork == 0)
	{
		NET_COMM_CONNECTION_DATA * p = (NET_COMM_CONNECTION_DATA *)((BYTE *)po - offsetof(NET_COMM_CONNECTION_DATA , sendOvlp));
		CreateIoCompletionPort((HANDLE)p->sock , m_hIocp , (ULONG_PTR)p , 0);
		PostConnRecvHead(p , 0);
		p->sockStatus = SOCK_STATUS_CONNECTED;
		m_hook->OnServerClientConnected(m_userData);
		DWORD tick = GetTime();
		p->lastRecvTime = tick;
		p->lastSendTime = tick;
	}
}

void CNetMsgImp::OnServerRecvMsg(NET_COMM_CONNECTION_DATA * pConn)
{
	if(pConn->stationID < 0)
	{
		pConn->stationID = pConn->recievePacket.stationID;
		m_hook->OnServerStationConnected(m_userData , pConn->stationID);
	}
	if(pConn->recievePacket.type == NET_MSG_PACKET_TYPE_HEART_BEAT)
	{
		pConn->heartBeatPacket = pConn->recievePacket;
		pConn->heartBeatPacket.originalTimestamp = pConn->heartBeatPacket.timestamp; 
		pConn->heartBeatPacket.stationID = m_serverCfg.stationID;
		pConn->peerTimeDif = pConn->heartBeatPacket.timestamp - GetTime();
		SendHeartBeat(pConn);
	}
	else
	{
		m_hook->OnServerRecvStationMsgData(m_userData , pConn->stationID , &pConn->recievePacket , pConn->recieveData);
	}
}

void CNetMsgImp::ProcessServerConn(NET_COMM_CONNECTION_DATA * pConn , NET_COMM_OVERLAPPED * po , DWORD dwBytes)
{
	if (m_shutDownNetwork)
	{
		return;
	}
	DWORD tick = GetTime();
	switch(po->op)
	{
	case OPERATION_TYPE_SEND:
		pConn->lastSendTime = tick;
		break;
	case OPERATION_TYPE_SEND_HEARTBEAT:
		pConn->lastSendTime = tick;
		break;

	case OPERATION_TYPE_RECV:
		if(dwBytes == 0)
		{
			CloseServerConn(pConn);
		}
		else
		{
			po->dwBytes += dwBytes;
			if(po->dwBytes >= sizeof(pConn->recievePacket))
			{
				pConn->lastRecvTime = tick;
				
				if(pConn->recievePacket.dataLen == 0)
				{
					OnServerRecvMsg(pConn);
					PostConnRecvHead(pConn , 0);
				}
				else
				{
					BYTE * recvBuf = (BYTE *)m_hook->OnGetServerRecieveStationMsgDataBuf(m_userData , pConn->stationID , &pConn->recievePacket);
					if(recvBuf == 0)
					{
						if(pConn->recievePacket.dataLen > pConn->recieveBufLen)
						{
							pConn->recieveBuf = (BYTE *)realloc(pConn->recieveBuf , pConn->recievePacket.dataLen);
						}
						pConn->recieveData = pConn->recieveBuf;
					}
					else
					{
						pConn->recieveData = recvBuf;
					}
					PostConnRecvData(pConn , 0);
				}
			}
			else
			{
				PostConnRecvHead(pConn , po->dwBytes);
			}
		}
		break;
	case OPERATION_TYPE_RECV_DATA:
		if(dwBytes == 0)
		{
			CloseServerConn(pConn);
		}
		else
		{
			po->dwBytes += dwBytes;
			if((int)po->dwBytes >= pConn->recievePacket.dataLen)
			{
				pConn->lastRecvTime = tick;
				OnServerRecvMsg(pConn);
				PostConnRecvHead(pConn , 0);
			}
			else
			{
				PostConnRecvData(pConn , po->dwBytes);
			}
		}
		break;
	}
}

