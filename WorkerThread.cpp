#include "stdafx.h"
#include "NetMsgImp.h"


DWORD WINAPI CNetMsgImp::NetworkThreadFun(void * param)
{
	CNetMsgImp * pThis = (CNetMsgImp *)param;
	return pThis->NetworkThread();
}

DWORD CNetMsgImp::NetworkThread()
{
	while(m_shutDownWorkThread == 0)
	{
		ULONG_PTR compKey;
		LPOVERLAPPED pOverlap;
		DWORD dwBytes;
		BOOL retVal = GetQueuedCompletionStatus(m_hIocp , &dwBytes , &compKey , &pOverlap , 10);
		NET_COMM_CONNECTION_DATA * pConn = (NET_COMM_CONNECTION_DATA *)compKey;
		NET_COMM_OVERLAPPED * po = (NET_COMM_OVERLAPPED *)pOverlap;
		if(retVal)
		{
			int bytes = dwBytes;
			if(bytes < 0)
			{
				ProcessCustomQueuePacket(pConn , po , -bytes);
			}
			else 
			{
				
				switch(pConn->sockType)
				{
				case SOCK_TYPE_CLIENT_CONN:
					ProcessClientConn((NET_COMM_CONNECTION_DATA *)pConn , po , dwBytes);
					break;
				case SOCK_TYPE_SERVER_LISTEN:
					ProcessServerListen((NET_SERVER_LISTEN_CONNECTION_DATA *)pConn , (NET_LISTEN_OVERLAPPED *)po , dwBytes);
					break;
				case SOCK_TYPE_SERVER_CONN:
					ProcessServerConn(pConn , po , dwBytes);
					break;
				}
			}
		}
		else
		{
			if(po == 0)
			{
				ProcessTimeout();
			}
			else
			{
				ProcessSocketError(pConn , po , dwBytes);
			}
			
		}
	}
	return 0;
}

