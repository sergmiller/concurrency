//
//  futex.hpp
//  Atomic-mutex
//
//  Created by Сергей Миллер on 15.03.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#ifndef futex_hpp
#define futex_hpp

#include <thread>
#include <atomic>
#include <stdio.h>

class futex {
private:
    std::atomic<int> id;
    int get_thread_id();
public:
    futex();
    void lock();
    bool try_lock();
    void unlock();
    //~futex();
};

#endif /* futex_hpp */
