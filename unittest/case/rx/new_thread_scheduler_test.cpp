#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/scheduler/new_thread_scheduler.h>

namespace zaf::testing {

class NewThreadSchedulerTest : public ::testing::Test {
protected:
    rx::NewThreadScheduler& Scheduler() noexcept {
        return scheduler_;
    }

    std::size_t ThreadCount() const noexcept {
        return scheduler_.ThreadCount();
    }

private:
    rx::NewThreadScheduler scheduler_;
};


TEST_F(NewThreadSchedulerTest, ScheduleWork_Precondition) {
    ASSERT_THROW(Scheduler().ScheduleWork(nullptr), zaf::PreconditionError);
}


TEST_F(NewThreadSchedulerTest, ScheduleWork_ThreadID) {

    std::optional<std::thread::id> thread_id1;
    std::optional<std::thread::id> thread_id2;

    Scheduler().ScheduleWork([&]() {
        thread_id1 = std::this_thread::get_id();
    });
    Scheduler().ScheduleWork([&]() {
        thread_id2 = std::this_thread::get_id();
    });

    // Wait for threads to terminate.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(thread_id1.has_value());
    ASSERT_TRUE(thread_id2.has_value());
    ASSERT_NE(std::this_thread::get_id(), *thread_id1);
    ASSERT_NE(std::this_thread::get_id(), *thread_id2);
    ASSERT_NE(*thread_id1, *thread_id2);
    ASSERT_EQ(ThreadCount(), 0);
}


TEST_F(NewThreadSchedulerTest, ScheduleWork_Disposable) {

    bool is_executed{};
    auto disposable = Scheduler().ScheduleWork([&]() {
        is_executed = true;
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    bool disposed_callback_called{};
    disposable->AddDisposedCallback([&]() {
        disposed_callback_called = true;
    });

    // Wait for thread to terminate.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_TRUE(is_executed);
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(disposed_callback_called);
}


TEST_F(NewThreadSchedulerTest, ScheduleDelayedWork_Precondition) {
    ASSERT_THROW(
        Scheduler().ScheduleDelayedWork(std::chrono::milliseconds(10), nullptr), 
        zaf::PreconditionError);
}


TEST_F(NewThreadSchedulerTest, ScheduleDelayedWork_ThreadID) {

    std::optional<std::thread::id> thread_id1;
    std::optional<std::thread::id> thread_id2;

    Scheduler().ScheduleDelayedWork(std::chrono::milliseconds(10), [&]() {
        thread_id1 = std::this_thread::get_id();
    });

    Scheduler().ScheduleDelayedWork(std::chrono::milliseconds(10), [&]() {
        thread_id2 = std::this_thread::get_id();
    });

    // Wait for threads to terminate.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(thread_id1.has_value());
    ASSERT_TRUE(thread_id2.has_value());
    ASSERT_NE(std::this_thread::get_id(), *thread_id1);
    ASSERT_NE(std::this_thread::get_id(), *thread_id2);
    ASSERT_NE(*thread_id1, *thread_id2);
    ASSERT_EQ(ThreadCount(), 0);
}


TEST_F(NewThreadSchedulerTest, ScheduleDelayedWork_Disposable) {

    bool is_executed{};
    auto disposable = Scheduler().ScheduleDelayedWork(std::chrono::milliseconds(20), [&]() {
        is_executed = true;
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());
    bool disposed_callback_called{};
    disposable->AddDisposedCallback([&]() {
        disposed_callback_called = true;
    });
    
    // Wait for thread to terminate.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_TRUE(is_executed);
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(disposed_callback_called);
}


TEST_F(NewThreadSchedulerTest, CancelDelayedWork) {

    bool is_executed{};
    auto disposable = Scheduler().ScheduleDelayedWork(std::chrono::milliseconds(20), [&]() {
        is_executed = true;
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    bool disposed_callback_called{};
    disposable->AddDisposedCallback([&]() {
        disposed_callback_called = true;
    });

    disposable->Dispose();
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(disposed_callback_called);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_FALSE(is_executed);
    ASSERT_EQ(ThreadCount(), 0);
}

}