#include "stdafx.h"
#include "NetMsgImp.h"


int  CNetMsgImp::InitNetwork(NET_MSG_CLIENT_CFG * clientCfg , NET_MSG_SERVER_CFG * serverCfg , CNetMsgHook * hook , void * userData)
{
	m_hook = hook;
	m_userData = userData;
	m_shutDownNetwork = 0;
	if(clientCfg)
	{
		m_clientCfg = *clientCfg;
		if(m_clientCfg.recvTimeout < m_clientCfg.heartBeatTime * 2)m_clientCfg.recvTimeout = m_clientCfg.heartBeatTime * 2;
	}

	if(serverCfg)
	{
		m_serverCfg = *serverCfg;
		ServerListen(&m_serverListen);
		ServerQueueAccept(&m_serverListen);
	}
	
	return 0;
}

static void FreeConns(NET_COMM_CONNECTION_DATA * p , int count)
{
	for(int i = 0 ; i < count ; ++i , ++p)
	{
		if(p->sock)
		{
			closesocket(p->sock);
			p->sock = 0;
			if(p->sendBuf)
			{
				free(p->sendBuf);
				p->sendBuf = 0;
				p->sendBufLen = 0;
			}
			if(p->recieveBuf)
			{
				free(p->recieveBuf);
				p->recieveBuf = 0;
				p->recieveBufLen = 0;
			}
		}
	}
}

void CNetMsgImp::FreeNetwork()
{
	m_shutDownNetwork = 1;
	if(m_serverListen.sock)
	{
		closesocket(m_serverListen.sock);
		m_serverListen.sock = 0;
	}
	FreeConns(m_serverConnTab , MAX_SERVER_CONNECTION);
	FreeConns(m_clientConnTab , MAX_CLIENT_CONNECTION);
}