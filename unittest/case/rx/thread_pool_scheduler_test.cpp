#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/scheduler/thread_pool_scheduler.h>

namespace zaf::testing {

class ThreadPoolSchedulerTest : public ::testing::Test {
protected:
    static std::size_t GetHybridQueueSize(const zaf::rx::ThreadPoolScheduler& scheduler) {
        return scheduler.HybridQueueSize();
    }
};

TEST_F(ThreadPoolSchedulerTest, DefaultInstance) {

    auto instance = zaf::rx::ThreadPoolScheduler::Default();
    std::size_t max_thread_count = std::thread::hardware_concurrency();
    if (max_thread_count == 0) {
        max_thread_count = 4;
    }
    ASSERT_EQ(instance->MaxThreadCount(), max_thread_count);
}


TEST_F(ThreadPoolSchedulerTest, DefaultConstructor) {

    zaf::rx::ThreadPoolScheduler scheduler;
    std::size_t max_thread_count = std::thread::hardware_concurrency();
    if (max_thread_count == 0) {
        max_thread_count = 4;
    }
    ASSERT_EQ(scheduler.MaxThreadCount(), max_thread_count);
    ASSERT_EQ(scheduler.CurrentThreadCount(), 0);
}


TEST_F(ThreadPoolSchedulerTest, ConstructorWithMaxThreadCount) {

    zaf::rx::ThreadPoolScheduler scheduler(10);
    ASSERT_EQ(scheduler.MaxThreadCount(), 10);
    ASSERT_EQ(scheduler.CurrentThreadCount(), 0);

    ASSERT_THROW(zaf::rx::ThreadPoolScheduler(0), zaf::PreconditionError);
}


TEST_F(ThreadPoolSchedulerTest, SchedulerWork_Precondition) {

    auto scheduler = zaf::rx::ThreadPoolScheduler::Default();
    ASSERT_THROW(scheduler->ScheduleWork(nullptr), zaf::PreconditionError);
}


TEST_F(ThreadPoolSchedulerTest, ScheduleWork) {

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    zaf::rx::ThreadPoolScheduler scheduler;
    std::optional<std::thread::id> execute_thread_id;
    auto disposable = scheduler.ScheduleWork([&] {
        {
            std::lock_guard<std::mutex> lock(mutex);
            execute_thread_id = std::this_thread::get_id();
        }
        cv.notify_one();
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());
    ASSERT_EQ(scheduler.CurrentThreadCount(), 1);

    cv.wait(lock);
    // Wait a moment to ensure the work item is disposed.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(execute_thread_id.has_value());
    ASSERT_NE(*execute_thread_id, std::this_thread::get_id());
}


TEST_F(ThreadPoolSchedulerTest, ScheduleWork_NotMoreThanMaxThreadCount) {

    zaf::rx::ThreadPoolScheduler scheduler(3);

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::optional<std::thread::id> thread_id1;
    std::optional<std::thread::id> thread_id2;
    std::optional<std::thread::id> thread_id3;

    scheduler.ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        {
            std::lock_guard<std::mutex> lock(mutex);
            thread_id1 = std::this_thread::get_id();
        }
        cv.notify_one();
    });
    scheduler.ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        {
            std::lock_guard<std::mutex> lock(mutex);
            thread_id2 = std::this_thread::get_id();
        }
        cv.notify_one();
    });
    scheduler.ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        {
            std::lock_guard<std::mutex> lock(mutex);
            thread_id3 = std::this_thread::get_id();
        }
        cv.notify_one();
    });

    cv.wait(lock, [&]() {
        return thread_id1.has_value() && thread_id2.has_value() && thread_id3.has_value();
    });
    ASSERT_NE(thread_id1, thread_id2);
    ASSERT_NE(thread_id1, thread_id3);
    ASSERT_NE(thread_id2, thread_id3);
    ASSERT_EQ(scheduler.CurrentThreadCount(), 3);
}


TEST_F(ThreadPoolSchedulerTest, ScheduleWork_MoreThanMaxThreadCount) {

    zaf::rx::ThreadPoolScheduler scheduler(1);

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::optional<std::thread::id> thread_id1;
    std::optional<std::thread::id> thread_id2;
    std::optional<std::thread::id> thread_id3;
    scheduler.ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        {
            std::lock_guard<std::mutex> lock(mutex);
            thread_id1 = std::this_thread::get_id();
        }
        cv.notify_one();
    });
    scheduler.ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        {
            std::lock_guard<std::mutex> lock(mutex);
            thread_id2 = std::this_thread::get_id();
        }
        cv.notify_one();
    });
    scheduler.ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        {
            std::lock_guard<std::mutex> lock(mutex);
            thread_id3 = std::this_thread::get_id();
        }
        cv.notify_one();
    });
    cv.wait(lock, [&]() {
        return thread_id1.has_value() && thread_id2.has_value() && thread_id3.has_value();
    });
    ASSERT_EQ(thread_id1, thread_id2);
    ASSERT_EQ(thread_id1, thread_id3);
    ASSERT_EQ(thread_id2, thread_id3);
    ASSERT_EQ(scheduler.CurrentThreadCount(), 1);
}


