#include "flyd_signal.h"
#include <signal.h>
#include "flyd_global.h"
#include <sys/wait.h>
#include <sys/types.h>

SignalMap flyd::Signal::signals_ ;
static void flyd_process_get_status(void);
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
        LOG_ERROR << signo << "信号注册失败\n";
    else{
        LOG_INFO << signo << "信号注册成功\n";
    }
}

void flyd::Signal::SignalHandler(int signo, siginfo_t *siginfo, void *ucontext)
{
    LOG_INFO << "来信号了";
    char            *action; //一个字符串，用于记录一个动作字符串以往日志文件中写
    if(signals_[signo] == NULL)
    {
        LOG_WARN << "该信号未注册";
    }

    action = (char* )" ";//信号的动作

    if(flyd_process == FLYD_MASTER_PROCESS)//MASTER进程，管理进程，信号处理比较多
    {

    }
    else if(flyd_process == FLYD_WORK_PROCESS)//WORK进程，具体干活的进程，处理信号比较少
    {

    }
    else{//未定义的其他进程，暂时什么都不干

    }

    if(siginfo && siginfo->si_pid)
    {
        LOG_INFO << "signal is " << signo << " ,si_pid is " << siginfo->si_pid;
    }
    else
    {
        LOG_INFO << "signal is " << signo ;
    }


    //子进程状态有变化，通常是意外退出【既然官方是在这里处理，我们也学习官方在这里处理】
    if (signo == SIGCHLD)
    {
        flyd_process_get_status(); //获取子进程的结束状态
    } //end if


}

static void flyd_process_get_status(void)
{
    pid_t            pid;
    int              status;
    int              err;
    int              one=0; //抄自官方nginx，应该是标记信号正常处理过一次

    //当你杀死一个子进程时，父进程会收到这个SIGCHLD信号。
    for ( ;; )
    {
        //waitpid，有人也用wait,但是掌握和使用waitpid即可；这个waitpid说白了获取子进程的终止状态，这样，子进程就不会成为僵尸进程了；
        //第一次waitpid返回一个> 0值，表示成功，后边显示 2019/01/14 21:43:38 [alert] 3375: pid = 3377 exited on signal 9【SIGKILL】
        //第二次再循环回来，再次调用waitpid会返回一个0，表示子进程还没结束，然后这里有return来退出；
        pid = waitpid(-1, &status, WNOHANG); //第一个参数为-1，表示等待任何子进程，
        //第二个参数：保存子进程的状态信息(大家如果想详细了解，可以百度一下)。
        //第三个参数：提供额外选项，WNOHANG表示不要阻塞，让这个waitpid()立即返回

        if(pid == 0) //子进程没结束，会立即返回这个数字，但这里应该不是这个数字【因为一般是子进程退出时会执行到这个函数】
        {
            return;
        } //end if(pid == 0)
        //-------------------------------
        if(pid == -1)//这表示这个waitpid调用有错误，有错误也理解返回出去，我们管不了这么多
        {
            //这里处理代码抄自官方nginx，主要目的是打印一些日志。考虑到这些代码也许比较成熟，所以，就基本保持原样照抄吧；
            err = errno;
            if(err == EINTR)           //调用被某个信号中断
            {
                continue;
            }

            if(err == ECHILD  && one)  //没有子进程
            {
                return;
            }

            if (err == ECHILD)         //没有子进程
            {
                LOG_ERROR << "waitpid() failed!";
                return;
            }
            LOG_ERROR << "waitpid() failed!";
            return;
        }  //end if(pid == -1)
        //-------------------------------
        //走到这里，表示  成功【返回进程id】 ，这里根据官方写法，打印一些日志来记录子进程的退出
        one = 1;  //标记waitpid()返回了正常的返回值
        if(WTERMSIG(status))  //获取使子进程终止的信号编号
        {
            LOG_INFO << "pid = " <<pid << " exited on signal " << WTERMSIG(status) ; //获取使子进程终止的信号编号
        }
        else
        {
            LOG_INFO << "pid = "<< pid << "exited with code" << WEXITSTATUS(status); //WEXITSTATUS()获取子进程传递给exit或者_exit参数的低八位
        }
    } //end for
    return;
}
