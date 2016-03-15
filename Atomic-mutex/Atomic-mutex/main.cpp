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
#include <mutex>
#include "futex.hpp"

#define MAX_THREADS 10


void work(futex* f, int num) {
    if(f->try_lock()) {
        std::cout<< "Hello world " << num << std::endl;
        f->unlock();
    }
}


int main(int argc, const char * argv[]) {
    futex f;
    std::vector <std::thread> vt;
    for(int i = 1; i <= MAX_THREADS; ++i) {
        vt.push_back(std::thread(work,&f,i));
    }
    
    for(auto& t : vt) {
        t.join();
    }
    return 0;
}


//key words: yeild, atomic, compare and exchange