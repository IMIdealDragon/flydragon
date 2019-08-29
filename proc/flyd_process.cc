//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "flyd_process.h"
#include <sys/prctl.h>
#include <signal.h>
#include <net/flyd_socket.h>
#include "flyd_global.h"
#include "flyd_func.h"
#include "logging/Logging.h"

char* mater_name = "flyd_master";
extern pid_t flyd_pid, flyd_parent;
extern CSocekt g_socket;               //socket全局对象


namespace flyd {
//描述：守护进程初始化
//执行失败：返回-1，   子进程：返回0，父进程：返回1
    int flyd_daemon() {

        //1.给予守护进程最高文件权限
        umask(0);

        //2.fork子进程作为守护进程
        int flag;
        flag = fork();
        switch (flag)  //fork()出来这个子进程才会成为咱们这里讲解的master进程；
        {
            case -1:
                //创建子进程失败
                LOG_FATAL << "创建master子进程失败,退出\n";
                return -1;
            case 0:
                //master子进程，走到这里直接break;
                LOG_INFO << "THIS IS MASTER PROCESS\n ";
                prctl(PR_SET_NAME, mater_name);
                flyd_process = FLYD_MASTER_PROCESS;
                break;
            default:
                //父进程以往 直接退出exit(0);现在希望回到主流程去释放一些资源
                return 1;  //父进程直接返回1；
                // exit(0);
        } //end switch


        //以下都是master子进程的程序空间了
        //3.调用setsid设置新的会话
        if (setsid() == -1) {
            LOG_ERROR<<"flyd_daemon()中setsid()失败!\n";
            return -1;
        }

        //4.更改文件目录为根文件目录
        //5.关闭用不到的文件描述符
        int i = 0;
        if (i > STDERR_FILENO)  //fd应该是3，这个应该成立
        {
            if (close(i) == -1)  //释放资源这样这个文件描述符就可以被复用；不然这个数字【文件描述符】会被一直占着；
            {
                LOG_ERROR << "ngx_daemon()中close(fd)失败!\n";
                return -1;
            }
        }

        //6.重定向标准输入输出
        int fd = open("/dev/null", O_RDWR);
        if (fd == -1) {
            LOG_ERROR << "ngx_daemon()中open(\"/dev/null\")失败!\n";
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) == -1) //先关闭STDIN_FILENO[这是规矩，已经打开的描述符，动他之前，先close]，类似于指针指向null，让/dev/null成为标准输入；
        {
            LOG_ERROR << "ngx_daemon()中dup2(STDIN)失败!\n";
            return -1;
        }
        if (dup2(fd, STDOUT_FILENO) == -1) //再关闭STDIN_FILENO，类似于指针指向null，让/dev/null成为标准输出；
        {
            LOG_ERROR << "ngx_daemon()中dup2(STDOUT)失败!\n";
            return -1;
        }

        return 0;  //子进程返回
    }

//描述：创建worker子进程,并进入master主循环
    void flyd_master_process_cycle() {
        sigset_t set;        //信号集

        sigemptyset(&set);   //清空信号集

        //下列这些信号在执行本函数期间不希望收到【考虑到官方nginx中有这些信号，老师就都搬过来了】（保护不希望由信号中断的代码临界区）
        //建议fork()子进程时学习这种写法，防止信号的干扰；
        sigaddset(&set, SIGCHLD);     //子进程状态改变
        sigaddset(&set, SIGALRM);     //定时器超时
        sigaddset(&set, SIGIO);       //异步I/O
        sigaddset(&set, SIGINT);      //终端中断符
        sigaddset(&set, SIGHUP);      //连接断开
        sigaddset(&set, SIGUSR1);     //用户定义信号
        sigaddset(&set, SIGUSR2);     //用户定义信号
        sigaddset(&set, SIGWINCH);    //终端窗口大小改变
        sigaddset(&set, SIGTERM);     //终止
        sigaddset(&set, SIGQUIT);     //终端退出符
        //.........可以根据开发的实际需要往其中添加其他要屏蔽的信号......

        //设置，此时无法接受的信号；阻塞期间，你发过来的上述信号，多个会被合并为一个，暂存着，等你放开信号屏蔽后才能收到这些信号。。。
        //sigprocmask()在第三章第五节详细讲解过 ， 设置信号屏蔽
        if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) //第一个参数用了SIG_BLOCK表明设置 进程 新的信号屏蔽字 为 “当前信号屏蔽字 和 第二个参数指向的信号集的并集
        {
            LOG_ERROR << "信号屏蔽设置失败!";
        }
        //即便sigprocmask失败，程序流程 也继续往下走

        //从配置文件中读取要创建的worker进程数量
