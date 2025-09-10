#include <gtest/gtest.h>
#include <zaf/base/error/precondition_error.h>
#include <zaf/rx/scheduler/immediate_scheduler.h>

TEST(ImmediateSchedulerTest, SchedulerWork_Precondition) {

    auto scheduler = zaf::rx::ImmediateScheduler::Instance();
    ASSERT_THROW(scheduler->ScheduleWork(nullptr), zaf::PreconditionError);
}


TEST(ImmediateSchedulerTest, ScheduleWork) {

    auto scheduler = zaf::rx::ImmediateScheduler::Instance();
    bool executed = false;
    auto disposable = scheduler->ScheduleWork([&]() {
        executed = true;
    });
    ASSERT_NE(disposable, nullptr);
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(executed);
}


TEST(ImmediateSchedulerTest, ScheduleWork_Serial) {

    auto scheduler = zaf::rx::ImmediateScheduler::Instance();
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


TEST(ImmediateSchedulerTest, ScheduleWork_Recursive) {

    auto scheduler = zaf::rx::ImmediateScheduler::Instance();
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

    ASSERT_EQ(values, (std::vector<int>{ 1, 2, 3, 4, 5 }));
}


TEST(ImmediateSchedulerTest, ScheduleDelayedWork_Precondition) {

    auto scheduler = zaf::rx::ImmediateScheduler::Instance();
    ASSERT_THROW(
        scheduler->ScheduleDelayedWork(std::chrono::milliseconds(10), nullptr), 
        zaf::PreconditionError);
}


TEST(ImmediateSchedulerTest, ScheduleDelayedWork) {

    auto scheduler = zaf::rx::ImmediateScheduler::Instance();

    auto begin = std::chrono::steady_clock::now();
    std::optional<std::chrono::steady_clock::time_point> end;
    auto disposable = scheduler->ScheduleDelayedWork(
        std::chrono::milliseconds(50),
        [&]() {
            end = std::chrono::steady_clock::now();
        });

    ASSERT_NE(disposable, nullptr);
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(end.has_value());

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(*end - begin).count();
    ASSERT_GE(elapsed, 50);
}