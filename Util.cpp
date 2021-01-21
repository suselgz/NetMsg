#include "stdafx.h"
#include "NetMsgImp.h"


static GUID connectex_guid = WSAID_CONNECTEX;
LPFN_CONNECTEX fnConnectEx;
static GUID GuidAcceptEx = WSAID_ACCEPTEX;
LPFN_ACCEPTEX lpfnAcceptEx;


SOCKET OpenSocket()
{
	SOCKET sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP , NULL , 0 , WSA_FLAG_OVERLAPPED);
	DWORD bytes = 0;
	if(fnConnectEx == NULL)
	{
		WSAIoctl(sock,SIO_GET_EXTENSION_FUNCTION_POINTER, &connectex_guid,	sizeof(connectex_guid), &fnConnectEx, sizeof(fnConnectEx), &bytes, NULL, NULL);
		WSAIoctl(sock,SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx,	sizeof(GuidAcceptEx), &lpfnAcceptEx, sizeof(lpfnAcceptEx), &bytes, NULL, NULL);
	}

	return sock;
}


int BindSocket(SOCKET sock , int port)
{
	sockaddr_in svr;
	svr.sin_family = AF_INET;
	svr.sin_addr.s_addr = INADDR_ANY;
	svr.sin_port = htons(port);
	bind(sock , (sockaddr *)&svr , sizeof(svr));
	return 0;
}