TEST_F(ThreadPoolSchedulerTest, ScheduleWork_SerialWorks) {

    zaf::rx::ThreadPoolScheduler scheduler(2);

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    scheduler.ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::lock_guard<std::mutex> lock(mutex);
        cv.notify_one();
    });
    cv.wait(lock);

    scheduler.ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::lock_guard<std::mutex> lock(mutex);
        cv.notify_one();
    });
    cv.wait(lock);

    ASSERT_EQ(scheduler.CurrentThreadCount(), 1);
}


TEST_F(ThreadPoolSchedulerTest, ScheduleWork_CancelQueuedWork) {

    zaf::rx::ThreadPoolScheduler scheduler(1);

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    // Schedule one work to make the thread busy.
    scheduler.ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        cv.notify_one();
    });

    // Schedule another work which will be queued.
    bool is_executed{};
    auto disposable = scheduler.ScheduleWork([&]() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            is_executed = true;
        }
        cv.notify_one();
    });
    // Cancel the queued work.
    disposable->Dispose();
    ASSERT_TRUE(disposable->IsDisposed());

    cv.wait(lock);
    // Wait a moment to ensure the second work item won't be executed.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(is_executed);
    ASSERT_EQ(GetHybridQueueSize(scheduler), 0);
}


TEST_F(ThreadPoolSchedulerTest, SchedulerDelayedWork_Precondition) {

    auto scheduler = zaf::rx::ThreadPoolScheduler::Default();
    ASSERT_THROW(
        scheduler->ScheduleDelayedWork(std::chrono::milliseconds(10), nullptr),
        zaf::PreconditionError);
}


TEST_F(ThreadPoolSchedulerTest, SchedulerDelayedWork) {

    zaf::rx::ThreadPoolScheduler scheduler;

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    auto begin = std::chrono::steady_clock::now();
    bool is_execute{};
    auto disposable = scheduler.ScheduleDelayedWork(std::chrono::milliseconds(50), [&]() {
        {
            std::lock_guard<std::mutex> lock(mutex);
            is_execute = true;
        }
        cv.notify_one();
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    cv.wait(lock);
    // Wait a moment to ensure the work item is disposed.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    auto end = std::chrono::steady_clock::now();
    ASSERT_GE(end - begin, std::chrono::milliseconds(50));
    ASSERT_TRUE(is_execute);
    ASSERT_TRUE(disposable->IsDisposed());
}


TEST_F(ThreadPoolSchedulerTest, SchedulerDelayedWork_Cancel) {

    zaf::rx::ThreadPoolScheduler scheduler;

    bool is_execute{};
    auto disposable = scheduler.ScheduleDelayedWork(std::chrono::milliseconds(50), [&]() {
        is_execute = true;
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    disposable->Dispose();
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_EQ(GetHybridQueueSize(scheduler), 0);

    // Wait a moment to ensure the work item won't be executed.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(is_execute);
}


TEST_F(ThreadPoolSchedulerTest, Destruct) {

    std::optional<zaf::rx::ThreadPoolScheduler> scheduler;
    scheduler.emplace(2);

    std::atomic<std::size_t> executed_count{};
    for (int count = 0; count < 4; ++count) {
        scheduler->ScheduleWork([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            ++executed_count;
        });
    }

    // This destructs the scheduler and waits all works done.
    scheduler.reset();

    ASSERT_EQ(executed_count, 4);
}


TEST_F(ThreadPoolSchedulerTest, ScheduleWorkWhileDestructing) {

    std::optional<zaf::rx::ThreadPoolScheduler> scheduler;
    scheduler.emplace(2);

    scheduler->ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        // Posting new works to the scheduler while it is destructing.
        ASSERT_THROW(scheduler->ScheduleWork([]() {}), zaf::PreconditionError);
        ASSERT_THROW(scheduler->ScheduleDelayedWork(std::chrono::milliseconds(10), []() {}), 
            zaf::PreconditionError);
    });

    scheduler.reset();
}

}