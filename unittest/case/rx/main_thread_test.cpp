#include <thread>
#include <gtest/gtest.h>
#include <zaf/rx/thread/main_thread.h>

namespace zaf::testing {
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

} // namespace


class MainThreadTest : public ::testing::Test {
protected:
    HWND MainWindowHandle() const noexcept {
        return zaf::rx::MainThread::Instance()->WindowHandle();
    }
};


TEST_F(MainThreadTest, PostWork) {

    auto main_thread = zaf::rx::MainThread::Instance();

    Work test_work;
    auto execute_count = test_work.ExecuteCount();
    auto destruct_count = test_work.DestructCount();
    auto disposable = main_thread->PostWork(std::move(test_work));
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    bool disposed_callback_called{};
    disposable->AddDisposedCallback([&]() {
        disposed_callback_called = true;
    });

    MSG msg{};
    BOOL has_message = PeekMessage(&msg, MainWindowHandle(), 0, 0, PM_REMOVE);
    ASSERT_TRUE(has_message);

    DispatchMessage(&msg);
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(disposed_callback_called);
    ASSERT_EQ(*execute_count, 1);
    ASSERT_EQ(*destruct_count, 1);
}


TEST_F(MainThreadTest, CancelWork) {

    auto main_thread = zaf::rx::MainThread::Instance();

    Work test_work;
    auto execute_count = test_work.ExecuteCount();
    auto destruct_count = test_work.DestructCount();
    auto disposable = main_thread->PostWork(std::move(test_work));
    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    bool disposed_callback_called{};
    disposable->AddDisposedCallback([&]() {
        disposed_callback_called = true;
    });

    disposable->Dispose();
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(disposed_callback_called);

    // Should have message even if cancel.
    MSG msg{};
    BOOL has_message = PeekMessage(&msg, MainWindowHandle(), 0, 0, PM_REMOVE);
    ASSERT_TRUE(has_message);

    // But the work shouldn't be executed.
    DispatchMessage(&msg);
    ASSERT_EQ(*execute_count, 0);
    ASSERT_EQ(*destruct_count, 1);
}


TEST_F(MainThreadTest, PostDelayedWork) {

    auto main_thread = zaf::rx::MainThread::Instance();
    Work test_work;
    auto execute_count = test_work.ExecuteCount();
    auto destruct_count = test_work.DestructCount();

    auto disposable = main_thread->PostDelayedWork(
        std::chrono::milliseconds(50), 
        std::move(test_work));

    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    bool disposed_callback_called{};
    disposable->AddDisposedCallback([&]() {
        disposed_callback_called = true;
    });

    // Wait for the delayed work to be executed.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    MSG msg{};
    BOOL has_message = PeekMessage(&msg, MainWindowHandle(), 0, 0, PM_REMOVE);
    ASSERT_TRUE(has_message);

    DispatchMessage(&msg);

    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(disposed_callback_called);
    ASSERT_NO_THROW(disposable->Dispose());

    ASSERT_EQ(*execute_count, 1);
    ASSERT_EQ(*destruct_count, 1);
}


TEST_F(MainThreadTest, CancelDelayedWork) {

    auto main_thread = zaf::rx::MainThread::Instance();
    Work test_work;
    auto execute_count = test_work.ExecuteCount();
    auto destruct_count = test_work.DestructCount();

    auto disposable = main_thread->PostDelayedWork(
        std::chrono::milliseconds(50), 
        std::move(test_work));

    ASSERT_NE(disposable, nullptr);
    ASSERT_FALSE(disposable->IsDisposed());

    bool disposed_callback_called{};
    disposable->AddDisposedCallback([&]() {
        disposed_callback_called = true;
    });

    disposable->Dispose();
    ASSERT_TRUE(disposable->IsDisposed());
    ASSERT_TRUE(disposed_callback_called);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Shouldn't have message after cancel.
    MSG msg{};
    BOOL has_message = PeekMessage(&msg, MainWindowHandle(), 0, 0, PM_REMOVE);
    ASSERT_FALSE(has_message);

    ASSERT_EQ(*execute_count, 0);
    ASSERT_EQ(*destruct_count, 1);
}

}