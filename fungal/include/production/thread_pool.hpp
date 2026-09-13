#pragma once

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <memory>
#include <vector>
#include <future>
#include <chrono>

namespace fungal::production {

class ThreadPool {
public:
    static ThreadPool& instance();

    // Initialize thread pool with specified number of threads
    void initialize(size_t num_threads = 0);

    // Submit a task and get future result
    template<typename F, typename... Args>
    auto submit_task(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<F, Args...>>;

    // Submit async task without waiting for result
    void submit_async(std::function<void()> task);

    // Stop the thread pool
    void shutdown();

    // Get number of active threads
    size_t get_thread_count() const;

    // Get number of pending tasks
    size_t get_pending_task_count() const;

    // Wait for all tasks to complete
    void wait_all();

    // Wait for all tasks with timeout
    bool wait_all_timeout(std::chrono::milliseconds timeout);

    // Set thread pool size
    void resize(size_t num_threads);

    // Get thread pool stats
    struct ThreadPoolStats {
        size_t active_threads;
        size_t total_threads;
        size_t pending_tasks;
        size_t completed_tasks;
    };

    ThreadPoolStats get_stats() const;

private:
    ThreadPool() = default;
    ~ThreadPool();

    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool shutdown_flag_ = false;
    size_t completed_tasks_ = 0;

    void worker_thread();
};

// Template implementation
template<typename F, typename... Args>
auto ThreadPool::submit_task(F&& f, Args&&... args)
    -> std::future<std::invoke_result_t<F, Args...>> {
    using return_type = std::invoke_result_t<F, Args...>;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if (shutdown_flag_) {
            throw std::runtime_error("ThreadPool is shutdown");
        }
        tasks_.emplace([task]() { (*task)(); });
    }
    cv_.notify_one();
    return res;
}

}  // namespace fungal::production
