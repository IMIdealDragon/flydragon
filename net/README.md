网络通信相关的源文件

1. **监听套接字**的读事件是新连接的到来，这个时候回调的epoll函数是，flyd_socket_accept.cc/CSocekt::flyd_event_accept
   在这个函数里面，会创建一个代表TCP连接的结构体 flyd_connection_s
   并会添加epoll的关心事件和相应的回调函数
2. **连接套接字**的读事件是新数据的到来，epoll的触发模式有ET和LT两种，这个模式在创建新连接的时候设置，新数据到来时候
    调用的是  flyd_socket_request.cc/flyd_wait_request_handler()
    