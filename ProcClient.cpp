#include "stdafx.h"
#include "NetMsgImp.h"


void CNetMsgImp::OnClientRecvMsg(NET_COMM_CONNECTION_DATA * pConn)
{
	if(pConn->recievePacket.type != NET_MSG_PACKET_TYPE_HEART_BEAT)
	{
		m_hook->OnClientRecieveServerMsgData(m_userData , pConn->cid , pConn->connUserID , &pConn->recievePacket , pConn->recieveData);
	}
}

void CNetMsgImp::ProcessCustomQueuePacket(NET_COMM_CONNECTION_DATA * pConn , NET_COMM_OVERLAPPED * po , int type)
{
	switch(type)
	{
	case CUSTOM_QUEUE_PACKET_TYPE_SHUTDOWN_WORKER_THREAD:
		m_shutDownWorkThread = 1;
		break;
	}
}


void CNetMsgImp::ProcessClientConn(NET_COMM_CONNECTION_DATA * pConn , NET_COMM_OVERLAPPED * po , DWORD dwBytes)
{
	DWORD tick = GetTime();
	switch(po->op)
	{
	case OPERATION_TYPE_CONNECT:
		pConn->lastRecvTime = tick;
		pConn->lastSendTime = tick;
		pConn->sockStatus = SOCK_STATUS_CONNECTED;
		PostConnRecvHead(pConn , 0);
		SendHeartBeat(pConn);
		m_hook->OnClientConnectedToServer(m_userData , pConn->cid , pConn->connUserID);
		break;

	case OPERATION_TYPE_SEND:
	case OPERATION_TYPE_SEND_HEARTBEAT:
		pConn->lastSendTime = tick;
		break;

	case OPERATION_TYPE_RECV:
		if(dwBytes == 0)
		{
			CloseClient(pConn , tick);
		}
		else
		{
			po->dwBytes += dwBytes;
			if(po->dwBytes >= sizeof(pConn->recievePacket))
			{
				pConn->lastRecvTime = tick;
				if(pConn->recievePacket.dataLen == 0)
				{
					OnClientRecvMsg(pConn);
					PostConnRecvHead(pConn , 0);
				}
				else
				{
					BYTE * recvBuf = (BYTE *)m_hook->OnGetClientRecieveServerMsgDataBuf(m_userData , pConn->cid , pConn->connUserID , &pConn->recievePacket);
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
			CloseClient(pConn , tick);
		}
		else
		{
			po->dwBytes += dwBytes;
			if((int)po->dwBytes >= pConn->recievePacket.dataLen)
			{
				pConn->lastRecvTime = tick;
				OnClientRecvMsg(pConn);
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