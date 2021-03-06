# LEVEL1:完成配置文件的读写

[^level1]: 2019年7月8日  23：00

第一步完成配置文件的读写，配置文件的加载采用一个Config单例类来实现。

### **优化1：首先是单例类的实现**

单例类的实现参考了muduo的pthread_once和视频的双锁机制，这两种方法写起来都比较麻烦，因此参考**meyers单例**，内部声明一个局部静态对象，C++11以后，规定局部静态对象只被声明一次，而且是线程安全的，局部静态对象声明期是到程序结束时为止，由编译器自己去管理，无需程序管理，因此省去了内存管理类。这个写法简洁有效。

### 优化2：然后是用智能指针代替new

在这个单例类中有一个vector对象，存储每条配置信息的，每次有信息读入，需要new一个对象，并放入vector中，之前用的是用vector存指针，后来改为vector中存的是智能指针。这样可以避免用new来开辟内存，接下去的划，智能指针的效率损失不大，可以接受。

### 优化3：配置项的数据结构换成unodered_map，

一开始用的是结构体对象，存储一个项和一个内容放到vector中，每次找配置项都需要遍历一次vector，现在采用unordered_map来实现，unordered_map底层采用hash表实现，map采用红黑树实现，在无序的情况下哈希表插入和查找的效果比红黑树要好。（**具体是为什么？需要看一下**）

