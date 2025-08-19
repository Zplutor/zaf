#include <mutex>
#include <gtest/gtest.h>
#include <zaf/rx/schedulers/trampoline_scheduler.h>
#include <zaf/rx/threads/default_run_loop_thread.h>

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

    scheduler->ScheduleWork([&]() {
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


TEST(TrampolineSchedulerTest, ScheduleDelayedWork) {

    auto scheduler = zaf::rx::TrampolineScheduler::Instance();

    auto begin = std::chrono::steady_clock::now();
    std::optional<std::chrono::steady_clock::time_point> end;
    auto disposable = scheduler->ScheduleDelayedWork(
        std::chrono::milliseconds(50),
        [&]() {
            end = std::chrono::steady_clock::now();
        });

    ASSERT_EQ(disposable, nullptr);
    ASSERT_TRUE(end.has_value());
    
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(*end - begin).count();
    ASSERT_GE(elapsed, 50);
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