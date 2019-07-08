//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#include <iostream>
#include <csapp.h>
#include "config.h"

using namespace flyd;
int main()
{

    Config &conf =  Config::GetInstance();
    if(conf.Load("../flyd.conf") == false)
//    if(conf.Load("/home/ideal2/Desktop/flydragon/flyd.conf") == false)
    {
        printf("配置文件读取失败，退出！\n");
        exit(1);
    }
    int port = conf.GetIntDefault("ListenPort", 0);
    printf("port = %d\n", port);
    const char *pDBInfo = conf.GetString("DBInfo");
    if(pDBInfo != NULL)
    {
        printf("DBInfo = %s\n", pDBInfo);
    }

    for(int i = 0; environ[i]; i++)
    {
        printf("environ[%d] 地址 = %x  ",i , (uint)(u_long)(environ[i]));
        printf("environ[%d] 内容 = %s  \n",i , (environ[i]));
    }

    std::cout << "this is test cmake" << std::endl;
    return 0;
}
