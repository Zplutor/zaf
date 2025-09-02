#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/scheduler/single_thread_scheduler.h>

TEST(SingleThreadSchedulerTest, Constructor_Precondition) {
    ASSERT_THROW(zaf::rx::SingleThreadScheduler(nullptr), zaf::PreconditionError);
}


TEST(SingleThreadSchedulerTest, ScheduleWork_Precondition) {

    zaf::rx::SingleThreadScheduler scheduler;
    ASSERT_THROW(scheduler.ScheduleWork(nullptr), zaf::PreconditionError);
}


TEST(SingleThreadSchedulerTest, ScheduleDelayedWork_Precondition) {

    zaf::rx::SingleThreadScheduler scheduler;
    ASSERT_THROW(
        scheduler.ScheduleDelayedWork(std::chrono::milliseconds(10), nullptr),
        zaf::PreconditionError);
}


TEST(SingleThreadSchedulerTest, Destruct) {

    std::optional<zaf::rx::SingleThreadScheduler> scheduler;
    scheduler.emplace();

    bool is_executed{};
    scheduler->ScheduleWork([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        is_executed = true;

        ASSERT_THROW(scheduler->ScheduleWork([]() {}), zaf::PreconditionError);
        ASSERT_THROW(scheduler->ScheduleDelayedWork(std::chrono::milliseconds(10), []() {}),
            zaf::PreconditionError);
    });

    scheduler.reset();
    ASSERT_TRUE(is_executed);
}