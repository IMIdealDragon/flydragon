/*
 * @Copyright(C): Ideal Dragon. All rights reserved. 
 * @lisence: GPL
 * @Author: Ideal Dragon
 * @Date: 2019-07-12 16:35:17
 * @Description: 
 */


#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <unordered_map>
#include <functional>
#include <siginfo.h>

namespace flyd{

typedef std::function<void(int signo, siginfo_t *siginfo, void *ucontext)> SignalHandler

class Signal() {
public:
    Singal();
    ~Signal(){ }
    Signal_Add(int signo,SignalHandler Handler);
    
    
private:
    std::unordered_map<int, SignalHandler> signals_;

    };

}
