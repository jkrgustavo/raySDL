#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <thread>
#include <utility>
#include <simd/simd.h>

struct RenderTask {
    uint start_x, start_y;
    uint end_x, end_y;

    bool is_shutdown = 0;
};

template<typename T>
class ThreadQueue {
private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable cv;

public:

    ThreadQueue() {}

    void push(T &value) {
        std::unique_lock<std::mutex> lock(mutex);
        queue.push(std::move(value)); 
        lock.unlock();
        cv.notify_one();
    }

    bool try_pop(T &value) {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.empty()) {
            return false;
        }
        value = std::move(queue.front());
        queue.pop();
        return true;
    }

    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]{ return !queue.empty(); });
        value = std::move(queue.front());
        queue.pop();
    }

    void wait_until(std::function<bool()> func) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock);
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    int size() {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }

};

struct ThreadManager {
    ThreadQueue<RenderTask> task_queue;
    ThreadQueue<RenderTask> completion_queue;
    std::vector<std::thread> thread_pool;
    std::vector<RenderTask> task_collection;
    int thread_count;

    ThreadManager(uint tex_width, uint tex_height)
        : thread_count(std::thread::hardware_concurrency()) 
    {
        task_collection = generate_tasks(tex_width, tex_height, thread_count);
    }


    template<typename Func, typename... Args>
    void threads_init(Func f, Args&&... args) {
        for (int i = 0; i < thread_count; i++) {
            thread_pool.emplace_back(f, std::forward<Args>(args)...);
        }
    }

    void push_tasks() {
        for (auto &task : task_collection) {
            task_queue.push(task);
        }
    }

    void wait_for_completion() {
        int completed = 0;
        while (completed < task_collection.size()) {
            RenderTask task;
            completion_queue.wait_and_pop(task);
            completed++;
        }
    }

    std::vector<RenderTask> generate_tasks(uint tex_width, uint tex_height, uint threads) {
        std::vector<RenderTask> tasks;
    
        int num_tiles_x = std::sqrt(threads);
        int num_tiles_y = threads / num_tiles_x;
        while (num_tiles_x * num_tiles_y != threads) {
            num_tiles_x++;
            num_tiles_y = threads / num_tiles_x;
        }

        int stride_i = tex_width / num_tiles_x / 16;
        int stride_j = tex_height / num_tiles_y / 16;

        for (uint j = 0; j < tex_height; j += stride_j) {
            for (uint i = 0; i < tex_width; i += stride_i) {
                uint u = std::min(i + (uint)stride_i, (uint)tex_width);
                uint v = std::min(j + (uint)stride_j, (uint)tex_height);

                tasks.emplace_back(RenderTask{
                    .start_x = i,
                    .start_y = j,
                    .end_x = u,
                    .end_y = v,
                });
            }
        }
        return tasks;
    }
};

