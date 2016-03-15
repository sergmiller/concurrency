//
//  main.cpp
//  Atomic-mutex
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

int hardware_concurrency;
int limit1 = 1e7;
int limit2 = 4*1e7;
int sum = 0;

template <class T>
void inc(T* mutex, int64_t lim, int64_t* sum, int64_t* thr_cnt) {
    while(1) {
        mutex->lock();
        if(*sum < lim) {
            ++*sum;
            ++*thr_cnt;
        } else {
            mutex->unlock();
            return;
        }
        mutex->unlock();
    }
}


template <class T>
void test_inc(int64_t lim, int thread_numb) {
    T f;
    int64_t sum = 0;
    std::vector <int64_t> counters(thread_numb,0);
    std::vector <std::thread> vt;
    for(int i = 0; i < thread_numb; ++i) {
        vt.push_back(std::thread(inc<T>,&f,lim, &sum, &counters[i]));
    }
    
    for(auto& t : vt) {
        t.join();
    }
    
    assert(sum == lim);
    
    for(int i = 0;i < thread_numb;++i) {
        sum -= counters[i];
    }
    
    assert(sum == 0);
};

void test_increment_to_limit(int limit) {
    hardware_concurrency = std::thread::hardware_concurrency();
    std::cout << "hardware_concurrency = " << hardware_concurrency << std::endl;
    std::cout << "limit = " << limit << std::endl;
    std::cout << std::endl;
    float c[] = {0.5,2};
    int64_t time_mutex[2];
    int64_t time_futex[2];
    std::chrono::time_point<std::chrono::system_clock> start, end;
    for (int i = 0;i < 2; ++i) {
        start = std::chrono::system_clock::now();
        test_inc<std::mutex>(limit, (int)(hardware_concurrency * c[i]));
        end = std::chrono::system_clock::now();
        time_mutex[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        start = std::chrono::system_clock::now();
        test_inc<futex>(limit, (int)(hardware_concurrency * c[i]));
        end = std::chrono::system_clock::now();
        time_futex[i] = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
        std::cout << "threads: " << (int)(hardware_concurrency * c[i]) << std::endl;
        std::cout << "mutex worktime: " << time_mutex[i] << " ms" <<  std::endl;
        std::cout << "futex worktime: " << time_futex[i] << " ms" << std::endl;
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


int main(int argc, const char * argv[]) {
    std::freopen("out.txt","w", stdout);
    test_increment_to_limit(limit1);
    test_increment_to_limit(limit2);
    return 0;
}


//key words: yeild, atomic, compare and exchange