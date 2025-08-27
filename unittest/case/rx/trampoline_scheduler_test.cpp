#include <mutex>
#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/scheduler/trampoline_scheduler.h>
#include <zaf/rx/thread/default_run_loop_thread.h>

// All threads share the same instance of TrampolineScheduler.
TEST(TrampolineSchedulerTest, Instance) {
    
    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::shared_ptr<zaf::rx::TrampolineScheduler> thread1_scheduler;
    zaf::rx::DefaultRunLoopThread thread1;
    thread1.PostWork([&] {
        {
            std::lock_guard<std::mutex> lock_guard(mutex);
            thread1_scheduler = zaf::rx::TrampolineScheduler::Instance();
        }
        cv.notify_one();
    });

    std::shared_ptr<zaf::rx::TrampolineScheduler> thread2_scheduler;
    zaf::rx::DefaultRunLoopThread thread2;
    thread2.PostWork([&] {
        {
            std::lock_guard<std::mutex> lock_guard(mutex);
            thread2_scheduler = zaf::rx::TrampolineScheduler::Instance();
        }
        cv.notify_one();
    });

    auto main_thread_scheduler = zaf::rx::TrampolineScheduler::Instance();
    cv.wait(lock, [&]() {
        return thread1_scheduler && thread2_scheduler;
    });

    ASSERT_EQ(main_thread_scheduler, thread1_scheduler);
    ASSERT_EQ(main_thread_scheduler, thread2_scheduler);
    ASSERT_EQ(thread1_scheduler, thread2_scheduler);
}


// Each thread executes work on their own.
TEST(TrampolineSchedulerTest, DifferentThread) {

    std::optional<std::thread::id> thread1_id;
    zaf::rx::DefaultRunLoopThread().PostWork([&]() {
        zaf::rx::TrampolineScheduler::Instance()->ScheduleWork([&]() {
            thread1_id = std::this_thread::get_id();
        });
    });

    std::optional<std::thread::id> thread2_id;
    zaf::rx::DefaultRunLoopThread().PostWork([&]() {
        zaf::rx::TrampolineScheduler::Instance()->ScheduleWork([&]() {
            thread2_id = std::this_thread::get_id();
        });
    });

    std::optional<std::thread::id> main_thread_id;
    zaf::rx::TrampolineScheduler::Instance()->ScheduleWork([&]() {
        main_thread_id = std::this_thread::get_id();
    });

    ASSERT_TRUE(thread1_id.has_value());
    ASSERT_TRUE(thread2_id.has_value());
    ASSERT_TRUE(main_thread_id.has_value());

    ASSERT_NE(main_thread_id, thread1_id);
    ASSERT_NE(main_thread_id, thread2_id);
    ASSERT_NE(thread1_id, thread2_id);
    ASSERT_EQ(main_thread_id, std::this_thread::get_id());
}


TEST(TrampolineSchedulerTest, SchedulerWork_Precondition) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();
    ASSERT_THROW(scheduler->ScheduleWork(nullptr), zaf::PreconditionError);
}


TEST(TrampolineSchedulerTest, SchedulerWork_Disposable) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();

    std::shared_ptr<zaf::rx::Disposable> inner_disposable;
    bool inner_disposed_callback_called{};
    auto disposable = scheduler->ScheduleWork([&]() {

        inner_disposable = scheduler->ScheduleWork([]() {
            // Do nothing.
        });
        ASSERT_NE(inner_disposable, nullptr);
        // Recursive work is not executed immediately, so it is not disposed.
        ASSERT_FALSE(inner_disposable->IsDisposed());

        inner_disposable->AddDisposedCallback([&]() {
            inner_disposed_callback_called = true;
        });
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_NE(inner_disposable, nullptr);
    // The inner work has been executed, so it is disposed.
    ASSERT_TRUE(inner_disposable->IsDisposed());
    ASSERT_TRUE(inner_disposed_callback_called);
}


TEST(TrampolineSchedulerTest, SchedulerWork_CancelWork) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();

    std::shared_ptr<zaf::rx::Disposable> inner_disposable;
    bool executed{};
    auto disposable = scheduler->ScheduleWork([&]() {

        inner_disposable = scheduler->ScheduleWork([&]() {
            executed = true;
        });

        ASSERT_NE(inner_disposable, nullptr);
        ASSERT_FALSE(inner_disposable->IsDisposed());

        bool inner_disposed_callback_called{};
        inner_disposable->AddDisposedCallback([&]() {
            inner_disposed_callback_called = true;
        });
        
        // Dispose the inner work before it is executed.
        inner_disposable->Dispose();
        ASSERT_TRUE(inner_disposable->IsDisposed());
        ASSERT_TRUE(inner_disposed_callback_called);
    });

    ASSERT_NE(disposable, nullptr);
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_NE(inner_disposable, nullptr);
    // The inner work has been disposed before execution, so it is disposed.
    ASSERT_TRUE(inner_disposable->IsDisposed());
    ASSERT_FALSE(executed);
}


