//
//  futex.hpp
//  futex
//
//  Created by Сергей Миллер on 17.03.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#ifndef futex_hpp
#define futex_hpp

#include <thread>
#include <atomic>
#include <stdio.h>

class futex {
private:
    std::atomic<int> _locked_thread_id;
    int _get_thread_id();
public:
    futex();
    void lock(int id = -1);
    bool try_lock(int id = -1);
    void unlock(int id = -1);
};

class BadCallUnlockException: public std::exception
{
public:
    const char * what() const throw()
    {
        return "futex has already unlocked or locked by another thread";
    }
};

#endif /* futex_hpp */
