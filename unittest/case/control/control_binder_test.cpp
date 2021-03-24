#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/control_binder.h>
#include <zaf/control/label.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

namespace {

class TestControl : public zaf::Control {
public:
    ZAF_BIND_CONTROL(zaf::Label, label);

protected:
    void AfterParsing() override {

        __super::AfterParsing();

        auto label = zaf::Create<zaf::Label>();
        label->SetName(L"label");
        AddChild(label);
    }
};

}

TEST(ControlBinderTest, Macro) {

    auto test_control = zaf::Create<TestControl>();
    ASSERT_EQ(test_control->label->GetName(), L"label");
}


TEST(ControlBinderTest, BindInControl) {

    constexpr wchar_t* const child_name = L"grace";

    auto child = zaf::Create<zaf::Label>();
    child->SetName(child_name);

    auto owner = zaf::Create<zaf::Control>();
    owner->AddChild(child);

    zaf::internal::ControlBinder<zaf::Label> binder{ owner.get(), child_name };
    ASSERT_EQ(binder->GetName(), child_name);
    ASSERT_EQ(binder->GetName(), child_name);
}


TEST(ControlBinderTest, BindInWindow) {

    constexpr wchar_t* const child_name = L"duty";

    auto child = zaf::Create<zaf::Label>();
    child->SetName(child_name);

    auto owner = zaf::Create<zaf::Window>();
    owner->GetRootControl()->AddChild(child);

    zaf::internal::ControlBinder<zaf::Label> binder{ owner.get(), child_name };
    ASSERT_EQ(binder->GetName(), child_name);
    ASSERT_EQ(binder->GetName(), child_name);
}


TEST(ControlBinderTest, NotFound) {

    auto owner = zaf::Create<zaf::Control>();

    zaf::internal::ControlBinder<zaf::Label> binder{ owner.get(), L"not_found" };
    ASSERT_THROW(binder->GetName(), zaf::Error);
}