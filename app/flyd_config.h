//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#ifndef FLYDRAGON_FLYD_CONFIG_H
#define FLYDRAGON_FLYD_CONFIG_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

#include "flyd_global.h"
//本项目的命名空间是flyd
//类名的命名规范是每个单词首字母大写
//该Config类读取config文件 采用meyers singleton,简单有效

namespace flyd{
    class Config{
    public:
        Config(){};
        ~Config(){};

//    public://静态成员函数不区分对象 类成员属于类本身，可通过类名访问
//        static Config& GetInstance()
//        {
//            static Config value_;
//
//            return value_;
//        }


    public:
        bool Load(const char *pconfName);//装载配置文件
        const char *GetString(const char *p_itemname);
        int GetIntDefault(const char *p_itemname, const int def);

    public:
        typedef std::unordered_map<std::string, std::string>  ConfigitemMap;
        ConfigitemMap config_map_;
    };

}



#endif //FLYDRAGON_FLYD_CONFIG_H
