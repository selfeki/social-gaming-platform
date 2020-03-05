#pragma once
#include <string>

class Nickname {
public:
    Nickname(std::string name);
    Nickname(Nickname& name);
    Nickname(Nickname&& nName);
    std::string getNickName();

private:
    bool isAlphaNum(std::string name);
    std::string nickName;
};
