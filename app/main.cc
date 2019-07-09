//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#include <iostream>
#include "flyd_func.h"
#include "config.h"



using namespace flyd;
    

int main(int argc, char **argv)
{

     //读取配置文件
    Config &conf =  Config::GetInstance();
    if(!conf.Load("../flyd.conf"))
    {
        printf("配置文件读取失败，退出！\n");
        exit(1);
    }
    //得到监听端口号
    int port = conf.GetIntDefault("ListenPort", 0);
    printf("port = %d\n", port);
    //得到监听的地址
    const char *pDBInfo = conf.GetString("DBInfo");
    if(pDBInfo != NULL)
    {
        printf("DBInfo = %s\n", pDBInfo);
    }



    std::cout << "exit successfully \n" << std::endl;


    return 0;
}
