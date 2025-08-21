#include <optional>
#include <gtest/gtest.h>
#include <zaf/rx/disposable.h>
#include <zaf/rx/disposable_host.h>
#include <zaf/rx/dispose_bag.h>
#include <zaf/rx/observable.h>
#include <zaf/rx/subject/subject.h>

TEST(RxDisposeWithTest, DisposeWithBag) {

    zaf::rx::Subject<int> subject;
    zaf::rx::DisposeBag bag;

    std::vector<int> values;
    auto sub = subject.AsObservable().DisposeWith(bag).Subscribe([&](int value) {
        values.push_back(value);
    });
    ASSERT_FALSE(sub->IsDisposed());

    // This emission will be received.
    subject.AsObserver().OnNext(1);
    bag.Clear();
    // This emission won't be received.
    subject.AsObserver().OnNext(2);

    ASSERT_TRUE(sub->IsDisposed());
    ASSERT_EQ(values, std::vector<int>{ 1 });
}


TEST(RxDisposeWithTest, DisposeWithHost) {

    zaf::rx::Subject<int> subject;
    std::optional<zaf::rx::DisposableHost> host;
    host.emplace();

    std::vector<int> values;
    auto sub = subject.AsObservable().DisposeWith(*host).Subscribe([&](int value) {
        values.push_back(value);
    });
    ASSERT_FALSE(sub->IsDisposed());

    // This emission will be received.
    subject.AsObserver().OnNext(1);
    host.reset();
    // This emission won't be received.
    subject.AsObserver().OnNext(2);

    ASSERT_TRUE(sub->IsDisposed());
    ASSERT_EQ(values, std::vector<int>{ 1 });
}


TEST(RxDisposeWithTest, SubscribeToColdAfterDisposal) {

    zaf::rx::DisposeBag bag;
    auto observable = zaf::rx::Observable<int>::Just(1).DisposeWith(bag);

    bag.Clear();

    bool on_next_called{};
    bool on_error_called{};
    bool on_completed_called{};
    auto sub = observable.Subscribe(
        [&](int value) {
            on_next_called = true;
        },
        [&](const std::exception_ptr& error) {
            on_error_called = true;
        },
        [&]() {
            on_completed_called = true;
        });

    ASSERT_TRUE(sub->IsDisposed());
    ASSERT_FALSE(on_next_called);
    ASSERT_FALSE(on_error_called);
    ASSERT_FALSE(on_completed_called);
}


TEST(RxDisposeWithTest, SubscribeToHotAfterDisposal) {

    zaf::rx::Subject<int> subject;
    zaf::rx::DisposeBag bag;
    auto observable = subject.AsObservable().DisposeWith(bag);

    bag.Clear();

    bool on_next_called{};
    bool on_error_called{};
    bool on_completed_called{};
    auto sub = observable.Subscribe(
        [&](int value) {
            on_next_called = true;
        },
        [&](const std::exception_ptr& error) {
            on_error_called = true;
        },
        [&]() {
            on_completed_called = true;
        });

    subject.AsObserver().OnNext(1);
    ASSERT_TRUE(sub->IsDisposed());
    ASSERT_FALSE(on_next_called);
    ASSERT_FALSE(on_error_called);
    ASSERT_FALSE(on_completed_called);
}