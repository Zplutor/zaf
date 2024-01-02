#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/rx/subject.h>

TEST(RxMapTest, Map) {

    struct TestData {
        std::vector<std::string> result;
        int error_count{};
        int completed_count{};
        zaf::Subscription sub;
    } test_data;

    auto create_subject = [&]() {

        zaf::Subject<int> subject;
        test_data.sub = subject.AsObservable().Map<std::string>([](int value) {
            return std::to_string(value);
        })
        .Subscribe([&](const std::string& value) {
            test_data.result.push_back(value);
        },
        [&](const zaf::Error&) {
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
        subject.AsObserver().OnNext(90);
        subject.AsObserver().OnNext(94);
        subject.AsObserver().OnNext(14);
        subject.AsObserver().OnCompleted();

        std::vector<std::string> expected{ "90", "94", "14" };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 0);
        ASSERT_EQ(test_data.completed_count, 1);
    }

    {
        test_data = {};
        auto subject = create_subject();
        subject.AsObserver().OnNext(9);
        subject.AsObserver().OnError(zaf::Error{ std::make_error_code(std::errc::bad_address) });

        std::vector<std::string> expected{ "9" };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 1);
        ASSERT_EQ(test_data.completed_count, 0);
    }
}


TEST(RxMapTest, ErrorInMapper) {

    struct TestData {
        std::vector<std::string> result;
        int error_count{};
        int completed_count{};
        zaf::Subscription sub;
    } test_data;

    auto create_subject = [&]() {

        zaf::Subject<int> subject;
        test_data.sub = subject.AsObservable().Map<std::string>([](int value) {
            if (value % 2 == 0) {
                return std::to_string(value);
            }
            throw zaf::Error{ zaf::make_error_code(zaf::BasicErrc::InvalidValue) };
        })
        .Subscribe([&](const std::string& value) {
            test_data.result.push_back(value);
        },
        [&](const zaf::Error&) {
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
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnNext(1);
        subject.AsObserver().OnCompleted();

        std::vector<std::string> expected{ "0" };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 1);
        ASSERT_EQ(test_data.completed_count, 0);
    }

    {
        test_data = {};
        auto subject = create_subject();
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnNext(1);
        subject.AsObserver().OnError(zaf::Error{ std::make_error_code(std::errc::bad_address) });

        std::vector<std::string> expected{ "0" };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 1);
        ASSERT_EQ(test_data.completed_count, 0);
    }
}


TEST(RxMapTest, SubscribeMultipleTimes) {

    int call_times{};

    zaf::Subject<int> subject;
    auto observable = subject.AsObservable().Map<std::string>([&](int value) {
        ++call_times;
        return std::to_string(value);
    });

    auto sub1 = observable.Subscribe();
    auto sub2 = observable.Subscribe();

    subject.AsObserver().OnNext(10);
    ASSERT_EQ(call_times, 2);
}