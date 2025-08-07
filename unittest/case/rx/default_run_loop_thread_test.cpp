#include <gtest/gtest.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/thread/default_run_loop_thread.h>

namespace zaf::testing {

class DefaultRunLoopThreadTest : public ::testing::Test {
protected:
    void SetUp() override {
        run_loop_thread_.emplace();
    }

    void TearDown() override {
        run_loop_thread_.reset();
    }

    zaf::rx::DefaultRunLoopThread& RunLoopThread() {
        return *run_loop_thread_;
    }

    void StopRunLoopThread() {
        run_loop_thread_.reset();
    }

    std::unique_lock<std::mutex> GetLock() {
        return std::unique_lock<std::mutex>{ mutex_ };
    }

    // A wrapper method to expose the private PostWorkAt method for testing.
    std::shared_ptr<zaf::rx::Disposable> PostWorkAt(
        std::chrono::steady_clock::time_point execute_time_point, 
        zaf::Closure work) {
        return run_loop_thread_->PostWorkAt(execute_time_point, std::move(work));
    }

protected:
    std::condition_variable cv_;
    std::mutex mutex_;

private:
    std::optional<zaf::rx::DefaultRunLoopThread> run_loop_thread_;
};


TEST_F(DefaultRunLoopThreadTest, PostWork) {

    auto lock = GetLock();

    std::thread::id execute_thread_id = std::this_thread::get_id();
    RunLoopThread().PostWork([&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_thread_id = std::this_thread::get_id();
        }
        cv_.notify_one();
    });

    cv_.wait(lock);
    ASSERT_NE(execute_thread_id, std::this_thread::get_id());
}


TEST_F(DefaultRunLoopThreadTest, PostWork_ExecuteOrder) {

    auto lock = GetLock();

    std::vector<int> execute_order;
    RunLoopThread().PostWork([&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_order.push_back(1);
        }
        cv_.notify_one();
    });
    RunLoopThread().PostWork([&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_order.push_back(2);
        }
        cv_.notify_one();
    });
    RunLoopThread().PostWork([&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_order.push_back(3);
        }
        cv_.notify_one();
    });
    cv_.wait(lock, [&]() {
        return execute_order.size() == 3;
    });
    ASSERT_EQ(execute_order, (std::vector<int>{ 1, 2, 3 }));
}


TEST_F(DefaultRunLoopThreadTest, PostDelayedWork) {

    auto lock = GetLock();

    std::thread::id execute_thread_id = std::this_thread::get_id();

    auto disposable = RunLoopThread().PostDelayedWork(std::chrono::milliseconds(50), [&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_thread_id = std::this_thread::get_id();
        }
        cv_.notify_one();
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    cv_.wait(lock);
    ASSERT_NE(execute_thread_id, std::this_thread::get_id());
    ASSERT_TRUE(disposable->IsDisposed());
}


TEST_F(DefaultRunLoopThreadTest, PostDelayedWork_ExecuteOrder) {

    auto lock = GetLock();

    std::vector<int> execute_order;
    RunLoopThread().PostDelayedWork(std::chrono::milliseconds(50), [&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_order.push_back(1);
        }
        cv_.notify_one();
    });

    RunLoopThread().PostDelayedWork(std::chrono::milliseconds(30), [&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_order.push_back(2);
        }
        cv_.notify_one();
    });

    RunLoopThread().PostDelayedWork(std::chrono::milliseconds(10), [&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_order.push_back(3);
        }
        cv_.notify_one();
    });

    cv_.wait(lock, [&]() {
        return execute_order.size() == 3;
    });

    ASSERT_EQ(execute_order, (std::vector<int>{ 3, 2, 1 }));
}


// If two delayed works are executed at the same time point, they should be executed in the order 
// they are posted.
TEST_F(DefaultRunLoopThreadTest, PostDelayedWork_SameTimePoint) {

    auto lock = GetLock();

    std::vector<int> execute_order;
    auto time_point = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
    this->PostWorkAt(time_point, [&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_order.push_back(1);
        }
        cv_.notify_one();
    });

    this->PostWorkAt(time_point, [&]() {
        {
            std::lock_guard<std::mutex> lock_guard(mutex_);
            execute_order.push_back(2);
        }
        cv_.notify_one();
    });
    cv_.wait(lock, [&]() {
        return execute_order.size() == 2;
    });
    ASSERT_EQ(execute_order, (std::vector<int>{ 1, 2 }));
}


TEST_F(DefaultRunLoopThreadTest, CancelDelayedWork) {

    std::atomic<bool> is_work_executed{};
    auto disposable = RunLoopThread().PostDelayedWork(std::chrono::milliseconds(50), [&]() {
        is_work_executed = true;
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    disposable->Dispose();
    ASSERT_TRUE(disposable->IsDisposed());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(is_work_executed.load());
}


TEST_F(DefaultRunLoopThreadTest, CancelDelayedWork_SameTimePoint) {

    std::atomic<bool> is_work_executed{};
    auto time_point = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
    auto disposable1 = this->PostWorkAt(time_point, [&]() {
        is_work_executed = true;
    });
    auto disposable2 = this->PostWorkAt(time_point, [&]() {
        is_work_executed = true;
    });

    disposable2->Dispose();
    ASSERT_TRUE(disposable2->IsDisposed());

    disposable1->Dispose();
    ASSERT_TRUE(disposable1->IsDisposed());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(is_work_executed.load());
}


// All queued works should be executed before the thread exits.
TEST_F(DefaultRunLoopThreadTest, Destruct) {

    std::vector<int> execute_values;
    RunLoopThread().PostWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        execute_values.push_back(1);
    });

    // This work will be executed after the thread is stopped.
    RunLoopThread().PostWork([&]() {

        execute_values.push_back(2);

        // Post new work to a stopped thread should throw an exception.
        ASSERT_THROW(
            RunLoopThread().PostWork([&]() { execute_values.push_back(3); }),
            zaf::InvalidOperationError);

        ASSERT_THROW(
            RunLoopThread().PostDelayedWork({}, [&]() { execute_values.push_back(4); }),
            zaf::InvalidOperationError);
    });

    // Here will wait for the thread to stop.
    StopRunLoopThread();

    ASSERT_EQ(execute_values, (std::vector<int>{ 1, 2 }));
}

}