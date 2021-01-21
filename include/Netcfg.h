#pragma  once

#define IMAGE_STATION_K_COUNTS 40
#define SHEET_K_X_COUNTS 7

//网络传送错误图像大小及数目
#define ERROR_IMAGE_WIDTH 180
#define ERROR_IMAGE_HEIGHT 120
#define ERROR_IMAGE_SIZE (ERROR_IMAGE_WIDTH * ERROR_IMAGE_HEIGHT * 3)
#define ERROR_IMAGE_MONO_SIZE (ERROR_IMAGE_WIDTH * ERROR_IMAGE_HEIGHT)
#define MAX_ERROR_IMAGE_COUNT 50

//缺陷信息网络传送缓存大小
#define NET_SEND_BUFFER_SIZE 20

//背面机缓存大小，采集完第三张，处理之前发送第一张的结果
#define MACHINE_SEND_BUFFER_SIZE 2

//连续出错张数，报连续废
#define NET_CONTINUE_ERROR_SHEET_COUNT 2
#define NET_CONTINUE_ERROR_K_COUNT 2

enum STATION_TYPE_CONST
{
	STATION_TYPE_SLAVE_FIRST,
	STATION_TYPE_SLAVE_SECOND,
	STATION_TYPE_SLAVE_THIRD,
	STATION_TYPE_COUNT,
};

enum NET_PACKET_MSG_TYPE_CONST
{
	NET_PACKET_MSG_TYPE_CHECK_RESULT = 1,
	NET_PACKET_MSG_TYPE_CHECK_RESULT_REPLY,
	NET_PACKET_MSG_TYPE_CHECK_RESULT_DETAIL, //发送详细检测信息
	NET_PACKET_MSG_TYPE_ERROR_IMAGE,         //发送误差图像
	NET_PACKET_MSG_TYPE_START_DET,           //开始检测
	NET_PACKET_MSG_TYPE_STOP_DET,            //停止检测
	NET_PACKET_MSG_TYPE_START_SEND,          //开始发包
	NET_PACKET_MSG_TYPE_STOP_SEND,           //停止发包
	NET_PACKET_MSG_TYPE_BUILD_LAB_MODEL,     //重建LAB模板
	NET_PACKET_MSG_TYPE_RELEARN_LAB_MODEL,   //再学LAB模板
	NET_PACKET_MSG_TYPE_SHUTDOWN,            //关机
	NET_PACKET_MSG_TYPE_SAVE_TEMPLATE,       //保存模板图像
	NET_PACKET_MSG_TYPE_SHEET_START_INFO,    //主站发送起始大张信息
	NET_PACKET_MSG_TYPE_SHEET_NUMBER_RECOG_RESULT, //号码识别结果
	NET_PACKET_MSG_TYPE_TEMPLATE_STAT,       //当前模板状态
	NET_PACKET_MSG_TYPE_COUNT,
};

struct NET_PACKET_CHECK_RESULT
{
	int checkResult;
	int errorCount;
};

struct NET_PACKET_ERROR_IMAGE_INFO
{
	int stationID;                 //从站号
	int xInWholeSheet;             //误差图像在大张中的列 0-4
	int yInWholeSheet;             //误差图像在该列的开位 0-7
	int sheetNumber;               //大张号
	int serialNumber;              //大张序列号
	int type;                      //0:背面; 1:正面 
	int errType;                   //错误类型
	int errEnergy;                 //残点能量
	BYTE r[ERROR_IMAGE_MONO_SIZE]; //r通道
	BYTE g[ERROR_IMAGE_MONO_SIZE]; //g通道
	BYTE b[ERROR_IMAGE_MONO_SIZE]; //b通道
};

//大张缺陷信息,每大张一个包
struct NET_PACKET_ERROR_DETAIL_INFO
{
	int stationID;     //图像采集从站号
	int sheetNumber;   //大张号
	int serialNumber;  //大张序列号
	int sheetInfo;     //0:大张无错; 10:无纸; 11:过版纸;
	int errType[IMAGE_STATION_K_COUNTS]; //1~9;每开的错误类型,只保存最严重错误
	int errEnergy[IMAGE_STATION_K_COUNTS];//误差能量
	int continueError[SHEET_K_X_COUNTS]; //1：连续废
};

enum NET_PACKET_SHEET_ERROR_TYPE_CONST 
{
	NET_PACKET_SHEET_ERROR_TYPE_PRESS = -1,      //离压
	NET_PACKET_SHEET_ERROR_TYPE_OK = 0,          //大张无错
	NET_PACKET_SHEET_ERROR_TYPE_WHITE,           //白点
	NET_PACKET_SHEET_ERROR_TYPE_ERROR,           //深浅
	NET_PACKET_SHEET_ERROR_TYPE_PARTIAL,         //折角
	NET_PACKET_SHEET_ERROR_TYPE_DIRTY,           //黑点
	NET_PACKET_SHEET_ERROR_TYPE_SIDE_GUIDE,      //侧规
	NET_PACKET_SHEET_ERROR_TYPE_FRONT_GUIDE,     //前规
	NET_PACKET_SHEET_ERROR_TYPE_MAYBE,           //疑似
	NET_PACKET_SHEET_ERROR_TYPE_THREAD,          //安全线
	NET_PACKET_SHEET_ERROR_TYPE_ANTICOPY_CIRCLE, //防复印圈
	NET_PACKET_SHEET_ERROR_TYPE_EMPTY = 10,      //大张空张
	NET_PACKET_SHEET_ERROR_TYPE_IMP = 11,        //过版纸	
	NET_PACKET_SHEET_ERROR_TYPE_PRESS_NOT_DET = 14,   //离压未检
	NET_PACKET_SHEET_ERROR_TYPE_NOT_DET = 15,    //超时未检
	NET_PACKET_SHEET_ERROR_TYPE_TEMPLATE = 16,   //学习模板
	NET_PACKET_SHEET_ERROR_TYPE_COUNT,
};

struct NET_PACKET_SHEET_START_INFO
{
	int CheHao;              //车号
	int startSheetNumber;    //8位起始大张号
	int backNowNumber;       //4位大张号
};

struct NET_PACKET_SHEET_NUMBER_RECOG_RESULT
{
	int errInfo;           //错误标识
	int chehao;            //车号
	int serialNumber;      //序列号(车号+序列号)
	int sheetNumber;       //喷标号(车号+大张号)
	int codeDetCount;
	int codeGrabCount;
};

struct NET_SEND_BUFFER
{
	NET_PACKET_ERROR_IMAGE_INFO errImage[IMAGE_STATION_K_COUNTS];
	NET_PACKET_ERROR_DETAIL_INFO errDetail;
	int crescentErrorType;
};

enum DET_MACHINE_TYPE_CONST
{
	DET_MACHINE_TYPE_H_FRONT = 0,   //一印
	DET_MACHINE_TYPE_H_BACK,		//二印
	DET_MACHINE_TYPE_COUNT,
};