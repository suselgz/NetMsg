
#include "stdafx.h"
#include "NetMsgImp.h"
static LARGE_INTEGER first;
static double dfreq;

CNetMsgImp::CNetMsgImp()
{
	memset(&m_hNetworkThread , 0 , offsetof(CNetMsgImp , m_hook) - offsetof(CNetMsgImp , m_hNetworkThread) + sizeof(m_hook));
	WSADATA wsaData;
	WSAStartup(  MAKEWORD( 2, 2 ), &wsaData );

	m_hIocp = CreateIoCompletionPort (INVALID_HANDLE_VALUE , 0 , 0 , 0);
	m_hNetworkThread = CreateThread(0 , 0 , NetworkThreadFun  , this , 0 , &m_hNetworkThreadID);
	SetThreadPriority(m_hNetworkThread , THREAD_PRIORITY_TIME_CRITICAL);
}

CNetMsgImp::~CNetMsgImp()
{
	FreeNetwork();
	PostQueuedCompletionStatus(m_hIocp , -CUSTOM_QUEUE_PACKET_TYPE_SHUTDOWN_WORKER_THREAD , 0 , 0);
	WaitForSingleObject(m_hNetworkThread , 1000);
	CloseHandle(m_hNetworkThread);
	m_hNetworkThread = 0;
	CloseHandle(m_hIocp);
	m_hIocp = 0;
	WSACleanup();
}
double  _stdcall GetCurTime()
{
	LARGE_INTEGER cur;
	QueryPerformanceCounter(&cur);
	if (dfreq == 0)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		first = cur;
		dfreq = 1.0 / ((double)freq.QuadPart);
	}
	return ((double)((__int64)(cur.QuadPart - first.QuadPart))) * dfreq;

}
double  _stdcall GetCurTimeMS()
{
	return GetCurTime() * 1000;
}
unsigned CNetMsgImp::GetTime()
{
	return (unsigned)::GetCurTimeMS();
}

CNetMsg * _stdcall CreateCNetMsgInstance(int version)
{
	if(version == NET_MSG_VERSION)return new CNetMsgImp;
	else return 0;
}

int  CNetMsgImp::Release()
{
	delete this;
	return 0;
}