//    CConfig *p_config = CConfig::GetInstance(); //单例类
//    int workprocess = p_config->GetIntDefault("WorkerProcesses",1); //从配置文件中得到要创建的worker进程数量
        flyd_start_worker_processes(4);  //这里要创建worker子进程

        //清空信号屏蔽
        sigemptyset(&set); //信号屏蔽字为空，表示不屏蔽任何信号
        if (sigprocmask(SIG_SETMASK, &set, NULL) == -1) //第一个参数用了SIG_BLOCK表明设置 进程 新的信号屏蔽字 为 “当前信号屏蔽字 和 第二个参数指向的信号集的并集
        {
            LOG_ERROR << "清空信号屏蔽失败!";
        }

        dummyFlush();//进入主循环前将日志消息写入文件
        while(1) {

            //    usleep(100000);
            //ngx_log_error_core(0,0,"haha--这是父进程，pid为%P",ngx_pid);

           // sigsuspend(&set); //阻塞在这里，等待一个信号，此时进程是挂起的，不占用cpu时间，只有收到信号才会被唤醒（返回）；

            sleep(1); //休息1秒
            //以后扩充.......

        }// end for(;;)

    }

    static void flyd_start_worker_processes(int processnums) {
        int i;
        for (i = 0; i < processnums; i++)  //master进程在走这个循环，来创建若干个子进程
        {
            flyd_spawn_process(i, "flyd_worker");
        } //end for

    }

//描述：产生一个子进程
//inum：进程编号【0开始】
//pprocname：子进程名字"worker process"
    static int flyd_spawn_process(int inum, const char *pprocname) {
        pid_t pid;

        pid = fork(); //fork()系统调用产生子进程
        switch (pid)  //pid判断父子进程，分支处理
        {
            case -1: //产生子进程失败
                     LOG_FATAL << "ngx_spawn_process()fork()产生子进程num=%d,procname=\"%s\"失败!";
                return -1;

            case 0:  //子进程分支
                flyd_parent = getppid();              //因为是子进程了，所有原来的pid变成了父pid
                flyd_pid = getpid();                //重新获取pid,即本子进程的pid
                flyd_process = FLYD_WORK_PROCESS;
                flyd_worker_process_cycle(inum, pprocname);    //我希望所有worker子进程，在这个函数里不断循环着不出来，也就是说，子进程流程不往下边走;
                break;

            default: //这个应该是父进程分支，直接break;，流程往switch之后走
                break;
        }//end switch

        //父进程分支会走到这里，子进程流程不往下边走-------------------------
        //若有需要，以后再扩展增加其他代码......
        return pid;
    }


//描述：worker子进程的功能函数，每个woker子进程，就在这里循环着了（无限循环【处理网络事件和定时器事件以对外提供web服务】）
//     子进程分叉才会走到这里
//inum：进程编号【0开始】
static void flyd_worker_process_cycle(int inum, const char *pprocname) {
        //设置一下变量
//    ngx_p rocess = NGX_PROCESS_WORKER;  //设置进程的类型，是worker进程

        //重新为子进程设置进程名，不要与父进程重复------
        prctl(PR_SET_NAME, pprocname);
        flyd_worker_process_init(inum);
        LOG_INFO << "【worker进程】" << pprocname << flyd_pid <<"启动并开始运行......!"; //设置标题时顺便记录下来进程名，进程id等信息到日志

        dummyFlush();//进入主循环前让日志消息写入文件
        for (;;) {

            flyd_process_events_and_timers(); //处理网络事件和定时器事件
        } //end for(;;)


    }

//描述：子进程创建时调用本函数进行一些初始化工作
    static void flyd_worker_process_init(int inum) {

        sigset_t set;      //信号集

        sigemptyset(&set);  //清空信号集
        if (sigprocmask(SIG_SETMASK, &set, NULL) == -1)  //原来是屏蔽那10个信号【防止fork()期间收到信号导致混乱】，现在不再屏蔽任何信号【接收任何信号】
        {
            LOG_ERROR << "ngx_worker_process_init()中sigprocmask()失败!";
        }

        g_socket.flyd_epoll_init();
    }
}
