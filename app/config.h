//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#ifndef FLYDRAGON_CONFIG_H
#define FLYDRAGON_CONFIG_H

#include <vector>
#include <memory>

#include "flyd_global.h"
//本项目的命名空间是flymd
//类名的命名规范是每个单词首字母大写
//该Config类读取config文件 采用meyers singleton,简单有效

namespace flyd{



    class Config{

    private:
        Config();
    public:
        ~Config();

    public://静态成员函数不区分对象 类成员属于类本身，可通过类名访问
        static Config& GetInstance()
        {
            static Config value_;

            return value_;
        }


    public:
        bool Load(const char *pconfName);//装载配置文件
        const char *GetString(const char *p_itemname);
        int GetIntDefault(const char *p_itemname, const int def);

    public:
        typedef std::vector<std::shared_ptr<CConfItem>> ConfigitemVector;
        ConfigitemVector  config_vector_;
    };

}



#endif //FLYDRAGON_CONFIG_H
