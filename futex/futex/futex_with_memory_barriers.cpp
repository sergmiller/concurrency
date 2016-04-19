//
//  futex_with_memory_barriers.cpp
//  futex
//
//  Created by Сергей Миллер on 19.04.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#include "futex_with_memory_barriers.hpp"
#include <iostream>
#include <sstream>

futex_with_memory_barriers_weak::futex_with_memory_barriers_weak(): _locked_thread_id(-1){}

//dummy!!!
int futex_with_memory_barriers_weak::_get_thread_id() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return (int32_t)std::stoll(ss.str(), nullptr, 16);
}

void futex_with_memory_barriers_weak::lock(int id) {
    int thread_id = (id == -1 ? _get_thread_id() : id);
    int empty;
    while(!_locked_thread_id.compare_exchange_weak(empty = -1,thread_id,std::memory_order_relaxed)) {
        std::this_thread::yield();
    }
}

bool futex_with_memory_barriers_weak::try_lock(int id) {
    int thread_id = (id == -1 ? _get_thread_id() : id);
    int empty;
    return _locked_thread_id.compare_exchange_strong(empty = -1,thread_id,std::memory_order_relaxed);
}

void futex_with_memory_barriers_weak::unlock(int id) {
    int thread_id = (id == -1 ? _get_thread_id() : id);
    if(!_locked_thread_id.compare_exchange_strong(thread_id,-1,std::memory_order_relaxed)) {
        throw new BadCallUnlockException();
    }
}

futex_with_memory_barriers_strong::futex_with_memory_barriers_strong(): _locked_thread_id(-1){}

//dummy!!!
int futex_with_memory_barriers_strong::_get_thread_id() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    return (int32_t)std::stoll(ss.str(), nullptr, 16);
}

void futex_with_memory_barriers_strong::lock(int id) {
    int thread_id = (id == -1 ? _get_thread_id() : id);
    int empty;
    while(!_locked_thread_id.compare_exchange_weak(empty = -1,thread_id,std::memory_order_acquire)) {
        std::this_thread::yield();
    }
}

bool futex_with_memory_barriers_strong::try_lock(int id) {
    int thread_id = (id == -1 ? _get_thread_id() : id);
    int empty;
    return _locked_thread_id.compare_exchange_strong(empty = -1,thread_id,std::memory_order_acquire);
}

void futex_with_memory_barriers_strong::unlock(int id) {
    int thread_id = (id == -1 ? _get_thread_id() : id);
    if(!_locked_thread_id.compare_exchange_strong(thread_id,-1,std::memory_order_release)) {
        throw new BadCallUnlockException();
    }
}