//
//  syncqueue.hpp
//  syncQueue
//
//  Created by Сергей Миллер on 17.03.16.
//  Copyright © 2016 Serg Miller. All rights reserved.
//

#ifndef syncqueue_hpp
#define syncqueue_hpp

#include <thread>
#include <atomic>
#include <queue>
#include <stdio.h>

template <class T>
class iConcurrencyQueue{
public:
    virtual ~iConcurrencyQueue(){}
    
    virtual void lock() = 0;
    virtual void unlock() = 0;
    
    virtual void pop() = 0;
    virtual void push(const T&) = 0;
    virtual const T& front() = 0;
    virtual const T& back() = 0;
    
    virtual bool empty() = 0;
    virtual int64_t size() = 0;
};

template <class T, class M = std::mutex>
class SyncQueue : public iConcurrencyQueue<T>{
private:
    std::queue <T> _queue;
    M _sync;
    M _queue_locker;
public:
    int64_t size() {
        return _queue.size();
    }
    
    bool empty() {
        return _queue.empty();
    }
    
    void lock() {
        _queue_locker.lock();
    }
    
    void unlock() {
        _queue_locker.unlock();
    }
    
    void push(const T& value) {
        std::lock_guard<M> guard(_sync);
        _queue.push(value);
    }
    
    void pop() {
        std::lock_guard<M> guard(_sync);
        _queue.pop();
    }
    
    
    const T& front() {
        std::lock_guard<M> guard(_sync);
        return _queue.front();
    }
    
    const T& back() {
        std::lock_guard<M> guard(_sync);
        return _queue.back();
    }
};

#endif /* syncqueue_hpp */
