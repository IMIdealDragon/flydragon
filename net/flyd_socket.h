//Copyright(C) Ideal Dragon. All rights reserved.
//Use of this source code is governed by GPL-style license
//Author: Ideal Dragon
//
// Created by Administrator on 2019/7/28.
//

#ifndef FLYDRAGON_FLYD_SOCKET_H
#define FLYDRAGON_FLYD_SOCKET_H

#include <vector>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>
#include <atomic>
#include <list>
#include "../logging/Mutex.h"
#include "../logging/Atomic.h"
#include "../_include/flyd_comm.h"

#define FLYD_LISTEN_BACKLOG 511  //已完成连接队列数目
#define FLYD_MAX_EVENTS     512  //最大连接数

typedef struct flyd_listening_s   flyd_listening_t, *lp_listening_t;//和监听端口有关的结构
typedef struct flyd_connection_s  flyd_connection_t,*lp_connection_t;//和连接有关的结构
typedef class  CSocekt            CSocekt;//socket类

typedef void (CSocekt::*flyd_event_handler_pt)(lp_connection_t c); //定义成员函数指针

void flyd_process_events_and_timers();

//监听端口结构体具体定义
struct flyd_listening_s  //和监听端口有关的结构
{
    int                       port;        //监听的端口号
    int                       fd;          //套接字句柄socket
    lp_connection_t           connection;  //连接池中的一个连接，注意这是个指针
};

//连接结构体具体定义
struct flyd_connection_s
{
    flyd_connection_s();
    virtual ~flyd_connection_s();
    void GetOneToUse();
    void PutOneToFree();

    int                       fd;             //套接字句柄socket
    lp_listening_t            listening;      //如果这个链接被分配给了一个监听套接字，那么这个里边就指向监听套接字对应的那个lpngx_listening_t的内存首地址

    //------------------------------------
    unsigned                  instance:1;     //【位域】失效标志位：0：有效，1：失效【这个是官方nginx提供，到底有什么用，ngx_epoll_process_events()中详解】
    uint64_t                  iCurrsequence;  //我引入的一个序号，每次分配出去时+1，此法也有可能在一定程度上检测错包废包，具体怎么用，用到了再说
    struct sockaddr           s_sockaddr;     //保存对方地址信息用的
    //char                      addr_text[100]; //地址的文本信息，100足够，一般其实如果是ipv4地址，255.255.255.255，其实只需要20字节就够

    //和读有关的标志-----------------------
    //uint8_t                   r_ready;        //读准备好标记【暂时没闹明白官方要怎么用，所以先注释掉】
    uint8_t                    w_ready;        //写准备好标记

    flyd_event_handler_pt      rhandler;       //读事件的相关处理方法
    flyd_event_handler_pt      whandler;       //写事件的相关处理方法

    //--------------------------------------------------
    lp_connection_t             data;           //这是个指针【等价于传统链表里的next成员：后继指针】，指向下一个本类型对象，用于把空闲的连接池对象串起来构成一个单向链表，方便取用

    //和epoll事件有关的
    uint32_t                    events;        //epoll事件标志

    //和收包有关
    unsigned char              curStat;       //当前收包的状态
    char                        dataHeadInfo[_DATA_BUFSIZE_]; //用于保存收到的数据的包头信息,已经收到的包头部分的内容
    char                        *precvbuf;           //接收数据的缓冲区头指针，对收到补全的包有用，再收到数据要放的地址
    unsigned int               irecvlen;            //要收到多少数据，由这个变量指定，还要收多少个数据
    char                        *precvMemPointer;    //new出来的用于收包的内存首地址，释放用的

    bool                      ifnewrecvMem;       //如果我们成功的收到了包头，那么我们就要分配内存开始保存 包头+消息头+包体内容，这个标记用来标记是否我们new过内存，因为new过是需要释放的
    char                      *pnewMemPointer;   //new出来的用于收包的内存首地址，和ifnewrecvMem配对使用


    pthread_mutex_t             logicPorcMutex;       //逻辑处理的相关互斥量

    //发包有关
    muduo::AtomicInt32          iThrowsendCount;
    char                        *psendMemPointer;//发送完成后释放用的，整个数据的头指针，其实是 消息头 + 包头 + 包体
    char                        *psendbuf;     //发送数据的缓冲区的头指针，开始 其实是包头+包体
    unsigned int               isendlen;      //要发送多少数据

    //和回收有关
    time_t                      inRecyTime;    //入到资源回收站里去的时间

    //和心跳包有关
    time_t                      lastPingTime;  //上次ping的时间【上次发送心跳包的事件】

    //和网络安全相关
    uint64_t                    FloodkickLastTime;  //Flood攻击上次收到包的时间
    int                         FloodAttackCount;  //Flood攻击在该时间内收到包的次数统计
    muduo::AtomicInt32          iSendCount;     //发送队列中有的数据条目数，若client只发不收，则可能造成此数过大，依据此数做出踢出处理


};

//消息头，引入的目的是当收到数据包时，额外记录一些内容以备将来使用
typedef struct _STRUC_MSG_HEADER
{
    lp_connection_t pConn;    //记录对应的连接，注意这是个指针
    uint64_t        iCurrsequence;   //收到数据包时，记录对应连接的序号，将来能用于比较是否连接已经作废
}STRUC_MSG_HEADER, *LPSTRUC_MSG_HEADER;


