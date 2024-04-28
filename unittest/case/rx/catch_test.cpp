#include <optional>
#include <gtest/gtest.h>
#include <zaf/base/error/basic_error.h>
#include <zaf/rx/creation.h>
#include <zaf/rx/subject.h>

TEST(RxCatchTest, Catch) {

    struct TestData {
        std::vector<int> result;
        int error_count{};
        std::optional<zaf::Error> error;
        int completed_count{};
        zaf::Subscription sub;
    } test_data;

    auto create_subject = [&]() {

        zaf::Subject<int> subject;
        test_data.sub = subject.AsObservable().Catch([](const zaf::Error& error) {

            zaf::ReplaySubject<int> new_subject;
            if (error.Code() == zaf::BasicErrc::InvalidValue) {
                new_subject.AsObserver().OnNext(100);
                new_subject.AsObserver().OnNext(101);
                new_subject.AsObserver().OnNext(102);
                new_subject.AsObserver().OnCompleted();
            }
            else if (error.Code() == zaf::BasicErrc::InvalidCast) {
                new_subject.AsObserver().OnError(zaf::Error{ 
                    zaf::make_error_code(zaf::BasicErrc::Unsupported)
                });
            }
            else {
                throw zaf::Error{ zaf::make_error_code(zaf::BasicErrc::Unknown) };
            }
            return new_subject.AsObservable();
        })
        .Subscribe([&](int value) {
            test_data.result.push_back(value);
        },
        [&](const std::exception_ptr& error) {
            test_data.error_count++;
            test_data.error = error;
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
        subject.AsObserver().OnNext(2);
        subject.AsObserver().OnCompleted();

        std::vector<int> expected{ 0, 1, 2 };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 0);
        ASSERT_EQ(test_data.completed_count, 1);
    }

    {
        test_data = {};
        auto subject = create_subject();
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnNext(1);
        subject.AsObserver().OnError(zaf::Error{ 
            zaf::make_error_code(zaf::BasicErrc::InvalidValue)
        });

        std::vector<int> expected{ 0, 1, 100, 101, 102 };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 0);
        ASSERT_EQ(test_data.completed_count, 1);
    }

    {
        test_data = {};
        auto subject = create_subject();
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnNext(1);
        subject.AsObserver().OnError(zaf::Error{
            zaf::make_error_code(zaf::BasicErrc::InvalidCast)
        });

        std::vector<int> expected{ 0, 1 };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 1);
        ASSERT_EQ(test_data.error->Code(), zaf::make_error_code(zaf::BasicErrc::Unsupported));
        ASSERT_EQ(test_data.completed_count, 0);
    }

    {
        test_data = {};
        auto subject = create_subject();
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnNext(2);
        subject.AsObserver().OnError(zaf::Error{
            zaf::make_error_code(zaf::BasicErrc::NameNotFound)
        });

        std::vector<int> expected{ 0, 2 };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 1);
        ASSERT_EQ(test_data.error->Code(), zaf::make_error_code(zaf::BasicErrc::Unknown));
        ASSERT_EQ(test_data.completed_count, 0);
    }
}


TEST(RxCatchTest, FreeSubscriptionOnCompleted) {

    std::vector<int> result;

    zaf::Subject<int> subject;
    zaf::Subscription sub;
    sub = subject.AsObservable().Catch([](const zaf::Error& error) {
        return zaf::rx::Just(99);
    })
    .Subscribe([&](int value) {
        result.push_back(value);
    },
    [&]() {
        //Freeing the subscription immediately could cause memory access issue, check if this has 
        //been fixed.
        sub = {};
    });

    subject.AsObserver().OnNext(1);
    subject.AsObserver().OnError(zaf::Error{ make_error_code(zaf::BasicErrc::InvalidValue) });

    std::vector<int> expected{ 1, 99 };
    ASSERT_EQ(result, expected);
}