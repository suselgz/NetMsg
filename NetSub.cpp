#include "stdafx.h"
#include "NetMsgImp.h"


void CNetMsgImp::PostConnRecvHead(NET_COMM_CONNECTION_DATA * pConn , int offset)
{
	WSABUF buf;
	buf.buf = (char *)&pConn->recievePacket + offset;
	buf.len = sizeof(pConn->recievePacket) - offset;
	NET_COMM_OVERLAPPED * po = &pConn->recvOvlp;
	po->op = OPERATION_TYPE_RECV;
	po->dwBytes = offset;
	po->dwFlag = 0;
	po->recvBytes = -1;
	WSARecv(pConn->sock , &buf , 1  , &po->recvBytes , &po->dwFlag , po , 0);
}

void CNetMsgImp::PostConnRecvData(NET_COMM_CONNECTION_DATA * pConn , int offset)
{
	WSABUF buf;
	buf.buf = (char *)pConn->recieveData + offset;
	buf.len = pConn->recievePacket.dataLen - offset;
	NET_COMM_OVERLAPPED * po = &pConn->recvOvlp;
	po->op = OPERATION_TYPE_RECV_DATA;
	po->dwBytes = offset;
	po->dwFlag = 0;
	po->recvBytes = -1;
	WSARecv(pConn->sock , &buf , 1  , &po->recvBytes , &po->dwFlag , po , 0);
}

void CNetMsgImp::SendHeartBeat(NET_COMM_CONNECTION_DATA * pConn)
{
	WSABUF buf;
	buf.buf = (char *)&pConn->heartBeatPacket;
	buf.len = sizeof(pConn->heartBeatPacket);
	pConn->heartBeatPacket.dataLen = 0;
	pConn->heartBeatPacket.type = NET_MSG_PACKET_TYPE_HEART_BEAT;
	NET_COMM_OVERLAPPED * po = &pConn->heartBeatOvlp;
	po->op = OPERATION_TYPE_SEND_HEARTBEAT;
	pConn->heartBeatPacket.timestamp = GetTime() + pConn->peerTimeDif;
	WSASend(pConn->sock , &buf , 1 , &po->sendBytes , 0 , po , 0);
}

void CNetMsgImp::SendPacket(NET_COMM_CONNECTION_DATA * pConn)
{
	WSABUF buf[2];
	buf[0].buf = (char *)pConn->sendPacketPtr;
	buf[0].len = sizeof(pConn->sendPacket);
	buf[1].buf = (char *)pConn->sendData;
	buf[1].len = pConn->sendPacketPtr->dataLen;
	pConn->sendPacketPtr->timestamp = GetTime() + pConn->peerTimeDif;
	NET_COMM_OVERLAPPED * po = &pConn->heartBeatOvlp;
	po->op = OPERATION_TYPE_SEND;
	WSASend(pConn->sock , buf , buf[1].len == 0 ? 1 : 2 , &po->sendBytes , 0 , po , 0);
}

