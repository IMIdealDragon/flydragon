//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#include <iostream>
#include "flyd_func.h"
#include "flyd_config.h"
#include "logging/Logging.h"
#include "signal/flyd_signal.h"
#include "flyd_singleton.h"
#include "proc/flyd_process.h"

using namespace flyd;

pid_t flyd_pid, flyd_parent;
    

int main(int argc, char **argv)
{

    //初始化信号
    Singleton<Signal>::getInstance().Init();

     //读取配置文件
    if(!Singleton<Config>::getInstance().Load("../flyd.conf"))
    {
        printf("配置文件读取失败，退出！\n");
        exit(1);
    }

    //得到监听端口号
    int port = Singleton<Config>::getInstance().GetIntDefault("ListenPort", 0);
    printf("port = %d\n", port);
    //得到监听的地址
    const char *pDBInfo = Singleton<Config>::getInstance().GetString("DBInfo");
    if(pDBInfo != NULL)
    {
        printf("DBInfo = %s\n", pDBInfo);
        LOG_INFO << "DBInfo = " << pDBInfo ;
    }
    else
    {
        printf("DBInfo get failed\n");
    }

    flyd_pid = getpid();      //取得进程pid
    flyd_parent = getppid();     //取得父进程的id
    printf("主进程pid = %d, ppid = %d\n", flyd_pid, flyd_parent);

    int cdaemonresult = flyd_daemon();
    if(cdaemonresult == -1) //fork()失败
        printf("cdaemonresult:master进程创建失败\n");
    if(cdaemonresult == 1)
    {
        printf("这是原始的父进程，退出\n");
        exit(0);
    }

    flyd_master_process_cycle();


    std::cout << "exit successfully \n" << std::endl;


    return 0;
}
