#ifndef FLY_SIGNAL
#define FLY_SIGNAL
#include <unordered_map>
#include <signal.h>
#include <functional>
#include "logging/Logging.h"

typedef  void(*SigFunction)(int,siginfo_t *, void*);
typedef std::unordered_map<int, SigFunction>  SignalMap;
namespace flyd{
 class Signal{
  public:
     Signal(){};
     ~Signal(){};

  public:
        static void SignalHandler(int signo, siginfo_t *siginfo, void *ucontext);

        void Init();
        void AddSingal(int signo, SigFunction sighandler);

 public:
        static SignalMap signals_;
    };

}



#endif