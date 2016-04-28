//
//  main.cpp
//  futex
//
//  Created by Сергей Миллер on 17.03.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <chrono>
#include <mutex>
#include "futex.hpp"
#include "futex_with_memory_barriers.hpp"

#define THREADS 4
#define LIMIT (int)5e8


template <class M,class T>
void inc(M* mutex, int64_t limit, T& sum, int64_t& thr_cnt) {
    while(1) {
        std::lock_guard<M> g(*mutex);
        if(sum < limit) {
            ++sum;
            ++thr_cnt;
        } else {
            return;
        }
    }
}


template <class M,class T>
int64_t test_inc(int64_t limit, int thread_numb) {
    M mutex;
    T sum = 0;
    std::vector<int64_t> counters(thread_numb,0);
    std::vector <std::thread> vt;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    for(int i = 0; i < thread_numb; ++i) {
        vt.push_back(std::thread(inc<M,T>,&mutex,limit, std::ref(sum), std::ref(counters[i])));
    }
    
    for(auto& t : vt) {
        t.join();
    }
    
    end = std::chrono::system_clock::now();
    int64_t elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();

    
    assert(sum == limit);
    
    for(int i = 0;i < thread_numb;++i) {
        sum -= counters[i];
    }
    
    assert(sum == 0);
    return elapsed_milliseconds;
};

template<class M,class T>
void test_increment_to_limit(int limit, int thread_numb) {
   
   //    for (int i = 1;i <= thread_numb; ++i) {
        int64_t elapsed_milliseconds = test_inc<M,T>(limit, thread_numb);
                std::cout << "threads: " << thread_numb << " time: " << elapsed_milliseconds/1000 << " s" << std::endl;
//    }
    std::cout << std::endl;
}

void case1() {
    //std::freopen("out.txt","w", stdout);
    std::cout << "case without volatile:"<< std::endl<<std::endl;
    
    std::cout << "test simple futex:" << std::endl;
    test_increment_to_limit<futex,int64_t>(LIMIT, THREADS);
    
    std::cout << "test futex with memory barriers for not volatile(strong):" << std::endl;
    test_increment_to_limit<futex_with_memory_barriers_strong,int64_t>(LIMIT, THREADS);
    
    std::cout << std::endl;
}

void case2() {
    std::cout << "case with volatile:"<< std::endl<<std::endl;
    
    std::cout << "test simple futex:" << std::endl;
    test_increment_to_limit<futex,volatile int64_t>(LIMIT, THREADS);
    
    std::cout << "test futex with memory barriers for not volatile(strong):" << std::endl;
    test_increment_to_limit<futex_with_memory_barriers_strong,volatile int64_t>(LIMIT, THREADS);
    
    std::cout << "test futex with memory barriers for volatile(weak):" << std::endl;
    test_increment_to_limit<futex_with_memory_barriers_weak, volatile int64_t>(LIMIT, THREADS);
    
    std::cout << std::endl;
}

int main(int argc, const char * argv[]) {
    std::cout << "hardware_concurrency = " << std::thread::hardware_concurrency() <<std::endl;
    std::cout << "limit = " << LIMIT << std::endl << std::endl;
    case1();
    case2();
    return 0;
}


//key words: yeild, atomic, compare and exchange