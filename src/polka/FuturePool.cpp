#include <thread>
#include <queue>
#include <functional>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <future>


//Not used due to c++23 requirements || project use c++20
class FuturePool {
public:
    explicit FuturePool(std::size_t numThreads) : stop_(false) {
        results_.reserve(numThreads);
    }

    void enqueueTask(std::function<void()> task) {
        std::packaged_task<void()> packagedTask(std::move(task));
        results_.emplace_back(packagedTask.get_future());
        std::async(std::launch::async | std::launch::deferred, std::move(packagedTask));
    }

    void waitForCompletion() {
        for (auto& result : results_) {
            result.get();
        }
        results_.clear();
        stop_ = true;
    }

    ~FuturePool() {
        if (!stop_) waitForCompletion();
    }

private:
    std::vector<std::future<void>> results_;
    bool stop_;
};
