//Copyright(C) Ideal Dragon. All rights reserved.
//Use of this source code is governed by GPL-style license
//Author: Ideal Dragon

#include "flyd_func.h"
#include "flyd_config.h"
#include "logging/Logging.h"
#include "signal/flyd_signal.h"
#include "flyd_singleton.h"
#include "proc/flyd_process.h"
#include "net/flyd_socket.h"
#include "logging/ThreadPool.h"
#include "../logic/flyd_logic.h"
#include "../misc/flyd_memory.h"
#include "../misc/flyd_crc32.h"

using namespace flyd;
using namespace muduo;
//进程相关变量
pid_t flyd_pid, flyd_parent;
Flyd_Process flyd_process;
int          g_stopEvent;


//日志相关变量
FILE* g_filep;

//socket相关
//CSocekt g_socket;               //socket全局对象
CLogicSocket g_socket;
ThreadPool g_threadpool;


void dummyOutput(const char* msg, int len)
{
    if (g_filep)
    {
        fwrite(msg, 1, len, g_filep);
        dummyFlush();
    }
}
void dummyFlush()
{
    fflush(g_filep);
}



int main(int argc, char **argv)
{
    //0表示不退出
    g_stopEvent = 0;

    g_filep = ::fopen("../flyd_log", "ae");
    if(!g_filep)
        printf("打开失败\n");
    muduo::Logger::setOutput(dummyOutput);
    muduo::Logger::setFlush(dummyFlush);
    muduo::Logger::setLogLevel(muduo::Logger::DEBUG);

    //初始化信号
    Singleton<Signal>::getInstance().Init();

     //读取配置文件
    if(!Singleton<Config>::getInstance().Load("../flyd.conf"))
    {
        LOG_FATAL<<"配置文件读取失败，退出！\n";
    }

    //内存单例类
    CMemory::GetInstance();

    //CRC校验算法单例类
    CCRC32::GetInstance();

    //得到监听端口号
    int port = Singleton<Config>::getInstance().GetIntDefault("ListenPort", 0);
    LOG_INFO << "port =" << port;
    //得到监听的地址
    const char *pDBInfo = Singleton<Config>::getInstance().GetString("DBInfo");
    if(pDBInfo != NULL)
    {
        LOG_INFO << "DBInfo = " << pDBInfo;
    }
    else
    {
        LOG_FATAL<<("DBInfo get failed\n");
    }

    flyd_pid = getpid();      //取得进程pid
    flyd_parent = getppid();     //取得父进程的id
    LOG_INFO << "主进程pid = "<< flyd_pid << "ppid = " << flyd_parent;

    if(!g_socket.Initialize())//初始化socket
    {
        LOG_FATAL << "socket initialize failed ";
    }


    int cdaemonresult = flyd_daemon();
    if(cdaemonresult == -1) //fork()失败
        LOG_ERROR << "cdaemonresult:master进程创建失败\n";
    if(cdaemonresult == 1)
    {
        LOG_WARN << "这是原始的父进程，退出\n";
      //  while(1);
        exit(0);
    }

    LOG_INFO << "master exit successfully \n " ;

    flyd_master_process_cycle();

    LOG_INFO << "exit successfully \n " ;

    return 0;
}