TEST(TrampolineSchedulerTest, ScheduleWork_Serial) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();
    std::vector<int> values;

    scheduler->ScheduleWork([&]() {
        values.push_back(1);
    });
    ASSERT_EQ(values, (std::vector<int>{ 1 }));

    scheduler->ScheduleWork([&]() {
        values.push_back(2);
    });
    ASSERT_EQ(values, (std::vector<int>{ 1, 2 }));

    scheduler->ScheduleWork([&]() {
        values.push_back(3);
    });
    ASSERT_EQ(values, (std::vector<int>{ 1, 2, 3 }));
}


TEST(TrampolineSchedulerTest, ScheduleWork_Recursive) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();
    std::vector<int> values;

    auto disposable1 = scheduler->ScheduleWork([&]() {
        values.push_back(1);

        scheduler->ScheduleWork([&]() {
            values.push_back(2);

            scheduler->ScheduleWork([&]() {
                values.push_back(3);
            });

            values.push_back(4);
        });

        values.push_back(5);
    });

    ASSERT_EQ(values, (std::vector<int>{ 1, 5, 2, 4, 3 }));
}


TEST(TrampolineSchedulerTest, ScheduleDelayedWork_Precondition) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();
    ASSERT_THROW(
        scheduler->ScheduleDelayedWork(std::chrono::milliseconds(10), nullptr),
        zaf::PreconditionError);
}


TEST(TrampolineSchedulerTest, ScheduleDelayedWork) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();

    auto begin = std::chrono::steady_clock::now();
    std::optional<std::chrono::steady_clock::time_point> end;
    auto disposable = scheduler->ScheduleDelayedWork(
        std::chrono::milliseconds(50),
        [&]() {
            end = std::chrono::steady_clock::now();
        });

    ASSERT_NE(disposable, nullptr);
    ASSERT_TRUE(disposable->IsDisposed());
    bool disposed_callback_called{};
    disposable->AddDisposedCallback([&]() {
        disposed_callback_called = true;
    });
    ASSERT_TRUE(disposed_callback_called);

    ASSERT_TRUE(end.has_value());
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(*end - begin).count();
    ASSERT_GE(elapsed, 50);
}


TEST(TrampolineSchedulerTest, CancelDelayedWorkOnOtherThread) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();

    auto begin = std::chrono::steady_clock::now();

    std::condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::shared_ptr<zaf::rx::Disposable> disposable;

    zaf::rx::DefaultRunLoopThread thread;
    thread.PostWork([&]() {
        std::lock_guard<std::mutex> lock(mutex);
        disposable->Dispose();
        cv.notify_one();
    });

    bool delay_executed{};
    bool disposed_callback_called{};
    scheduler->ScheduleWork([&]() {

        // This delayed work won't be executed, but the delay time should still be waited.
        disposable = scheduler->ScheduleDelayedWork(std::chrono::milliseconds(50), [&]() {
            delay_executed = true;
        });

        ASSERT_NE(disposable, nullptr);
        ASSERT_FALSE(disposable->IsDisposed());
        disposable->AddDisposedCallback([&]() {
            disposed_callback_called = true;
        });

        // Unlock the mutex to allow the other thread to dispose the work.
        lock.unlock();
    });

    cv.wait(lock, [&]() {
        return disposable->IsDisposed();
    });
    auto end = std::chrono::steady_clock::now();

    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(disposed_callback_called);
    ASSERT_FALSE(delay_executed);

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    ASSERT_GE(elapsed, 50);
}


TEST(TrampolineSchedulerTest, CancelRecursiveDelayedWork) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();

    auto begin = std::chrono::steady_clock::now();
    bool delay_executed{};
    auto disposable = scheduler->ScheduleWork([&]() {

        // This delayed work won't be executed, and the delay time should not be waited.
        auto disposable = scheduler->ScheduleDelayedWork(std::chrono::milliseconds(50), [&]() {
            delay_executed = true;
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
    });

    ASSERT_FALSE(delay_executed);

    auto end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    ASSERT_LT(elapsed, 50);
}


TEST(TrampolineSchedulerTest, ScheduleDelayedWork_Blocked) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();
    
    auto begin = std::chrono::steady_clock::now();
    std::optional<std::chrono::steady_clock::time_point> end;
    scheduler->ScheduleWork([&]() {

        scheduler->ScheduleDelayedWork(
            std::chrono::milliseconds(50),
            [&]() {
                end = std::chrono::steady_clock::now();
            });

        // Here blocks the thread. The block duration should be included in the delayed time.
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    });

    ASSERT_TRUE(end.has_value());

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(*end - begin).count();
    ASSERT_GE(elapsed, 50);
    ASSERT_LT(elapsed, 100);
}