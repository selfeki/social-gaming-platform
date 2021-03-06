#pragma once

#include <ctime>
#include "Rule.h"


namespace gameSpecification {


struct TimerObject {
    
    TimerObject(double _duration, rule::Rule* _owner, rule::TimerMode _mode) : duration(_duration), owner(_owner), mode(_mode) {
        this->started_at = time(nullptr);
    }

    time_t started_at;
    double duration;
    rule::Rule* owner;
    rule::TimerMode mode;

    bool isTimeOut() {
        time_t cur_time = time(nullptr);
        double seconds_past = difftime(cur_time, started_at);
        if(seconds_past > duration) {
            return true;
        } else {
            return false;
        }
    }    
};

}


