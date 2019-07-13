//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#include <cstring>
#include "flyd_config.h"
#include "flyd_string.h"
#include <iostream>
#include "flyd_func.h"

using namespace flyd;


//装载配置文件
bool Config::Load(const char *pconfName)
{
    FILE *fp;
    fp = fopen(pconfName, "r");
    if(fp == NULL) {
        return false;
    }
    //每一行配置文件读出来都放在这里
    char linebuf[501];//每行配置都不要太长，保持<500字符
    //文件成功打开
    while(!feof(fp)) //检查文件是否结束
    {
        if(fgets(linebuf, 500, fp) == NULL)//从文件中读出一行,若空则重新读
            continue;

        if(linebuf[0] == 0)
            continue;

        //处理注释行
        if(*linebuf == ';' || *linebuf == ' '
        || *linebuf == '#' || *linebuf == '\t' || *linebuf == '\n')
            continue;

        if(linebuf[0] == 0)
            continue;
        if(*linebuf == '[')
            continue;

        //开始处理实际有效的配置文件
        char *ptmp = strchr(linebuf, '=');//获得“=”的地址
        if(ptmp != NULL)
        {
            std::string item_name, item_content;

            item_name.append(linebuf, (int)(ptmp - linebuf));
            item_content.append(ptmp + 1);
            item_name = Trim(item_name);
            item_content = Trim(item_content);

            config_map_.insert({item_name, item_content});
        }
    }  //end while(!feof(fp))
    ::fclose(fp);
    return true;
} //end Load


bool StringCompare(std::string& a, std::string& b)
{
    return a == b;
}
//根据ItemName获取配置信息字符串，不会有修改操作，不用互斥
//修改为unordered_map存储，无序插入和删除操作效率较高
const char *Config::GetString(const char *p_itemname) {
    auto iter = config_map_.find(p_itemname);
    if (iter != config_map_.end())
        return (*iter).second.c_str();
    else
        return NULL;
}

//根据ItemName获取数字类型配置信息，不会有修改操作，不用互斥
int Config::GetIntDefault(const char *p_itemname, const int def) {

    auto s = GetString(p_itemname);
    if(s != NULL)
        return atoi(s);
    else
        return def;
}