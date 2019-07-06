//
// Created by ideal2 on 19-7-6.
//

#ifndef FLYDRAGON_FLYD_GLOBAL_H
#define FLYDRAGON_FLYD_GLOBAL_H

//定义类型别名CConfItem 类型指针，而c语言中是跟的对象名称
typedef struct g_ConfItem
{
    char ItemName[50];
    char ItemContent[500];
}CConfItem,*LPCConfItem;  //typedef struct 和c语言中不一样，


#endif //FLYDRAGON_FLYD_GLOBAL_H
