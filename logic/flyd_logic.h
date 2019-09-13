#ifndef __NGX_C_SLOGIC_H__
#define __NGX_C_SLOGIC_H__

#include <sys/socket.h>
#include "../net/flyd_socket.h"



//处理逻辑和通讯的子类
class CLogicSocket : public CSocekt   //继承自父类CScoekt
{
public:
	CLogicSocket();                                                         //构造函数
	virtual ~CLogicSocket();                                                //释放函数
	virtual bool Initialize();                                              //初始化函数

public:
	//各种业务逻辑相关函数都在这里
	bool _HandleRegister(lp_connection_t pConn,LPSTRUC_MSG_HEADER pMsgHeader,char *pPkgBody,unsigned short iBodyLength);
	bool _HandleLogIn(lp_connection_t pConn,LPSTRUC_MSG_HEADER pMsgHeader,char *pPkgBody,unsigned short iBodyLength);
	bool _HandlePing(lp_connection_t pConn,LPSTRUC_MSG_HEADER pMsgHeader,char *pPkgBody,unsigned short iBodyLength);

	//发送只有包头的数据包
	void SendNoBodyPkgToClient(LPSTRUC_MSG_HEADER pMsgHeader, unsigned short iMsgCode);
    virtual void procPingTimeOutChecking(LPSTRUC_MSG_HEADER tmpmsg,time_t cur_time);

public:
	virtual void threadRecvProcFunc();
};

#endif