#ifndef FLY_SIGNAL
#define FLY_SIGNAL
#include <unordered_map>
#include <signal.h>
#include <functional>

namespace flyd{
 class Signal{
  public:
     Signal(){};
     ~Signal(){};

  public://静态成员函数不区分对象 类成员属于类本身，可通过类名访问
//        static Signal& GetInstance()
//        {
//            static Signal value_;
//
//            return value_;
//        }

        static void SignalHandler(int signo, siginfo_t *siginfo, void *ucontext)
        {
            printf("signo == %d\n", signo);
        }
  public:
        typedef  void(*SigFunction)(int,siginfo_t *, void*);
        void Init();
        void AddSingal(int signo, SigFunction sighandler);

  public:

        typedef std::unordered_map<int, SigFunction>  SignalMap;
        SignalMap signals_;
    };

}

#endif