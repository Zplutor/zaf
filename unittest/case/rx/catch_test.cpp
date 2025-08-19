#include <optional>
#include <gtest/gtest.h>
#include <zaf/rx/subjects/replay_subject.h>
#include <zaf/rx/subjects/subject.h>

namespace {

enum class MockError {
    Unknown,
    InvalidValue,
    InvalidCast,
    Unsupported,
    NameNotFound,
};

}

TEST(RxCatchTest, Catch) {

    struct TestData {
        std::vector<int> result;
        int error_count{};
        std::optional<MockError> error;
        int completed_count{};
        std::shared_ptr<zaf::rx::Disposable> sub;
    } test_data;

    auto create_subject = [&]() {

        zaf::rx::Subject<int> subject;
        test_data.sub = subject.AsObservable().Catch([](const std::exception_ptr& error) {

            zaf::rx::ReplaySubject<int> new_subject;

            try {
                std::rethrow_exception(error);
            }
            catch (MockError mock_error) {

                if (mock_error == MockError::InvalidValue) {
                    new_subject.AsObserver().OnNext(100);
                    new_subject.AsObserver().OnNext(101);
                    new_subject.AsObserver().OnNext(102);
                    new_subject.AsObserver().OnCompleted();
                }
                else if (mock_error == MockError::InvalidCast) {
                    new_subject.AsObserver().OnError(
                        std::make_exception_ptr(MockError::Unsupported));
                }
                else {
                    throw MockError::Unknown;
                }
            }

            return new_subject.AsObservable();
        })
        .Subscribe([&](int value) {
            test_data.result.push_back(value);
        },
        [&](const std::exception_ptr& error) {

            test_data.error_count++;

            try {
                std::rethrow_exception(error);
            }
            catch (MockError mock_error) {
                test_data.error = mock_error;
            }
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
        subject.AsObserver().OnError(std::make_exception_ptr(MockError::InvalidValue));

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
        subject.AsObserver().OnError(std::make_exception_ptr(MockError::InvalidCast));

        std::vector<int> expected{ 0, 1 };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 1);
        ASSERT_EQ(test_data.error, MockError::Unsupported);
        ASSERT_EQ(test_data.completed_count, 0);
    }

    {
        test_data = {};
        auto subject = create_subject();
        subject.AsObserver().OnNext(0);
        subject.AsObserver().OnNext(2);
        subject.AsObserver().OnError(std::make_exception_ptr(MockError::NameNotFound));

        std::vector<int> expected{ 0, 2 };
        ASSERT_EQ(test_data.result, expected);
        ASSERT_EQ(test_data.error_count, 1);
        ASSERT_EQ(test_data.error, MockError::Unknown);
        ASSERT_EQ(test_data.completed_count, 0);
    }
}


TEST(RxCatchTest, FreeSubscriptionOnCompleted) {

    std::vector<int> result;

    zaf::rx::Subject<int> subject;
    std::shared_ptr<zaf::rx::Disposable> sub;
    sub = subject.AsObservable().Catch([](const std::exception_ptr& error) {
        return zaf::rx::Observable<int>::Just(99);
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
    subject.AsObserver().OnError(std::make_exception_ptr(MockError::InvalidCast));

    std::vector<int> expected{ 1, 99 };
    ASSERT_EQ(result, expected);
}