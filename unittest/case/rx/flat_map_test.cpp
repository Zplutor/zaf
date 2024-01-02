#include <mutex>
#include <optional>
#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/scheduler.h>
#include <zaf/rx/subject.h>

TEST(RxFlatMapTest, FlatMap) {

    struct TestData {
        std::vector<std::string> result;
        int error_count{};
        int completed_count{};
        zaf::Subscription sub;
    } test_data;

    auto create_subject = [&]() {
    
        zaf::Subject<int> subject;
        test_data.sub = subject.AsObservable().FlatMap<std::string>([](int value) {
            return zaf::rx::Just(std::to_string(value));
        })
        .Subscribe([&](const std::string& value) {
            test_data.result.push_back(value);
        },
        [&](const zaf::Error& error) {
            test_data.error_count++;
        },
        [&]() {
            test_data.completed_count++;
        });
        return subject;
    };

    {
        test_data = {};
        auto subject = create_subject();
        subject.AsObserver().OnNext(3);
        subject.AsObserver().OnNext(33);
        subject.AsObserver().OnNext(333);
        subject.AsObserver().OnCompleted();

        std::vector<std::string> expected{ "3", "33", "333" };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 0);
        ASSERT_EQ(test_data.completed_count, 1);
    }

    {
        test_data = {};
        auto subject = create_subject();
        subject.AsObserver().OnNext(2);
        subject.AsObserver().OnError(zaf::Error{ std::make_error_code(std::errc::bad_message) });

        std::vector<std::string> expected{ "2" };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 1);
        ASSERT_EQ(test_data.completed_count, 0);
    }
}


TEST(RxFlatMapTest, AsyncObservable) {

    std::condition_variable cv;
    std::mutex lock;
    std::unique_lock<std::mutex> unique_lock{ lock };

    std::vector<std::string> result;

    auto scheduler = zaf::Scheduler::CreateOnSingleThread();

    zaf::Subject<int> subject;
    auto sub = subject.AsObservable().ObserveOn(scheduler)
    .FlatMap<std::string>([scheduler](int value) {

        return zaf::rx::Just(std::to_string(value))
            .SubscribeOn(scheduler);
    })
    .Subscribe([&](const std::string& value) {
        result.push_back(value);
        std::scoped_lock<std::mutex> lock_guard(lock);
        cv.notify_all();
    });

    subject.AsObserver().OnNext(104);
    cv.wait(unique_lock);

    std::vector<std::string> expected{ "104" };
    ASSERT_EQ(result, expected);
}


TEST(RxFlatMapTest, ErrorInMapper) {

    int value_count{};
    std::optional<zaf::Error> error;
    int completed_count{};

    zaf::Subject<int> subject;
    auto sub = subject.AsObservable().FlatMap<std::string>(
        [](int value) -> zaf::Observable<std::string> {
            throw zaf::Error{ zaf::make_error_code(zaf::BasicErrc::InvalidValue) };
        }
    )
    .Subscribe([&](const std::string& value) {
        value_count++;
    },
    [&](const zaf::Error& err) {
        error = err;
    },
    [&]() {
        completed_count++;
    });

    subject.AsObserver().OnNext(2);

    ASSERT_EQ(value_count, 0);
    ASSERT_TRUE(error.has_value());
    ASSERT_EQ(error->Code(), zaf::BasicErrc::InvalidValue);
    ASSERT_EQ(completed_count, 0);
}


TEST(RxFlatMapTest, SubscribeMultipleTimes) {

    int call_times{};

    zaf::Subject<int> subject;
    auto observable = subject.AsObservable().FlatMap<std::string>([&](int value) {
        ++call_times;
        return zaf::rx::Just(std::to_string(value));
    });

    auto sub1 = observable.Subscribe();
    auto sub2 = observable.Subscribe();

    subject.AsObserver().OnNext(11);
    ASSERT_EQ(call_times, 2);
}