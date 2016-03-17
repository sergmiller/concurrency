//
//  main.cpp
//  futex
//
//  Created by Сергей Миллер on 17.03.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

//
//  main.cpp
//  mipt-concurrency
//
//  Created by Сергей Миллер on 15.03.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <chrono>
#include <mutex>
#include "futex.hpp"

#define MAX_THREADS 20
#define LIMIT_10_SEC (int)1e7

int hardware_concurrency;

template <class M>
void inc(M* mutex, int64_t limit, int64_t* sum, int64_t* thr_cnt) {
    while(1) {
        mutex->lock();
        if(*sum < limit) {
            ++*sum;
            ++*thr_cnt;
        } else {
            mutex->unlock();
            return;
        }
        mutex->unlock();
    }
}


template <class M>
void test_inc(int64_t limit, int thread_numb) {
    M mutex;
    int64_t sum = 0;
    std::vector <int64_t> counters(thread_numb,0);
    std::vector <std::thread> vt;
    for(int i = 0; i < thread_numb; ++i) {
        vt.push_back(std::thread(inc<M>,&mutex,limit, &sum, &counters[i]));
    }
    
    for(auto& t : vt) {
        t.join();
    }
    
    assert(sum == limit);
    
    for(int i = 0;i < thread_numb;++i) {
        sum -= counters[i];
    }
    
    assert(sum == 0);
};

template<class M>
void test_increment_to_limit(int limit, int thread_numb) {

    std::cout << "hardware_concurrency = " << hardware_concurrency << std::endl;
    std::cout << "limit = " << limit << std::endl;
   
    std::chrono::time_point<std::chrono::system_clock> start, end;
    for (int i = 1;i <= thread_numb; ++i) {
        start = std::chrono::system_clock::now();
        test_inc<M>(limit, i);
        end = std::chrono::system_clock::now();
        int64_t elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        std::cout << "threads: " << i << " time: " << elapsed_milliseconds << " ms" << std::endl;
    }
    std::cout << std::endl;
}

void case1() {
    //std::freopen("out.txt","w", stdout);
    hardware_concurrency = std::thread::hardware_concurrency();
    std::cout << "test futex:" << std::endl;
    test_increment_to_limit<futex>(LIMIT_10_SEC, MAX_THREADS);
    std::cout << "test std::mutex:" << std::endl;
    test_increment_to_limit<std::mutex>(LIMIT_10_SEC, MAX_THREADS);
}

int main(int argc, const char * argv[]) {
    //case1();
    //case2();
    return 0;
}


//key words: yeild, atomic, compare and exchange