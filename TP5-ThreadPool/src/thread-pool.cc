// thread-pool.cc
#include "thread-pool.h"
#include <iostream>
#include <algorithm>

static mutex oslock;  // Protege salida por consola

ThreadPool::ThreadPool(size_t numThreads)
  : wts(numThreads)
  , done(false)
{
    for (size_t i = 0; i < numThreads; ++i) {
        wts[i].disponible    = true;
        wts[i].tareaRealizada = false;
        wts[i].identificador = int(i);
        wts[i].thunk         = nullptr;
        wts[i].ts = thread([this,i](){ worker(int(i)); });
    }
    dt = thread([this](){ dispatcher(); });
}

void ThreadPool::dispatcher() {
    while (true) {
        unique_lock<mutex> lock(queueLock);
        taskAvailable.wait(lock, [this]() {
            return done.load() || !taskQueue.empty();
        });

        if (done.load() && taskQueue.empty())
            break;

        int freeWorker = -1;
        for (size_t i = 0; i < wts.size(); ++i) {
            if (wts[i].disponible) {
                freeWorker = static_cast<int>(i);
                break;
            }
        }

        if (freeWorker == -1) {
            continue;
        }

        if (taskQueue.empty()) {
            continue;
        }

        function<void(void)> task = std::move(taskQueue.front());
        taskQueue.pop();

        wts[freeWorker].disponible = false;
        wts[freeWorker].thunk = std::move(task);

        lock.unlock();
        wts[freeWorker].semaforo.signal();
    }

    for (auto& w : wts)
        w.semaforo.signal();
}



void ThreadPool::worker(int id) {
    while (true) {
        wts[id].semaforo.wait();

        if (done.load() && wts[id].thunk == nullptr)
            break;

        if (wts[id].thunk) {
            try {
                wts[id].thunk();
                {
                    lock_guard<mutex> guard(oslock);
                }
                wts[id].tareaRealizada = true;
                wts[id].thunk = nullptr;
            } 
            catch (const exception& e) {
                lock_guard<mutex> guard(oslock);
            }
        }

        
        unique_lock<mutex> lock(queueLock);
        wts[id].disponible = true;
        taskAvailable.notify_all();  
        
    }
}


void ThreadPool::schedule(const function<void(void)>& thunk) {
    
    unique_lock<mutex> lock(queueLock);
    taskQueue.push(thunk);
    
    taskAvailable.notify_one();
}

void ThreadPool::wait() {
    unique_lock<mutex> lock(queueLock);
    taskAvailable.wait(lock, [this]() {
        if (!taskQueue.empty()) return false;
        for (const auto& w : wts) {
            if (!w.disponible) return false;
        }
        return true;
    });
}

ThreadPool::~ThreadPool() {
    {
        unique_lock<mutex> lock(queueLock);
        done.store(true);
        taskAvailable.notify_all();
    }
    if (dt.joinable()) dt.join();

    for (auto& w : wts) {
        if (w.ts.joinable())
            w.ts.join();
    }
}
