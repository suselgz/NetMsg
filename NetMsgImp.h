#pragma once

#include "NetMsg.h"


#define MAX_SERVER_CONNECTION 20
#define MAX_CLIENT_CONNECTION 20

enum SOCK_TYPE_CONST
{
	SOCK_TYPE_CLIENT_CONN,
	SOCK_TYPE_SERVER_LISTEN,
	SOCK_TYPE_SERVER_CONN,
};

enum SOCK_STATUS_CONST
{
	SOCK_STATUS_EMPTY,
	SOCK_STATUS_UNCONNECTED,
	SOCK_STATUS_CONNECTING,
	SOCK_STATUS_CONNECTED,
	SOCK_STATUS_CLOSED,
};

enum OPERATION_TYPE_CONST
{
	OPERATION_TYPE_CONNECT,
	OPERATION_TYPE_SEND,
	OPERATION_TYPE_RECV,
	OPERATION_TYPE_RECV_DATA,
	OPERATION_TYPE_SEND_HEARTBEAT,
};

enum CUSTOM_QUEUE_PACKET_TYPE_CONST
{
	CUSTOM_QUEUE_PACKET_TYPE_SHUTDOWN_WORKER_THREAD = 1,
};

struct NET_COMM_OVERLAPPED : OVERLAPPED
{
	int op;
	DWORD dwBytes , recvBytes , sendBytes;
	DWORD dwFlag;
};

struct NET_LISTEN_OVERLAPPED : OVERLAPPED
{
	DWORD dwBytes;
};

struct NET_CONNECTION_DATA
{
	int cid;
	int sockType;
	SOCKET sock;
	int sockStatus;
	int connUserID;
	sockaddr_in peerAddr;
	DWORD connectingTime , lastRecvTime , lastSendTime , lastCloseTime;
};

struct NET_SERVER_LISTEN_CONNECTION_DATA : NET_CONNECTION_DATA
{
	BYTE acceptBuf[2 * (sizeof(sockaddr_in) + 16)];
};

struct NET_COMM_CONNECTION_DATA : NET_CONNECTION_DATA
{
	int stationID;
	BYTE * sendBuf , * recieveBuf;
	int sendBufLen , recieveBufLen;
	BYTE * sendData , * recieveData;
	NET_MSG_PACKET_HEAD * sendPacketPtr;
	unsigned peerTimeDif;
	NET_COMM_OVERLAPPED sendOvlp , recvOvlp , heartBeatOvlp;
	NET_MSG_PACKET_HEAD sendPacket , recievePacket , heartBeatPacket;
};

class CNetMsgImp:public CNetMsg
{
public:
	HANDLE m_hNetworkThread;
	DWORD m_hNetworkThreadID;
	int m_shutDownNetwork , m_shutDownWorkThread;
	
	NET_MSG_CLIENT_CFG m_clientCfg;
	NET_MSG_SERVER_CFG m_serverCfg;
	NET_COMM_CONNECTION_DATA m_clientConnTab[MAX_CLIENT_CONNECTION];
	NET_SERVER_LISTEN_CONNECTION_DATA m_serverListen;
	NET_COMM_CONNECTION_DATA m_serverConnTab[MAX_SERVER_CONNECTION];
	HANDLE m_hIocp;
	void * m_userData;
	CNetMsgHook * m_hook;

	CNetMsgImp();
	~CNetMsgImp();

	static DWORD WINAPI NetworkThreadFun(void * param);
	DWORD _stdcall NetworkThread();

	

	virtual int  _stdcall Release();
	virtual int  _stdcall InitNetwork(NET_MSG_CLIENT_CFG * clientCfg , NET_MSG_SERVER_CFG * serverCfg , CNetMsgHook * hook , void * userData);
	virtual void _stdcall FreeNetwork();
	virtual int  _stdcall OpenConnection(LPCTSTR serverIP , int serverPort , int userID);
	virtual void _stdcall CloseConnection(int cid);
	virtual int  _stdcall SendMsgToServer(int cid , NET_MSG_PACKET_HEAD * msg  , BYTE * packetData , int flag);
	virtual int  _stdcall SendMsgToClient(int stationID , NET_MSG_PACKET_HEAD * msg , BYTE * packetData , int flag);
	virtual unsigned  _stdcall GetTime();

	void ConnectClientToServer(NET_COMM_CONNECTION_DATA * pConn);
	void CloseClient(NET_COMM_CONNECTION_DATA * pConn , DWORD closeTime);

	void ServerListen(NET_SERVER_LISTEN_CONNECTION_DATA * pConn);
	void ServerQueueAccept(NET_SERVER_LISTEN_CONNECTION_DATA * pConn);
	void ServerQueueOneAccept(NET_COMM_CONNECTION_DATA * p);
	void CloseServerConn(NET_COMM_CONNECTION_DATA * pConn);

	void PostConnRecvHead(NET_COMM_CONNECTION_DATA * pConn , int offset);
	void PostConnRecvData(NET_COMM_CONNECTION_DATA * pConn , int offset);
	void SendHeartBeat(NET_COMM_CONNECTION_DATA * pConn);
	void SendPacket(NET_COMM_CONNECTION_DATA * pConn);

	void OnClientRecvMsg(NET_COMM_CONNECTION_DATA * pConn);
	void OnServerRecvMsg(NET_COMM_CONNECTION_DATA * pConn);
	void ProcessCustomQueuePacket(NET_COMM_CONNECTION_DATA * pConn , NET_COMM_OVERLAPPED * po , int type);
	void ProcessClientConn(NET_COMM_CONNECTION_DATA * pConn , NET_COMM_OVERLAPPED * po , DWORD dwBytes);
	void ProcessServerListen(NET_SERVER_LISTEN_CONNECTION_DATA * pConn , NET_LISTEN_OVERLAPPED * po , DWORD dwBytes);
	void ProcessServerConn(NET_COMM_CONNECTION_DATA * pConn , NET_COMM_OVERLAPPED * op , DWORD dwBytes);

	void ProcessTimeout();
	void ProcessSocketError(NET_COMM_CONNECTION_DATA * pConn , NET_COMM_OVERLAPPED * po , DWORD dwBytes);

};

extern LPFN_CONNECTEX fnConnectEx;
extern LPFN_ACCEPTEX lpfnAcceptEx;

SOCKET OpenSocket();
int BindSocket(SOCKET sock , int port);
SOCKET AcceptSocket(SOCKET sockSvr , int timeout);
int RecieveData(SOCKET sock , char * buf , int len , int timeout);