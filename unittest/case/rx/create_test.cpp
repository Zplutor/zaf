#include <mutex>
#include <thread>
#include <gtest/gtest.h>
#include <zaf/base/error/invalid_data_error.h>
#include <zaf/base/error/invalid_operation_error.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/schedulers/single_thread_scheduler.h>
#include <zaf/rx/single.h>

TEST(RxCreateTest, Create_OnCompleted) {

    auto observable = zaf::rx::Observable<int>::Create([](zaf::rx::Subscriber<int> subscriber) {
        subscriber.OnNext(39);
        subscriber.OnNext(40);
        subscriber.OnCompleted();
        //The following emissions should be ignored.
        subscriber.OnNext(41); 
        subscriber.OnError(std::runtime_error(""));
        subscriber.OnCompleted();
    });

    std::vector<int> values;
    int on_error_called_count{};
    int on_completed_called_count{};

    auto sub = observable.Subscribe(
        [&values](int value) {
            values.push_back(value);
        },
        [&on_error_called_count](const std::exception_ptr&) {
            on_error_called_count++;
        },
        [&on_completed_called_count]() {
            on_completed_called_count++;
        });

    ASSERT_EQ(values, (std::vector<int>{ 39, 40 }));
    ASSERT_EQ(on_error_called_count, 0);
    ASSERT_EQ(on_completed_called_count, 1);
}


TEST(RxCreateTest, Create_OnError) {

    auto observable = zaf::rx::Observable<int>::Create([](zaf::rx::Subscriber<int> subscriber) {
        subscriber.OnNext(39);
        subscriber.OnError(std::runtime_error("Test error"));
        //The following emissions should be ignored.
        subscriber.OnNext(41);
        subscriber.OnError(std::runtime_error(""));
        subscriber.OnCompleted();
    });

    std::vector<int> values;
    std::string error_message;
    int on_completed_called_count{};

    auto sub = observable.Subscribe(
        [&values](int value) {
            values.push_back(value);
        },
        [&error_message](const std::exception_ptr& error) {
            try {
                std::rethrow_exception(error);
            }
            catch (const std::runtime_error& e) {
                error_message = e.what();
            }
        },
        [&on_completed_called_count]() {
            on_completed_called_count++;
        });

    ASSERT_EQ(values, (std::vector<int>{ 39 }));
    ASSERT_EQ(error_message, "Test error");
    ASSERT_EQ(on_completed_called_count, 0);
}


TEST(RxCreateTest, Create_ThrowBeforeTermination) {

    auto observable = zaf::rx::Observable<int>::Create([](zaf::rx::Subscriber<int> subscriber) {
        subscriber.OnNext(57);
        subscriber.OnNext(58);
        throw std::runtime_error("thrown");
    });

    std::vector<int> values;
    std::string error_message;
    bool on_completed_called{};

    auto sub = observable.Subscribe(
        [&values](int value) {
            values.push_back(value);
        },
        [&error_message](const std::exception_ptr& error) {
            try {
                std::rethrow_exception(error);
            }
            catch (const std::runtime_error& e) {
                error_message = e.what();
            }
        },
        [&on_completed_called]() {
            on_completed_called = true;
        });

    ASSERT_EQ(values, (std::vector<int>{ 57, 58 }));
    ASSERT_EQ(error_message, "thrown");
    ASSERT_FALSE(on_completed_called);
}


/*
The exception thrown after termination should be propagated to the caller of Subscribe().
*/
TEST(RxCreateTest, Create_ThrowAfterOnCompleted) {

    auto observable = zaf::rx::Observable<int>::Create([](zaf::rx::Subscriber<int> subscriber) {
        subscriber.OnCompleted();
        throw zaf::InvalidOperationError();
    });
    
    bool on_error_called{};
    bool on_completed_called{};
    bool error_thrown{};
    try {
        auto sub = observable.Subscribe(
            [](int value) {},
            [&](const std::exception_ptr& error) {
                on_error_called = true;
            },
            [&]() {
                on_completed_called = true;
            });
    }
    catch (const zaf::InvalidOperationError&) {
        error_thrown = true;
    }
    ASSERT_FALSE(on_error_called);
    ASSERT_TRUE(on_completed_called);
    ASSERT_TRUE(error_thrown);
}


