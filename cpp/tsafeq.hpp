#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

// A simple and hopefully correct generic thread-safe queue class

template<class T>
class tsafeq_c {
    public:
        tsafeq_c() {};
        void push(T e) {
            std::lock_guard<std::mutex> lock(mtx);
            q.push(e);
            cv.notify_one();
        }

        // does not block on non-emptiness but will still block
        // to get the mutex
        bool pop_noblock(T& e) {
            std::unique_lock<std::mutex> lock(mtx);
            if (q.empty()) {
                return false;
            }
            e = q.front();
            q.pop();
            return true;
        }

        // this blocks on the mutex *and* non-emptiness
        T pop_block() {
            T elem;
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]{return !q.empty();});
            if (!q.empty()) {
                elem = q.front();
                q.pop();
            }
            return elem;
        }
    private:    
        std::mutex mtx;
        std::condition_variable cv;
        std::queue<T> q;
};

