#include <gtest/gtest.h>
#include <zaf/base/error/contract_error.h>
#include <zaf/rx/scheduler/thread_pool_scheduler.h>

TEST(ThreadPoolSchedulerTest, DefaultInstance) {

    auto instance = zaf::rx::ThreadPoolScheduler::Default();
    std::size_t max_thread_count = std::thread::hardware_concurrency();
    if (max_thread_count == 0) {
        max_thread_count = 4;
    }
    ASSERT_EQ(instance->MaxThreadCount(), max_thread_count);
}


TEST(ThreadPoolSchedulerTest, DefaultConstructor) {

    zaf::rx::ThreadPoolScheduler scheduler;
    std::size_t max_thread_count = std::thread::hardware_concurrency();
    if (max_thread_count == 0) {
        max_thread_count = 4;
    }
    ASSERT_EQ(scheduler.MaxThreadCount(), max_thread_count);
    ASSERT_EQ(scheduler.CurrentThreadCount(), 0);
}


TEST(ThreadPoolSchedulerTest, ConstructorWithMaxThreadCount) {

    zaf::rx::ThreadPoolScheduler scheduler(10);
    ASSERT_EQ(scheduler.MaxThreadCount(), 10);
    ASSERT_EQ(scheduler.CurrentThreadCount(), 0);

    ASSERT_THROW(zaf::rx::ThreadPoolScheduler(0), zaf::PreconditionError);
}