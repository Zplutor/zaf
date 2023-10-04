#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/control/control_binder.h>
#include <zaf/control/label.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>
#include "utility/assert.h"

namespace {

class TestControl : public zaf::Control {
public:
    ZAF_BIND_CONTROL(zaf::Label, label);

    std::wstring GetText() const {
        return label->Text();
    }

protected:
    void AfterParse() override {

        __super::AfterParse();

        auto label = zaf::Create<zaf::Label>();
        label->SetName(L"label");
        AddChild(label);
    }
};


class TestWindow : public zaf::Window {
public:
    ZAF_BIND_CONTROL(zaf::Control, root);
    ZAF_BIND_CONTROL(zaf::Label, child);

protected:
    void AfterParse() override {

        __super::AfterParse();

        RootControl()->SetName(L"root");

        auto child = zaf::Create<zaf::Label>();
        child->SetName(L"child");
        RootControl()->AddChild(child);
    }
};

}

TEST(ControlBinderTest, Macro) {

    {
        auto test_control = zaf::Create<TestControl>();
        ASSERT_EQ(test_control->label->Name(), L"label");
    }

    {
        auto test_window = zaf::Create<TestWindow>();
        ASSERT_EQ(test_window->root->Name(), L"root");
        ASSERT_EQ(test_window->child->Name(), L"child");
    }
}


TEST(ControlBinderTest, BindInControl) {

    constexpr const wchar_t* child_name = L"grace";

    auto child = zaf::Create<zaf::Label>();
    child->SetName(child_name);

    auto owner = zaf::Create<zaf::Control>();
    owner->AddChild(child);

    zaf::internal::ControlBinder<zaf::Label> binder{ owner.get(), child_name };
    ASSERT_EQ(binder->Name(), child_name);
    ASSERT_EQ(binder->Name(), child_name);

    std::shared_ptr<zaf::Label> label = binder;
    ASSERT_NE(label, nullptr);
    ASSERT_EQ(label->Name(), child_name);
}


TEST(ControlBinderTest, BindInWindow) {

    constexpr const wchar_t* child_name = L"duty";

    auto child = zaf::Create<zaf::Label>();
    child->SetName(child_name);

    auto owner = zaf::Create<zaf::Window>();
    owner->RootControl()->AddChild(child);

    zaf::internal::ControlBinder<zaf::Label> binder{ owner.get(), child_name };
    ASSERT_EQ(binder->Name(), child_name);
    ASSERT_EQ(binder->Name(), child_name);

    std::shared_ptr<zaf::Label> label = binder;
    ASSERT_NE(label, nullptr);
    ASSERT_EQ(label->Name(), child_name);
}


TEST(ControlBinderTest, Const) {

    constexpr const wchar_t* child_name = L"const_child";
    constexpr const wchar_t* text = L"const_text";

    auto child = zaf::Create<zaf::Label>();
    child->SetName(child_name);
    child->SetText(text);

    auto owner = zaf::Create<zaf::Control>();
    owner->AddChild(child);

    const zaf::internal::ControlBinder<zaf::Label> binder{ owner.get(), child_name };
    ASSERT_EQ(binder->Text(), text);

    std::shared_ptr<zaf::Label> label = binder;
    ASSERT_NE(label, nullptr);
    ASSERT_EQ(label->Text(), text);
}


TEST(ControlBinderTest, NotFound) {

    auto owner = zaf::Create<zaf::Control>();

    zaf::internal::ControlBinder<zaf::Label> binder{ owner.get(), L"not_found" };
    ASSERT_THROW_ERRC(binder->Name(), zaf::BasicErrc::NameNotFound);

    ASSERT_THROW_ERRC(
        static_cast<std::shared_ptr<zaf::Label>>(binder), 
        zaf::BasicErrc::NameNotFound);
}


TEST(ControlBinderTest, OperatorStar) {

    {
        auto control = zaf::Create<TestControl>();
        zaf::Label& label = *control->label;
        label.SetText(L"settext");
        ASSERT_EQ(label.Text(), control->GetText());
    }

    {
        std::shared_ptr<const TestControl> control = zaf::Create<TestControl>();
        const zaf::Label& label = *control->label;
        ASSERT_EQ(label.Text(), control->GetText());
    }
}