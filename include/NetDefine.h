#pragma once
#include "NetMsg.h"

// Network方向
#define NET_MSG_DIR_CS_TO_CLIENT					0		
#define NET_MSG_DIR_CLIENT_TO_CS					1
#define NET_MSG_DIR_VS_TO_CLIENT					2
#define NET_MSG_DIR_CLIENT_TO_VS					3
#define NET_MSG_DIR_CS_TO_VS						4
#define NET_MSG_DIR_VS_TO_CS						5
#define NET_MSG_DIR_CLIENT_TO_CLIENT				6

// COMMAND
#define NET_CMD_NOTIFY_WHITEBALANCE_START			1			// 开始白平衡
#define NET_CMD_NOTIFY_WHITEBALANCE_STOP			2			// 停止白平衡
#define NET_CMD_NOTIFY_GRAB_START					3			// 开始采集
#define NET_CMD_NOTIFY_GRAB_STOP					4			// 停止采集
#define NET_CMD_NOTIFY_WHITEBALANCE_CC2				5			// 学习白平衡CC 2命令
#define NET_CMD_NOTIFY_WHITEBALANCE_GETIMAGE		6			// 请求从站发送一帧白平衡图像
#define NET_CMD_NOTIFY_GETONEIMAGE					7			// 请求从站发送一帧实时全幅面图像
#define NET_CMD_NOTIFY_ENABLEAUTOSAVE				8			// 请求从站开始保存图像
#define NET_CMD_NOTIFY_DISABLEAUTOSAVE				9			// 请求从站停止保存图像
#define NET_CMD_NOTIFY_STARTRECREATETEMPLATE		10			// 开始重建模板	
#define NET_CMD_NOTIFY_STOPRECREATETEMPLATE			11			// 停止重建模板
#define NET_CMD_NOTIFY_STARTFREERUN					12			// 开始自由运行模式
#define NET_CMD_NOTIFY_STOPFREERUN					13			// 停止自由运行模式
#define NET_CMD_NOTIFY_APPLICATIONEXIT				14			// 程序退出
#define NET_CMD_NOTIFY_UNIQUESN						38			// 唯一序列号
#define NET_CMD_NOTIFY_HAOMA_BASIC					39			// 冠字信息
#define NET_CMD_NOTIFY_HAOMA_CFG_CHANGED			40			// 发送号码参数到从站
#define NET_CMD_NOTIFY_MACHINE_SIGNAL				41			// 发送合压进位信号到从站
#define NET_CMD_NOTIFY_YG_CFG_CHANGED				42			// 荧光参数
#define NET_CMD_NOTIFY_SAVE_CLEAR_IMG				50	//保存清晰图像
// 从站发送到显示站
#define NETPACKET_TYPE_TOVS_ERROR_INFO				0x10		// 检测站传送的错误记录，包含小块图像
#define NETPACKET_TYPE_TOVS_RUNTIME_IMAGE			0x11		// 检测站传送的实时图像，文件大小在包头内定义
#define NETPACKET_TYPE_FROMVS_AQUIREIMAGE			0x12		// 请求从站传图像，错误与实时图像都用这个命令
#define NETPACKET_TYPE_FROMVS_STOPSNATCHREALIMAGE	0x13		// 停止从站传送图像，错误与实时图像都用这个命令
#define NETPACKET_TYPE_TOVS_REALERRORIMAGE			0x14		// 从站发送实时错误图像
#define NETPACKET_TYPE_FROMVS_AQUIREERRORIMAGE		0x15		// 请求从站发送错误图像
#define NETPACKET_TYPE_TOVS_RMBCODE_IMG				0x16		// 检测站传送的图像
#define NETPACKET_TYPE_VS_AQUIRECURRENTERRORIMAGE	0x1A		// 请求指定工作站发送最新一张错误图像
//zy add
#define NETPACKET_TYPE_TOVS_STAT_INFO				0x1B		//大张统计信息

//end

// 从站发送到控制站
#define NETPACKET_TYPE_TOCS_NOTE_ERRORRECORD		0x30		// 检测站传送到控制主战的小开错误记录
#define NETPACKET_TYPE_TOCS_SHEET_ERRORRECORD		0x31		// 检测站传送到控制主战的大张单路错误记录
#define NETPACKET_TYPE_TOCS_NOTE_NOCHECKED			0x32		// 小开未检
#define NETPACKET_TYPE_TOCS_SHEET_NOCHECKED			0x33		// 大张未检
#define NETPACKET_TYPE_TOCS_REMIND_WHITEBALANCE		0x34		// 通知控制主站应该做白平衡


struct NET_MSG_CHEHAO_INFO
{
	char strClass[20];
	char strCaptaion[20];
	char strCheHao[20];
	char strIntaglio[20];
	char strOffset[20];
	char strIntaglioAux[20];
	char strOffsetAux[20];
};

struct NET_MSG_HOST_TO_CLIENT
{
	DWORD dwPackageSize;
	DWORD dwNotifyType;
	DWORD dwStationIDFrom;
	DWORD dwCommand;
};

struct NETWORK_CONFIG_SETS
{
	int nConnectTimeout;
	int nRecvTimeout;
	int nHeartBeatTime;
	int nReconnectDelayTime;
	int nHostProvideServerPort;
	int nLocalStationID;
	int nLocalProvideServerPort;
};

struct NET_IMAGE_PACKET_HEAD
{
	DWORD dwPacketSize;
	DWORD dwStationIDFrom;
	DWORD dwImageWidth;
	DWORD dwImageHeight;
	DWORD dwImagePitch;
};

struct NET_MSG_PACKET_AQUIREIMAGEDEFINE
{
	BOOL bIsRealImage, bIsTimeInterval;
	int nTimeInterval, nSheetInterval;
	int bErrorImage;
	int left, top, right, bottom;
	int wLeft, wTop, wRight, wBottom;
};
