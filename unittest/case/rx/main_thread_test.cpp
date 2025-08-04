#include <thread>
#include <gtest/gtest.h>
#include <zaf/rx/main_thread.h>

namespace {

class Work {
public:
    Work() = default;
    ~Work() {
        if (destruct_count_) {
            *destruct_count_ += 1;
        }
    }

    Work(const Work& other) = default;
    Work& operator=(const Work& other) = default;

    Work(Work&& other) : 
        execute_count_(std::move(other.execute_count_)), 
        destruct_count_(std::move(other.destruct_count_)) {

    }

    Work& operator=(Work&& other) {
        if (this != &other) {
            execute_count_ = std::move(other.execute_count_);
            destruct_count_ = std::move(other.destruct_count_);
        }
        return *this;
    }

    std::shared_ptr<int> ExecuteCount() const noexcept {
        return execute_count_;
    }

    std::shared_ptr<int> DestructCount() const noexcept {
        return destruct_count_;
    }

    void operator()() {
        *execute_count_ += 1;
    }

private:
    std::shared_ptr<int> execute_count_{ std::make_shared<int>(0) };
    std::shared_ptr<int> destruct_count_{ std::make_shared<int>(0) };
};

}

TEST(MainThreadTest, PostWork) {

    auto main_thread = zaf::rx::MainThread::Instance();

    Work test_work;
    auto execute_count = test_work.ExecuteCount();
    auto destruct_count = test_work.DestructCount();
    main_thread->PostWork(std::move(test_work));

    MSG msg{};
    BOOL has_message = PeekMessage(&msg, main_thread->WindowHandle(), 0, 0, PM_REMOVE);
    ASSERT_TRUE(has_message);

    DispatchMessage(&msg);
    ASSERT_EQ(*execute_count, 1);
    ASSERT_EQ(*destruct_count, 1);
}


TEST(MainThreadTest, PostDelayedWork) {

    auto main_thread = zaf::rx::MainThread::Instance();
    Work test_work;
    auto execute_count = test_work.ExecuteCount();
    auto destruct_count = test_work.DestructCount();

    main_thread->PostDelayedWork(std::chrono::milliseconds(50), std::move(test_work));
    // Wait for the delayed work to be executed.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    MSG msg{};
    BOOL has_message = PeekMessage(&msg, main_thread->WindowHandle(), 0, 0, PM_REMOVE);
    ASSERT_TRUE(has_message);

    DispatchMessage(&msg);
    ASSERT_EQ(*execute_count, 1);
    ASSERT_EQ(*destruct_count, 1);
}