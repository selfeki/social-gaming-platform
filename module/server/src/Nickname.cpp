#include "Nickname.h"

#include <algorithm>
#include <string>
using std::string;

Nickname::Nickname(std::string name)
    : nickName { name } {
    if (!isAlphaNum(name)) {
        //some error handling
    }
}

Nickname::Nickname(Nickname& other_name)
    : nickName { other_name.getNickName() } {
}

Nickname::Nickname(Nickname&& other_name)
    : nickName("") {
    nickName = other_name.getNickName();
    other_name.nickName = "";
}

std::string Nickname::getNickName() {
    return nickName;
}

bool isAlphaNum(const std::string& str) {
    for (char c : str) {
        if (!(isalnum(c)))
            return false;
    }
    return true;
}
