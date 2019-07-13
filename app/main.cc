//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#include <iostream>
#include "flyd_func.h"
#include "flyd_config.h"
#include "logging/Logging.h"
#include "signal/flyd_signal.h"
#include "flyd_singleton.h"



using namespace flyd;
    

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



    std::cout << "exit successfully \n" << std::endl;


    return 0;
}
