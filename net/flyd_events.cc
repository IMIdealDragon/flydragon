//Copyright(C) Ideal Dragon. All rights reserved.
//Use of this source code is governed by GPL-style license
//Author: Ideal Dragon

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>   //信号相关头文件
#include <errno.h>    //errno
#include <unistd.h>

#include <flyd_func.h>
#include <net/flyd_socket.h>
#include "../_include/flyd_global.h"



//处理网络事件和定时器事件，我们遵照nginx引入这个同名函数
void flyd_process_events_and_timers()
{
    g_socket.flyd_epoll_process_events(-1); //-1表示卡着等待把

    //...再完善
}

