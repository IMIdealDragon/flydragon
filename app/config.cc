//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

#include <cstring>
#include "config.h"
#include "flyd_string.h"

using namespace flyd;


//构造函数
Config::Config() {

}
//析构函数
//vector<B*>指针是需要析构的，如果vector<B>则不需要
Config::~Config() {
//    std::vector<LPCConfItem >::iterator pos;
//    for(pos = config_list_.begin(); pos != config_list_.end(); ++pos)//++pos比pos++快
//    {
//        delete (*pos);
//    }
//    config_list_.clear();
}
//
//
//装载配置文件
bool Config::Load(const char *pconfName)
{
    FILE *fp;
    fp = fopen(pconfName, "r");
    if(fp == NULL)
        return false;

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

        //如果行尾有换行 回车 空格等都截取掉
        uint16_t end = strlen(linebuf) - 1;
        while(linebuf[end] == 10
              || linebuf[end] == 13
              || linebuf[end] == 32)
        {
            linebuf[end] = 0;
            end--;
        }

        if(linebuf[0] == 0)
            continue;
        if(*linebuf == '[')
            continue;

        //开始处理实际有效的配置文件
        char *ptmp = strchr(linebuf, '=');
        if(ptmp != NULL)
        {
            std::shared_ptr<CConfItem> confitem(new CConfItem);
            memset(confitem.get(), 0, sizeof(CConfItem));
            //等号左侧拷贝到confitem->ItemName
            strncpy(confitem->ItemName, linebuf, (int)(ptmp - linebuf));
            strcpy(confitem->ItemContent, ptmp + 1);

           //截取字符串首尾的空格
            Rtrim(confitem->ItemName);
            Ltrim(confitem->ItemName);
            Rtrim(confitem->ItemContent);
            Ltrim(confitem->ItemContent);

            config_vector_.push_back(confitem);

        }
    }  //end while(!feof(fp))
    ::fclose(fp);
    return true;
} //end Load


//根据ItemName获取配置信息字符串，不会有修改操作，不用互斥
const char *Config::GetString(const char *p_itemname) {
//    std::vector<LPCConfItem >::iterator pos;
//    for(pos = config_list_.begin(); pos != config_list_.end(); ++pos)
//    {
//        if(strcasecmp( (*pos)->ItemName, p_itemname) == 0)
//            return (*pos)->ItemContent;
//    }//end for


    for(auto pos : config_vector_)
    {
        if(strcasecmp( (*pos).ItemName, p_itemname) == 0)
            return (*pos).ItemContent;
    }

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