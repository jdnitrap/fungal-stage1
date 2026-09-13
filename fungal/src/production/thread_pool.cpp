#include "production/thread_pool.hpp"

namespace fungal::production {

ThreadPool& ThreadPool::instance() {
    static ThreadPool instance;
    return instance;
}

void ThreadPool::initialize(size_t num_threads) {
    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 2;
    }

    resize(num_threads);
}

void ThreadPool::submit_async(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        if (shutdown_flag_) {
            throw std::runtime_error("ThreadPool is shutdown");
        }
        tasks_.push(task);
    }
    cv_.notify_one();
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        shutdown_flag_ = true;
    }
    cv_.notify_all();

    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    workers_.clear();
}

size_t ThreadPool::get_thread_count() const {
    return workers_.size();
}

size_t ThreadPool::get_pending_task_count() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return tasks_.size();
}

void ThreadPool::wait_all() {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (tasks_.empty()) break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

bool ThreadPool::wait_all_timeout(std::chrono::milliseconds timeout) {
    auto start = std::chrono::steady_clock::now();
    while (true) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            if (tasks_.empty()) return true;
        }
        if (std::chrono::steady_clock::now() - start > timeout) {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void ThreadPool::resize(size_t num_threads) {
    size_t current_size = workers_.size();

    if (num_threads < current_size) {
        for (size_t i = num_threads; i < current_size; ++i) {
            if (workers_[i].joinable()) {
                workers_[i].join();
            }
        }
        workers_.resize(num_threads);
    } else if (num_threads > current_size) {
        for (size_t i = current_size; i < num_threads; ++i) {
            workers_.emplace_back(&ThreadPool::worker_thread, this);
        }
    }
}

ThreadPool::ThreadPoolStats ThreadPool::get_stats() const {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    return {
        workers_.size(),
        workers_.size(),
        tasks_.size(),
        completed_tasks_
    };
}

ThreadPool::~ThreadPool() {
    if (!shutdown_flag_) {
        shutdown();
    }
}

void ThreadPool::worker_thread() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            cv_.wait(lock, [this] { return !tasks_.empty() || shutdown_flag_; });

            if (shutdown_flag_ && tasks_.empty()) {
                break;
            }

            if (!tasks_.empty()) {
                task = std::move(tasks_.front());
                tasks_.pop();
            }
        }

        if (task) {
            try {
                task();
                ++completed_tasks_;
            } catch (...) {
                // Silently catch exceptions to prevent thread death
            }
        }
    }
}

}  // namespace fungal::production
