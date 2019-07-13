#include "flyd_signal.h"
#include <signal.h>


void flyd::Signal::Init() {
    AddSingal(SIGUSR1,flyd::Signal::SignalHandler);
}

void flyd::Signal::AddSingal(int signo, SigFunction sighandler)
{
    struct sigaction sa;// 定义一个管理信号的结构体
    signals_.insert({signo, sighandler});
    sa.sa_sigaction = sighandler;
    sa.sa_flags = SA_SIGINFO;//令sa_sigaction指定的信号处理程序生效
    sigemptyset(&sa.sa_mask);
    if( sigaction(signo, &sa, NULL) < 0)
        printf("注册失败\n");
}

