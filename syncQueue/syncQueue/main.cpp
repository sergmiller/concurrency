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
#include "futex.hpp"

#define TEST_SIZE (int64_t)1e5

std::default_random_engine generator;
std::uniform_int_distribution<int64_t> distribution(-(int64_t)1e9,(int64_t)1e9);

std::default_random_engine generator_bool;
std::uniform_int_distribution<int> distribution_bool(0, 1);

template <class M>
void work(int64_t* control_sum, std::vector <int64_t>* values, std::vector <bool>* operations,     SyncQueue<int64_t, M>* queue) {
    int j = 0;
    for(int i =0;i < operations->size(); ++i) {
        if((*operations)[i]) {
            queue->push((*values)[j]);
            ++j;
        } else {
            queue->lock();
            while(queue->empty()) {
                std::this_thread::yield();
            }
            int64_t val = queue->front();
            control_sum -= val;
            queue->pop();
            queue->unlock();
        }
    }
}

template <class M>
void case1(int THREAD_NUMB) {
    SyncQueue<int64_t, M> queue;
    std::vector<std:: vector <int64_t> > data(THREAD_NUMB, std::vector<int64_t>());
    std::vector<std:: vector <bool> > action(THREAD_NUMB, std::vector<bool>());
    int64_t control_sum = 0;
    int balance = 0;
    for(int i = 0;i < TEST_SIZE; ++i) {
        bool act = (i/TEST_SIZE < 0.7);
        if(act) {
            int64_t next_val = distribution(generator);
            data[i % THREAD_NUMB].push_back(next_val);
            control_sum += next_val;
        }
        
        balance += (act ? 1 : -1);
        action[i % THREAD_NUMB].push_back(act);
    }
    
    std::vector <std::thread> vt;
    
    
    
    for(int i =0;i < THREAD_NUMB; ++i) {
        vt.push_back(std::thread(work<M>,&control_sum, &data[i], &action[i], &queue));
    }
    
    
    while(balance--) {
        queue.lock();
        while(queue.empty()) {
            std::this_thread::yield();
        }
        control_sum -= queue.front();
        queue.pop();
        queue.unlock();
    }


    
    for(auto& t : vt) {
        t.join();
    }
    
    assert(queue.empty() && !control_sum);
}

int main(int argc, const char * argv[]) {
    case1<std::mutex>(1);
    case1<futex>(1);
    case1<std::mutex>(4);
    case1<futex>(4);
    case1<std::mutex>(8);
    case1<futex>(8);
    return 0;
}
