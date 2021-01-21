#include "stdafx.h"
#include "NetMsg.h"

CNetMsg * netMsg ;
NET_MSG_PACKET_HEAD sendMsg;
#define  DebugMsg printf
static LARGE_INTEGER first;
static double dfreq;
double  _stdcall GetCurTimeMSEx()
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
	return ((double)((__int64)(cur.QuadPart - first.QuadPart))) * dfreq*1000;
}
class CNetMsgHookImp : public CNetMsgHook
{
public:
	virtual void _stdcall OnClientConnectingToServer(void * userData , int cid , int userID) 
	{
		DebugMsg(TEXT("Client %d Connecting To Server@%1.1fmS\n") , cid , GetCurTimeMSEx());
		
	}
	virtual void _stdcall OnClientConnectedToServer(void * userData , int cid , int userID)
	{
		DebugMsg(TEXT("Client Connected To Server@%1.1fmS\n") , GetCurTimeMSEx());
		
	}

	virtual void _stdcall OnClientConnectToServerTimeout(void * userData , int cid , int userID)
	{
		DebugMsg(TEXT("Client Connect To Server Timeout @%1.1fmS\n") , GetCurTimeMSEx());
		
	}

	virtual void _stdcall OnClientDisconnectedFromServer(void * userData , int cid , int userID)
	{
		DebugMsg(TEXT("Client %d DisConnected From Server @%1.1fmS\n") , cid , GetCurTimeMSEx());
		
	}

	virtual void * _stdcall OnGetClientRecieveServerMsgDataBuf(void * userData , int cid , int userID , NET_MSG_PACKET_HEAD * msg )
	{
		DebugMsg(TEXT("Client Server Reply Msg Head@%1.1fmS\n") , GetCurTimeMSEx());
		return 0;
	}


	virtual void _stdcall OnClientRecieveServerMsgData(void * userData , int cid , int userID , NET_MSG_PACKET_HEAD * msg , BYTE * recvData)
	{
		DebugMsg(TEXT("Client Server Reply Msg Data@%1.1fmS\n") , GetCurTimeMSEx());

	}

	virtual void _stdcall OnClientServerReplyMsgTimeout(void * userData , int cid , int userID)
	{
		DebugMsg(TEXT("Client Server Reply Msg Timeout @%1.1fmS\n") , GetCurTimeMSEx());
		
	}

	virtual void _stdcall OnClientSocketError(void * userData , int cid , int userID) 
	{
		DebugMsg(TEXT("Client Socket Error @%1.1fmS\n") , GetCurTimeMSEx());
		
	}


	virtual void _stdcall OnServerClientConnected(void * userData)
	{
		DebugMsg(TEXT("Server Client Connected @%1.1fmS\n") , GetCurTimeMSEx());
		
	}

	virtual void _stdcall OnServerStationConnected(void * userData , int stationID)
	{
		DebugMsg(TEXT("Server Station %d  Connected @%1.1fmS\n") , stationID , GetCurTimeMSEx());
		
	}

	virtual void _stdcall OnServerClientTimeout(void * userData , int stationID)
	{
		DebugMsg(TEXT("Server Station %d Timeout @%1.1fmS\n") , stationID , GetCurTimeMSEx());
		
	}

	virtual void _stdcall OnServerStationDisconnected(void * userData  , int stationID)
	{
		DebugMsg(TEXT("Server Station %d DisConnected @%1.1fmS\n") , stationID , GetCurTimeMSEx());
		
	}

	void * _stdcall OnGetServerRecieveStationMsgDataBuf(void * userData , int stationID , NET_MSG_PACKET_HEAD * msg ) 
	{
		DebugMsg(TEXT("Server Recv Station %d Msg Head@%1.1fmS\n") , stationID , GetCurTimeMSEx());
		return 0;
	}

	virtual void _stdcall OnServerRecvStationMsgData(void * userData , int stationID , NET_MSG_PACKET_HEAD * msg  , BYTE * packetData)
	{
		DebugMsg(TEXT("Server Recv Station %d Msg @%1.1fmS@\n") , stationID , GetCurTimeMSEx());


	}


}hook;

BYTE imageData[100];//4096 * 3200

int _tmain(int argc, _TCHAR* argv[])
{

	netMsg = CreateCNetMsgInstance();
	
	NET_MSG_SERVER_CFG serverCfg;
	serverCfg.stationID = 13;
	serverCfg.bindPort = 4939;
	serverCfg.recvTimeout = 2000;

	netMsg->InitNetwork(NULL , &serverCfg , &hook , netMsg);

	for (int i=0;i<50;i++)
	{
		imageData[2*i]='c';
		imageData[2*i+1]='a';
	}
	imageData[99]='\0';
	for(;;)
	{
		sendMsg.stationID = 11;
		sendMsg.type = 8;
		sendMsg.dataLen = 100 ;

		Sleep(100);
	}

	getchar();

	netMsg->FreeNetwork();
	Sleep(1000);
	netMsg->Release();

	return 0;
}

