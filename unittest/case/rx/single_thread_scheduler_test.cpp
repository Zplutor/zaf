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