//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#ifndef FLYDRAGON_FLYD_SINGLETON_H
#define FLYDRAGON_FLYD_SINGLETON_H
namespace flyd {
    template<typename T>
    class Singleton {
    public:
        static T &getInstance() {
            static T value_; //静态局部变量
            return value_;
        }

    private:
        Singleton();

        ~Singleton();

        Singleton(const Singleton &); //拷贝构造函数
        Singleton &operator=(const Singleton &); // =运算符重载
    };

}
#endif //FLYDRAGON_FLYD_SINGLETON_H
