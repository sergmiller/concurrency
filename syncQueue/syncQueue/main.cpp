//
//  main.cpp
//  syncQueue
//
//  Created by Сергей Миллер on 17.03.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <cassert>
#include <vector>
#include <random>
#include "syncqueue.hpp"

#define TEST_SIZE (int64_t)1e5

void _reader(int64_t size, std::shared_ptr<SyncQueue<int64_t> > queue) {
    for(int i = 0;i < size; ++i) {
        queue->pop();
    }
}

void _counter(int64_t size, std::shared_ptr<SyncQueue<int64_t> > queue, std::shared_ptr<int64_t> sum) {
    for(int i = 0;i < size; ++i) {
        std::lock_guard<SyncQueue<int64_t> > g(*queue);
        *sum -= queue->front();
        queue->pop();
    }
}

void _writer(int64_t size, std::shared_ptr<SyncQueue<int64_t> > queue) {
    for(int i = 0;i < size; ++i) {
        queue->push(i);
    }
}


void case1(int THREAD_NUMB) {
    std::cout << "TEST: push & pop,   threads = " << THREAD_NUMB << " ";
    std::shared_ptr <SyncQueue<int64_t> > queue(new SyncQueue<int64_t>);
    
    std::vector <std::thread> vt;
    
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    for(int i = 0;i < THREAD_NUMB; ++i) {
        vt.push_back(std::thread((i < THREAD_NUMB/2 ? _reader : _writer), TEST_SIZE, queue));
    }
    
    for(auto& t : vt) {
        t.join();
    }
    
    end = std::chrono::system_clock::now();
    assert(queue->empty());
    
    std::cout << "OK";
    int64_t elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout <<" time: " << elapsed_milliseconds << " ms" << std::endl;
    
    std::cout << "TEST: push & count, threads = " << THREAD_NUMB << " ";
    
    vt.clear();
    
    start = std::chrono::system_clock::now();
    
    std::shared_ptr<int64_t> sum(new int64_t);
    *sum = (THREAD_NUMB/2)*((TEST_SIZE - 1)*(TEST_SIZE))/2;
    
    for(int i = 0;i < THREAD_NUMB; ++i) {
        vt.push_back((i < THREAD_NUMB/2 ? std::thread(_counter,TEST_SIZE, queue, sum) : std::thread(_writer,TEST_SIZE, queue)));
    }
    
    for(auto& t : vt) {
        t.join();
    }
    
    end = std::chrono::system_clock::now();
    assert(queue->empty() && !*sum);
    
    std::cout << "OK";
    elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    std::cout <<" time: " << elapsed_milliseconds << " ms" << std::endl;
}

int main(int argc, const char * argv[]) {
    //freopen("out.txt", "w", stdout);
    case1(2);
    case1(4);
    case1(8);
    case1(16);
    return 0;
}