//socket相关类
class CSocekt
{
public:
    CSocekt();                                                         //构造函数
    virtual ~CSocekt();                                                //释放函数
public:
    virtual bool Initialize(); //这竟然是个虚函数                                        //初始化函数
    virtual bool Initialize_subproc();
    virtual void Shutdown_subproc();
public:
    int  flyd_epoll_init();                                             //epoll功能初始化
    //void ngx_epoll_listenportstart();                                  //监听端口开始工作
    int  flyd_epoll_add_event(int fd,int readevent,int writeevent,uint32_t otherflag,uint32_t eventtype,lp_connection_t c);
    int  flyd_epoll_oper_event(int fd,uint32_t eventtype, uint32_t flag,int bcaction,lp_connection_t pConn);       
    //epoll增加事件
    int  flyd_epoll_process_events(int timer);                          //epoll等待接收和处理事件


    void msgSend(char *psendbuf);
    //初始化连接池
    void initconnection();
    void clearconnection();//清理连接池
    void inRecyConnectQueue(lp_connection_t pConn);

    static void* ServerSendQueueThread(void* threadData);
    static void* ServerRecyConnectionThread(void* threadData);

private:
    

    void ReadConf();                                                   //专门用于读各种配置项
    bool flyd_open_listening_sockets();                                 //监听必须的端口【支持多个端口】
    void flyd_close_listening_sockets();                                //关闭监听套接字
    bool setnonblocking(int sockfd);                                   //设置非阻塞套接字
    void clearMsgSendQueue();

    //一些业务处理函数handler
    void flyd_event_accept(lp_connection_t oldc);                    //建立新连接
    void flyd_wait_request_handler(lp_connection_t c);               //设置数据来时的读处理函数
    ssize_t recvproc(lp_connection_t pConn,char *buff,ssize_t buflen); //接收从客户端来的数据专用函数

    void flyd_wait_request_handler_proc_p1(lp_connection_t c);
    void flyd_wait_request_handler_proc_plast(lp_connection_t c);
    void inMsgRecvQueue(char *buf, int &imrqc); //buf这段内存 ： 消息头 + 包头 + 包体
    void tmpoutMsgRecvQueue();
    void clearMsgRecvQueue();

    virtual void threadRecvFunc();
    virtual void threadRecvProcFunc() = 0;


    void flyd_close_connection(lp_connection_t c);          //用户连入，我们accept4()时，得到的socket在处理中产生失败，则资源用这个函数释放【因为这里涉及到好几个要释放的资源，所以写成函数】

    //获取对端信息相关
    size_t flyd_sock_ntop(struct sockaddr *sa,int port,u_char *text,size_t len);  //根据参数1给定的信息，获取地址端口字符串，返回这个字符串的长度

    //连接池 或 连接 相关
    lp_connection_t flyd_get_connection(int isock);                  //从连接池中获取一个空闲连接
    void flyd_free_connection(lp_connection_t c);                    //归还参数c所代表的连接到到连接池中

protected:
    //和通讯相关的一些变量
    size_t                         m_iLenPkgHeader;   //sizeof(COMM_PKG_HEADER);
    size_t                         m_iLenMsgHeader;   //sizeof(STRUC_MSG_HEADER);
    std::list<char *>              m_MsgRecvQueue;          //接收数据消息队列
    muduo::AtomicInt32             m_iRecvMsgQueueCount;    //收消息队列大小 

private:
   struct ThreadItem   
    {
        pthread_t   _Handle;                        //线程句柄
        CSocekt     *_pThis;                        //记录线程池的指针	
        bool        ifrunning;                      //标记是否正式启动起来，启动起来后，才允许调用StopAll()来释放

        //构造函数
        ThreadItem(CSocekt *pthis):_pThis(pthis),ifrunning(false){}                             
        //析构函数
        ~ThreadItem(){}        
    };

    int                            m_worker_connections;               //epoll连接的最大项数
    int                            m_ListenPortCount;                  //所监听的端口数量
    int                            m_epollhandle;                      //epoll_create返回的句柄

    //和连接池有关的
    std::list<lp_connection_t>     m_connectionList;      //连接列表
    std::list<lp_connection_t>     m_freeconnectionList;  //空闲连接列表[这里边装的都是空闲连接]
    muduo::AtomicInt32             m_total_connection_n;  //连接池总的连接数
    muduo::AtomicInt32             m_free_connection_n;   //连接池空闲连接数
    muduo::MutexLock               m_connectionMutex;     //连接相关互斥量
    muduo::MutexLock               m_recyconnqueueMutex;  //连接回收队列相关的互斥量
    std::list<lp_connection_t>     m_recyconnectionList;  //将要释放的连接队列，这是为了延迟一段时间再释放
    muduo::AtomicInt32             m_total_recyconnection_n; //将要释放的队列的连接的大小
    int                            m_RecyConnectionWaitTime;  //等待这些秒后才回收


    std::vector<lp_listening_t>    m_ListenSocketList;                 //监听套接字队列

    struct epoll_event             m_events[FLYD_MAX_EVENTS];           //用于在epoll_wait()中承载返回的所发生的事件


   
    std::list<char*>               m_MsgSendQueue;            //发消息队列
    muduo::AtomicInt32             m_iSendMsgQueueCount;       //发送数据消息队列大小
    muduo::MutexLock               m_sendMessageQueueMutex;    //发消息队列锁变量

    muduo::MutexLock               m_recvMessageQueueMutex;          
    sem_t                          m_semEventSendQueue;        //处理发消息线程的信号量
    std::vector<ThreadItem *>      m_threadVector;

};


#endif //FLYDRAGON_FLYD_SOCKET_H