TEST(RxCreateTest, Create_ThrowAfterOnError) {

    auto observable = zaf::rx::Observable<int>::Create([](zaf::rx::Subscriber<int> subscriber) {
        subscriber.OnError(zaf::InvalidOperationError());
        throw zaf::InvalidDataError();
    });

    bool on_error_called{};
    bool error_thrown{};
    try {
        auto sub = observable.Subscribe(
            [](int value) {},
            [&](const std::exception_ptr& error) {
                on_error_called = true;
            });
    }
    catch (const zaf::InvalidDataError&) {
        error_thrown = true;
    }
    ASSERT_TRUE(on_error_called);
    ASSERT_TRUE(error_thrown);
}


TEST(RxCreateTest, Create_Unsubscribe) {

    auto observable = zaf::rx::Observable<int>::Create([](zaf::rx::Subscriber<int> subscriber) {
        while (!subscriber.IsDisposed()) {
            subscriber.OnNext(0);
        }
    });

    auto subscribe_scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();
    auto observe_scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();

    std::condition_variable cv;
    std::mutex lock;
    std::unique_lock<std::mutex> unique_lock{ lock };

    std::shared_ptr<zaf::rx::Disposable> sub;
    sub = observable.SubscribeOn(subscribe_scheduler)
        .ObserveOn(observe_scheduler)
        .Subscribe([&](int value) {
            std::scoped_lock<std::mutex> lock_guard(lock);
            sub->Dispose();
            cv.notify_all();
        });

    cv.wait(unique_lock);
    //If there are problems with unsubscription, this test will hang indefinitely.
}


TEST(RxCreateTest, Create_Single) {

    // OnSuccess
    {
        int value{};
        bool on_error_called{};
        auto sub = zaf::rx::Single<int>::Create([](zaf::rx::SingleSubscriber<int> subscriber) {
            subscriber.OnSuccess(31);
            subscriber.OnError(std::runtime_error(""));
        })
        .Subscribe([&](int v) {
            value = v;
        },
        [&](std::exception_ptr) {
            on_error_called = true;
        });
        ASSERT_EQ(value, 31);
        ASSERT_FALSE(on_error_called);
    }

    // OnError
    {
        bool on_success_called{};
        std::string error_message;
        auto sub = zaf::rx::Single<int>::Create([](zaf::rx::SingleSubscriber<int> subscriber) {
            subscriber.OnError(std::runtime_error("Test error"));
            subscriber.OnSuccess(36);
        })
        .Subscribe([&](int) {
            on_success_called = true;
        },
        [&](std::exception_ptr error) {
            try {
                std::rethrow_exception(error);
            }
            catch (const std::runtime_error& e) {
                error_message = e.what();
            }
        });
        ASSERT_FALSE(on_success_called);
        ASSERT_EQ(error_message, "Test error");
    }
}


TEST(RxCreateTest, CreateOn_ThreadID) {

    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();
    std::thread::id run_thread_id = std::this_thread::get_id();
    std::thread::id on_next_thread_id;

    auto observable = zaf::rx::Observable<int>::CreateOn(
        scheduler, 
        [&run_thread_id](zaf::rx::Subscriber<int> subscriber) {

        run_thread_id = std::this_thread::get_id();
        subscriber.OnNext(48);
        subscriber.OnCompleted();
    });

    std::condition_variable cv;
    std::mutex lock;
    std::unique_lock<std::mutex> unique_lock{ lock };

    auto sub = observable.Subscribe(
        [&](int) {
            on_next_thread_id = std::this_thread::get_id();
        }, 
        [&]() {
            std::scoped_lock<std::mutex> lock_guard(lock);
            cv.notify_all();
        });

    cv.wait(unique_lock);
    ASSERT_NE(run_thread_id, std::this_thread::get_id());
    ASSERT_EQ(on_next_thread_id, run_thread_id);
}


TEST(RxCreateTest, CreateOn_Single) {

    auto scheduler = std::make_shared<zaf::rx::SingleThreadScheduler>();
    std::thread::id run_thread_id = std::this_thread::get_id();
    std::thread::id on_success_thread_id;

    auto single = zaf::rx::Single<int>::CreateOn(
        scheduler, 
        [&](zaf::rx::SingleSubscriber<int> subscriber) {
            run_thread_id = std::this_thread::get_id();
            subscriber.OnSuccess(1);
        });

    std::condition_variable cv;
    std::mutex lock;
    std::unique_lock<std::mutex> unique_lock{ lock };

    auto sub = single.Subscribe(
        [&](int) {
            on_success_thread_id = std::this_thread::get_id();
            std::scoped_lock<std::mutex> lock_guard(lock);
            cv.notify_all();
        });

    cv.wait(unique_lock);
    ASSERT_NE(run_thread_id, std::this_thread::get_id());
    ASSERT_EQ(on_success_thread_id, run_thread_id);
}