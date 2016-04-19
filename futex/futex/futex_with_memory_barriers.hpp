//
//  futex_with_memory_barriers.hpp
//  futex
//
//  Created by Сергей Миллер on 19.04.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#ifndef futex_with_memory_barriers_hpp
#define futex_with_memory_barriers_hpp

#include <thread>
#include <atomic>
#include "futex.hpp"
#include <stdio.h>

class futex_with_memory_barriers_weak {
private:
    std::atomic<int> _locked_thread_id;
    int _get_thread_id();
public:
    futex_with_memory_barriers_weak();
    void lock(int id = -1);
    bool try_lock(int id = -1);
    void unlock(int id = -1);
};

class futex_with_memory_barriers_strong {
private:
    std::atomic<int> _locked_thread_id;
    int _get_thread_id();
public:
    futex_with_memory_barriers_strong();
    void lock(int id = -1);
    bool try_lock(int id = -1);
    void unlock(int id = -1);
};




#endif /* futex_with_memory_barriers_hpp */
