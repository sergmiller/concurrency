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

#define MAX_THREADS 10
#define LIMIT (int)1e5


template <class M>
void inc(M* mutex, int64_t limit, int64_t* sum, int64_t* thr_cnt) {
    while(1) {
        std::lock_guard<M> g(*mutex);
        if(*sum < limit) {
            ++*sum;
            ++*thr_cnt;
        } else {
            return;
        }
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
    std::cout << "hardware_concurrency = " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "test simple futex:" << std::endl;
    test_increment_to_limit<futex>(LIMIT, MAX_THREADS);
    std::cout << "test futex with memory barriers for volatile(weak):" << std::endl;
    test_increment_to_limit<futex_with_memory_barriers_weak>(LIMIT, MAX_THREADS);
    std::cout << "test futex with memory barriers for not volatile(strong):" << std::endl;
    test_increment_to_limit<futex_with_memory_barriers_strong>(LIMIT, MAX_THREADS);
    std::cout << "test std::mutex:" << std::endl;
    test_increment_to_limit<std::mutex>(LIMIT, MAX_THREADS);
}

int main(int argc, const char * argv[]) {
    case1();
    //case2();
    return 0;
}


//key words: yeild, atomic, compare and exchange