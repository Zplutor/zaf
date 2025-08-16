#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/base/event/event.h>

static_assert(!std::is_copy_constructible_v<zaf::Event<int>>);
static_assert(!std::is_copy_assignable_v<zaf::Event<int>>);
static_assert(std::is_move_constructible_v<zaf::Event<int>>);
static_assert(std::is_move_assignable_v<zaf::Event<int>>);

namespace {

class TestObject {
public:
    explicit TestObject(const std::shared_ptr<bool>& has_copied) : has_copied_(has_copied) {

    }

    TestObject(const TestObject& other) : has_copied_(other.has_copied_) {
        *has_copied_ = true;
    }

private:
    std::shared_ptr<bool> has_copied_;
};

}


TEST(EventTest, Raise) {

    zaf::Event<int> event;

    int value{};
    zaf::Application::Instance().Disposables() += event.GetObservable().Subscribe(
        [&value](int v) {
        
        value = v;
    });

    event.Raise(112);
    ASSERT_EQ(value, 112);
}


TEST(EventTest, RaiseWithoutSubscription) {

    auto has_copied = std::make_shared<bool>();
    TestObject test_object{ has_copied };

    zaf::Event<TestObject> event;
    event.Raise(test_object);
    ASSERT_EQ(*has_copied, false);

    zaf::Application::Instance().Disposables() += event.GetObservable().Subscribe(
        [](const TestObject&) { });

    event.Raise(test_object);
    ASSERT_EQ(*has_copied, true);
}