/*
 * @Copyright(C): Ideal Dragon. All rights reserved. 
 * @lisence: GPL
 * @Author: Ideal Dragon
 * @Date: 2019-07-12 15:47:37
 * @Description: 
 */

#include "flyd_signal.h"

static void signal_handler(int signo, siginfo_t *siginfo, void* ucontext)
{
    printf("this is signal process\n");
}

flyd::Signal::Signal()
{
    signals_.emplace(SIGHUP, signal_handler);
}
