#include <thread>
#include <queue>
#include <functional>
#include <vector>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    explicit ThreadPool(std::size_t numThreads) : stop_(false) {
        for (std::size_t i = 0; i < numThreads; ++i) {
            threads_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty()) return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    void enqueueTask(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            tasks_.push(std::move(task));
        }
        condition_.notify_one();
    }

    void waitForCompletion() {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (std::thread& thread : threads_) {
            thread.join();
        }
    }

    ~ThreadPool() {
        if (!stop_) waitForCompletion();
    }

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool stop_;
};
