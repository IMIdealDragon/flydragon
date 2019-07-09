
//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon

//各种全局函数的声明
#ifndef FLYDRAGON_FLYD_FUNC_H
#define FLYDRAGON_FLYD_FUNC_H

void flyd_init_setproctitle();
void flyd_setproctitle(const char *title);

std::string &Ltrim(std::string &s);
std::string &Rtrim(std::string &s);
std::string &Trim(std::string &s);



#endif //FLYDRAGON_FLYD_FUNC_H
