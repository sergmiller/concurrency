//
//  futex.cpp
//  futex
//
//  Created by Сергей Миллер on 17.03.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#include "futex.hpp"
#include <iostream>
#include <sstream>

futex::futex(): _locked_thread_id(-1){}

//dummy!!!
int futex::_get_thread_id() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return (int32_t)std::stoll(ss.str(), nullptr, 16);
}

void futex::lock(int id) {
    int thread_id = (id == -1 ? _get_thread_id() : id);
    int empty;
    while(!_locked_thread_id.compare_exchange_weak(empty = -1,thread_id)) {
        std::this_thread::yield();
    }
}

bool futex::try_lock(int id) {
    int thread_id = (id == -1 ? _get_thread_id() : id);
    int empty;
    return _locked_thread_id.compare_exchange_weak(empty = -1,thread_id);
}

void futex::unlock(int id) {
    int thread_id = (id == -1 ? _get_thread_id() : id);
    if(!_locked_thread_id.compare_exchange_weak(thread_id,-1)) {
        throw new BadCallUnlockException();
    }
}