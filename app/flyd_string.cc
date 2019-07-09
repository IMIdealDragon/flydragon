
//Copyright(C) Ideal Dragon. All rights reserved.
//Use if this source code is governed by GPL-style license
//Author: Ideal Dragon


#include <stdio.h>
#include <string>
#include <string.h>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

//一行代码不超过80个字符
// trim from start
std::string &Ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                    std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
std::string &Rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
std::string &Trim(std::string &s) {
    return Ltrim(Rtrim(s));
}
