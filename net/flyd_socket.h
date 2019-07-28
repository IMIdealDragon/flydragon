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

#define FLYD_LISTEN_BACKLOG 511  //已完成连接队列数目
#define FLYD_MAX_EVENTS     512  //最大连接数

typedef struct flyd_listening_s   flyd_listening_t, *lp_listening_t;//和监听端口有关的结构
typedef struct flyd_connection_s  flyd_connection_t,*lp_connection_t;//和连接有关的结构
typedef class  CSocekt            CSocekt;//socket类

typedef void (CSocekt::*flyd_event_handler_pt)(lp_connection_t c); //定义成员函数指针

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

    int                       fd;             //套接字句柄socket
    lp_listening_t            listening;      //如果这个链接被分配给了一个监听套接字，那么这个里边就指向监听套接字对应的那个lpngx_listening_t的内存首地址

    //------------------------------------
    unsigned                  instance:1;     //【位域】失效标志位：0：有效，1：失效【这个是官方nginx提供，到底有什么用，ngx_epoll_process_events()中详解】
    uint64_t                  iCurrsequence;  //我引入的一个序号，每次分配出去时+1，此法也有可能在一定程度上检测错包废包，具体怎么用，用到了再说
    struct sockaddr           s_sockaddr;     //保存对方地址信息用的
    //char                      addr_text[100]; //地址的文本信息，100足够，一般其实如果是ipv4地址，255.255.255.255，其实只需要20字节就够

    //和读有关的标志-----------------------
    //uint8_t                   r_ready;        //读准备好标记【暂时没闹明白官方要怎么用，所以先注释掉】
    uint8_t                   w_ready;        //写准备好标记

    flyd_event_handler_pt      rhandler;       //读事件的相关处理方法
    flyd_event_handler_pt      whandler;       //写事件的相关处理方法

    //--------------------------------------------------
    lp_connection_t             data;           //这是个指针【等价于传统链表里的next成员：后继指针】，指向下一个本类型对象，用于把空闲的连接池对象串起来构成一个单向链表，方便取用
};

//socket相关类
class CSocekt
{
public:
    CSocekt();                                                         //构造函数
    virtual ~CSocekt();                                                //释放函数
public:
    virtual bool Initialize(); //这竟然是个虚函数                                        //初始化函数

public:
    int  flyd_epoll_init();                                             //epoll功能初始化
    //void ngx_epoll_listenportstart();                                  //监听端口开始工作
    int  flyd_epoll_add_event(int fd,int readevent,int writeevent,uint32_t otherflag,uint32_t eventtype,lp_connection_t c);
    //epoll增加事件
    int  flyd_epoll_process_events(int timer);                          //epoll等待接收和处理事件

private:
    void ReadConf();                                                   //专门用于读各种配置项
    bool flyd_open_listening_sockets();                                 //监听必须的端口【支持多个端口】
    void flyd_close_listening_sockets();                                //关闭监听套接字
    bool setnonblocking(int sockfd);                                   //设置非阻塞套接字

    //一些业务处理函数handler
    void flyd_event_accept(lp_connection_t oldc);                    //建立新连接
    void flyd_wait_request_handler(lp_connection_t c);               //设置数据来时的读处理函数

    void flyd_close_accepted_connection(lp_connection_t c);          //用户连入，我们accept4()时，得到的socket在处理中产生失败，则资源用这个函数释放【因为这里涉及到好几个要释放的资源，所以写成函数】

    //获取对端信息相关
    size_t flyd_sock_ntop(struct sockaddr *sa,int port,u_char *text,size_t len);  //根据参数1给定的信息，获取地址端口字符串，返回这个字符串的长度

    //连接池 或 连接 相关
    lp_connection_t flyd_get_connection(int isock);                  //从连接池中获取一个空闲连接
    void flyd_free_connection(lp_connection_t c);                    //归还参数c所代表的连接到到连接池中

private:
    int                            m_worker_connections;               //epoll连接的最大项数
    int                            m_ListenPortCount;                  //所监听的端口数量
    int                            m_epollhandle;                      //epoll_create返回的句柄

    //和连接池有关的
    lp_connection_t             m_pconnections;                  //注意这里可是个指针，其实这是个连接池的首地址
    lp_connection_t             m_pfree_connections;             //空闲连接链表头，连接池中总是有某些连接被占用，为了快速在池中找到一个空闲的连接，我把空闲的连接专门用该成员记录;
    //【串成一串，其实这里指向的都是m_pconnections连接池里的没有被使用的成员】
    //lpngx_event_t                  m_pread_events;                     //指针，读事件数组
    //lpngx_event_t                  m_pwrite_events;                    //指针，写事件数组
    int                            m_connection_n;                     //当前进程中所有连接对象的总数【连接池大小】
    int                            m_free_connection_n;                //连接池中可用连接总数


    std::vector<lp_listening_t> m_ListenSocketList;                 //监听套接字队列

    struct epoll_event             m_events[FLYD_MAX_EVENTS];           //用于在epoll_wait()中承载返回的所发生的事件

};


#endif //FLYDRAGON_FLYD_SOCKET_H
