
//  futex.cpp
//  Atomic-mutex
//
//  Created by Сергей Миллер on 15.03.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#include "futex.hpp"
#include <iostream>
#include <sstream>

futex::futex(){
    id = -1;
}

//dummy!!!
int futex::get_thread_id() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return (int32_t)std::stoll(ss.str(), nullptr, 16);
}

void futex::lock() {
    int thread_id = get_thread_id();
    int empty = -1;
    while(!id.compare_exchange_weak(empty = -1,thread_id)) {
        empty = -1;
        std::this_thread::yield();
    }
}

bool futex::try_lock() {
    int thread_id = get_thread_id();
    int empty = -1;
    return id.compare_exchange_weak(empty,thread_id);
}

void futex::unlock() {
    int thread_id = get_thread_id();
    if(!id.compare_exchange_weak(thread_id,-1)) {
        throw new std::exception();
    }
}