#include <gtest/gtest.h>
#include <zaf/creation.h>
#include <zaf/object/object.h>

namespace {

class Object : public zaf::Object {
public:
    Object() = default;

    Object(int initial_value) : initial_value_(initial_value) {

    }

    int InitialValue() const {
        return initial_value_;
    }

    bool HasInitialized() const {
        return has_initialized_;
    }

protected:
    void Initialize() override {
        __super::Initialize();
        has_initialized_ = true;
    }

private:
    int initial_value_{ 55 };
    bool has_initialized_{};
};

}

TEST(CreateTest, CreateZafObject) {

    auto object = zaf::Create<Object>();
    ASSERT_NE(object, nullptr);
    ASSERT_EQ(object->InitialValue(), 55);
    ASSERT_TRUE(object->HasInitialized());

    object = zaf::Create<Object>(105);
    ASSERT_NE(object, nullptr);
    ASSERT_EQ(object->InitialValue(), 105);
    ASSERT_TRUE(object->HasInitialized());

    object = zaf::Init(new Object(240));
    ASSERT_NE(object, nullptr);
    ASSERT_EQ(object->InitialValue(), 240);
    ASSERT_TRUE(object->HasInitialized());
}


TEST(CreateTest, CreateNonZafObject) {

    auto string = zaf::Create<std::string>();
    ASSERT_NE(string, nullptr);
    ASSERT_EQ(*string, "");

    string = zaf::Create<std::string>("nonzafobject");
    ASSERT_NE(string, nullptr);
    ASSERT_EQ(*string, "nonzafobject");

    string = zaf::Init(new std::string("pointer"));
    ASSERT_NE(string, nullptr);
    ASSERT_EQ(*string, "pointer");
}