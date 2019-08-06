//Copyright(C) Ideal Dragon. All rights reserved.
//Use of this source code is governed by GPL-style license
//Author: Ideal Dragon

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>    //uintptr_t
#include <stdarg.h>    //va_start....
#include <unistd.h>    //STDERR_FILENO等
#include <sys/time.h>  //gettimeofday
#include <time.h>      //localtime_r
#include <fcntl.h>     //open
#include <errno.h>     //errno
#include <sys/socket.h>
#include <sys/ioctl.h> //ioctl
#include <arpa/inet.h>

#include "flyd_global.h"
#include "flyd_func.h"
#include "flyd_socket.h"
#include "../logging/Logging.h"

//来数据时候的处理，当连接上有数据来的时候，本函数会被ngx_epoll_process_events()所调用  ,官方的类似函数为ngx_http_wait_request_handler();
void CSocekt::flyd_wait_request_handler(lp_connection_t c)
{
    //ngx_log_stderr(errno,"22222222222222222222222.");

    LOG_INFO << "2222222222222";
    //ET测试代码
    unsigned char buf[10]={0};
    memset(buf,0,sizeof(buf));
    do
    {
        int n = recv(c->fd,buf,2,0); //每次只收两个字节
        if(n == -1 && errno == EAGAIN)
            break; //数据收完了
        else if(n == 0)
            break;
        LOG_INFO << "OK，收到的字节数为" << n << "内容为" << buf;
    }while(1);



}
