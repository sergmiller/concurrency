//
//  main.cpp
//  cache-ping-pong
//
//  Created by Сергей Миллер on 19.04.16.
//  Copyright © 2016 sergmiller. All rights reserved.
//

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cassert>

#define COUNT_BOUND 1e9

struct struct1 {
    int v1;
    int v2;
    int dummy[int(1e6)];
};

struct struct2 {
    int v1;
    int dummy[int(1e6)];
    int v2;
};

void count(int bound, int& v) {
    for(int i = 0;i < bound;++i) {
        ++v;
    }
}

int64_t timeForCount(int bound, int& v1, int& v2) {
    
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    std::thread th1(count, bound, std::ref(v1));
    std::thread th2(count, bound, std::ref(v2));
    
    th1.join();
    th2.join();
    
    end = std::chrono::system_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
}

int main() {
    struct1 s1;
    struct2 s2;
    int64_t t1 = timeForCount(COUNT_BOUND, s1.v1, s1.v2);
    int64_t t2 = timeForCount(COUNT_BOUND, s2.v1, s2.v2);
    std::cout << "with cache ping-pong: " << t1 << std::endl;
    std::cout << "without cache ping-pong: " << t2 << std::endl;
    return 0;
}
