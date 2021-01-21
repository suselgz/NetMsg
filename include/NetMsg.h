#pragma  once
#include "Windows.h"

#define NET_MSG_MAIN_VERSION 2
#define NET_MSG_SUB_VERSION 1
#define NET_MSG_VERSION ((NET_MSG_MAIN_VERSION << 16) + NET_MSG_SUB_VERSION)

enum NET_MSG_PACKET_TYPE_CONST
{
	NET_MSG_PACKET_TYPE_HEART_BEAT,
};

struct NET_MSG_PACKET_HEAD
{
	int stationID;
	int type;
	int frameIndex;
	int dataLen;
	unsigned timestamp;
	unsigned originalTimestamp;
	unsigned packet_comtype;	//是主控与检测站、主控与显示、显示与检测站 0 1 2
	unsigned packet_type;		//Packet包类型，类型不同后面PacketReserved起始地址后的数据结构不同
	int imgWidth;
	int imgHeight;
	int imgPitch;
	int imgSize;
	int checkResult;
	union
	{
		BYTE reserved[512 - 9 * sizeof(int) - 4 * sizeof(unsigned)];
	}PacketReserved;
};

struct NET_MSG_CLIENT_CFG
{
	int stationID;
	int connectTimeout , recvTimeout;
	int heartBeatTime , reconnectDelay;
};

struct NET_MSG_SERVER_CFG
{
	int stationID;
	int bindPort;
	int recvTimeout;
};

enum NET_MSG_SEND_MSG_FLAG_CONST
{
	NET_MSG_SEND_MSG_FLAG_NO_DATA_COPY = 0x01,
};

class CNetMsgHook
{
public:
	virtual void _stdcall OnClientConnectingToServer(void * userData , int cid , int userID) = 0;
	virtual void _stdcall OnClientConnectedToServer(void * userData , int cid , int userID) = 0;
	virtual void _stdcall OnClientConnectToServerTimeout(void * userData , int cid , int userID) = 0;
	virtual void _stdcall OnClientDisconnectedFromServer(void * userData , int cid , int userID) = 0;

	virtual void * _stdcall OnGetClientRecieveServerMsgDataBuf(void * userData , int cid , int userID , NET_MSG_PACKET_HEAD * msg ) = 0;
	virtual void _stdcall OnClientRecieveServerMsgData(void * userData , int cid , int userID , NET_MSG_PACKET_HEAD * msg  , BYTE * packetData) = 0;
	virtual void _stdcall OnClientServerReplyMsgTimeout(void * userData , int cid , int userID) = 0;

	virtual void _stdcall OnServerClientConnected(void * userData) = 0;
	virtual void _stdcall OnServerStationConnected(void * userData , int stationID) = 0;
	virtual void _stdcall OnServerStationDisconnected(void * userData , int stationID) = 0;
	virtual void * _stdcall OnGetServerRecieveStationMsgDataBuf(void * userData , int stationID , NET_MSG_PACKET_HEAD * msg ) = 0;
	virtual void _stdcall OnServerRecvStationMsgData(void * userData , int stationID , NET_MSG_PACKET_HEAD * msg  , BYTE * packetData) = 0;
	virtual void _stdcall OnServerClientTimeout(void * userData , int stationID) = 0;
};

class CNetMsg
{
public:
	virtual int  _stdcall Release() = 0;
	virtual int  _stdcall InitNetwork(NET_MSG_CLIENT_CFG * clientCfg , NET_MSG_SERVER_CFG * serverCfg , CNetMsgHook * hook , void * userData) = 0;
	virtual void _stdcall FreeNetwork() = 0;
	virtual int  _stdcall OpenConnection(LPCTSTR serverIP , int serverPort , int userID) = 0;
	virtual void _stdcall CloseConnection(int cid) = 0;
	virtual int  _stdcall SendMsgToServer(int cid , NET_MSG_PACKET_HEAD * msg  , BYTE * packetData , int flag) = 0;
	virtual int  _stdcall SendMsgToClient(int stationID , NET_MSG_PACKET_HEAD * msg , BYTE * packetData , int flag) = 0;
	virtual unsigned  _stdcall GetTime() = 0;

};

CNetMsg * _stdcall CreateCNetMsgInstance(int version = NET_MSG_VERSION);