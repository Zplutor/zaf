#include <gtest/gtest.h>
#include <zaf/control/button.h>
#include <zaf/control/label.h>
#include <zaf/window/window.h>
#include "utility.h"

TEST(WindowParser, ParseStyle) {

    auto test = [](const std::string& value, zaf::Window::Style style) {
    
        auto xaml = "<Window style=\"" + value + "\" />";
        auto window = CreateObjectFromXaml<zaf::Window>(xaml);
        ASSERT_EQ(window->GetStyle(), style);

        xaml = "<Window><Window.Style>" + value + "</Window.Style></Window>";
        window = CreateObjectFromXaml<zaf::Window>(xaml);
        ASSERT_EQ(window->GetStyle(), style);
    };

    test("Overlapped", zaf::Window::Style::Overlapped);
    test("Popup", zaf::Window::Style::Popup);
}


TEST(WindowParser, ParseInitialRectStyle) {

    auto test = [](const std::string& value, zaf::Window::InitialRectStyle style) {
    
        std::string xaml(R"(<Window initialRectStyle=")" + value + R"(" />)");
        auto window = CreateObjectFromXaml<zaf::Window>(xaml);
        ASSERT_EQ(window->GetInitialRectStyle(), style);

        xaml = 
            "<Window><Window.InitialRectStyle>" + value + 
            "</Window.InitialRectStyle></Window>";
        window = CreateObjectFromXaml<zaf::Window>(xaml);
        ASSERT_EQ(window->GetInitialRectStyle(), style);
    };

    test("CenterInScreen", zaf::Window::InitialRectStyle::CenterInScreen);
    test("CenterInOwner", zaf::Window::InitialRectStyle::CenterInOwner);
    test("Custom", zaf::Window::InitialRectStyle::Custom);
}


TEST(WindowParser, ParseBorderStyle) {

    auto test = [](const std::string& value, zaf::Window::BorderStyle style) {
    
        std::string xaml = "<Window borderStyle=\"" + value + "\" />";
        auto window = CreateObjectFromXaml<zaf::Window>(xaml);
        ASSERT_EQ(window->GetBorderStyle(), style);

        xaml =
            "<Window><Window.BorderStyle>" + value +
            "</Window.BorderStyle></Window>";
        window = CreateObjectFromXaml<zaf::Window>(xaml);
        ASSERT_EQ(window->GetBorderStyle(), style);
    };

    test("None", zaf::Window::BorderStyle::None);
    test("Normal", zaf::Window::BorderStyle::Normal);
};


TEST(WindowParser, ParseTitle) {

    auto xaml = R"(<Window title="xxx"></Window>)";
    auto window = CreateObjectFromXaml<zaf::Window>(xaml); 
    ASSERT_EQ(window->GetTitle(), L"xxx");

    xaml = R"(<Window><Window.Title>yyy</Window.Title></Window>)";
    window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetTitle(), L"yyy");
}


TEST(WindowParser, ParseWidthAndHeight) {

    auto xaml = R"(<Window width="1" height="2"></Window>)";
    auto window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetWidth(), 1);
    ASSERT_EQ(window->GetHeight(), 2);

    xaml = R"(
        <Window>
            <Window.Width>3</Window.Width>
            <Window.Height>4</Window.Height>
        </Window>
    )";
    window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetWidth(), 3);
    ASSERT_EQ(window->GetHeight(), 4);
}


TEST(WindowParser, ParseMinimumWidthAndHeight) {

    auto xaml = R"(<Window minimumWidth="1" minimumHeight="2"></Window>)";
    auto window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMinimumWidth(), 1);
    ASSERT_EQ(window->GetMinimumHeight(), 2);

    xaml = R"(
        <Window>
            <Window.MinimumWidth>3</Window.MinimumWidth>
            <Window.MinimumHeight>4</Window.MinimumHeight>
        </Window>
    )";
    window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMinimumWidth(), 3);
    ASSERT_EQ(window->GetMinimumHeight(), 4);
}


TEST(WindowParser, ParseSize) {

    auto xaml = R"(<Window size="10,20"></Window>)";
    auto window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetSize(), zaf::Size(10, 20));

    xaml = R"(
        <Window>
            <Window.Size width="30" height="40"></Window.Size>
        </Window>
    )";
    window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetSize(), zaf::Size(30, 40));
}


TEST(WindowParser, ParseRect) {

    auto window = CreateObjectFromXaml<zaf::Window>(R"(
        <Window>
            <Window.Rect position="1,2" size="3,4"></Window.Rect>
        </Window>
    )");
    ASSERT_EQ(window->GetRect(), zaf::Rect(1, 2, 3, 4));
}


TEST(WindowParser, ParseMaximumWidthAndHeight) {

    auto xaml = R"(<Window maximumWidth="1" maximumHeight="2"></Window>)";
    auto window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMaximumWidth(), 1);
    ASSERT_EQ(window->GetMaximumHeight(), 2);

    xaml = R"(
        <Window>
            <Window.MaximumWidth>3</Window.MaximumWidth>
            <Window.MaximumHeight>4</Window.MaximumHeight>
        </Window>
    )";
    window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMaximumWidth(), 3);
    ASSERT_EQ(window->GetMaximumHeight(), 4);
}


TEST(WindowParser, ParseMinimumAndMaximumSize) {

    auto xaml = R"(<Window minimumSize="10,20" maximumSize="30,40"></Window>)";
    auto window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMinimumSize(), zaf::Size(10, 20));
    ASSERT_EQ(window->GetMaximumSize(), zaf::Size(30, 40));

    xaml = R"(
        <Window>
            <Window.MinimumSize width="50" height="60"></Window.MinimumSize>
            <Window.MaximumSize width="70" height="80"></Window.MaximumSize>
        </Window>
    )";
    window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->GetMinimumSize(), zaf::Size(50, 60));
    ASSERT_EQ(window->GetMaximumSize(), zaf::Size(70, 80));
}


TEST(WindowParser, ParseCanMinimizeAndCanMaximize) {

    auto xaml = R"(<Window canMinimize="false" canMaximize="false" />)";
    auto window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->CanMinimize(), false);
    ASSERT_EQ(window->CanMaximize(), false);

    xaml = R"(
        <Window>
            <Window.CanMinimize>true</Window.CanMinimize>
            <Window.CanMaximize>true</Window.CanMaximize>
        </Window>
    )";
    window = CreateObjectFromXaml<zaf::Window>(xaml);
    ASSERT_EQ(window->CanMinimize(), true);
    ASSERT_EQ(window->CanMaximize(), true);
}


TEST(WindowParser, ParseIsToolWindow) {

    auto window = CreateObjectFromXaml<zaf::Window>(R"(<Window isToolWindow="true" />)");
    ASSERT_EQ(window->IsToolWindow(), true);

    window = CreateObjectFromXaml<zaf::Window>(R"(
        <Window><Window.IsToolWindow>false</Window.IsToolWindow></Window>
    )");
    ASSERT_EQ(window->IsToolWindow(), false);
}


TEST(WindowParser, ParseIsSizable) {

    auto window = CreateObjectFromXaml<zaf::Window>(R"(<Window isSizable="false" />)");
    ASSERT_EQ(window->IsSizable(), false);

    window = CreateObjectFromXaml<zaf::Window>(R"(
        <Window><Window.IsSizable>true</Window.IsSizable></Window>
    )");
    ASSERT_EQ(window->IsSizable(), true);
}


TEST(WindowParser, ParseHasSystemMenu) {

    auto window = CreateObjectFromXaml<zaf::Window>(R"(<Window hasSystemMenu="false" />)");
    ASSERT_EQ(window->HasSystemMenu(), false);

    window = CreateObjectFromXaml<zaf::Window>(R"(
        <Window><Window.HasSystemMenu>true</Window.HasSystemMenu></Window>
    )");
    ASSERT_EQ(window->HasSystemMenu(), true);
}


TEST(WindowParser, ParseRootControl) {

    //Force initialization global objects.
    zaf::Label::Type->GetName();
    zaf::Button::Type->GetName();

    auto window = CreateObjectFromXaml<zaf::Window>(R"(<Window rootControl="Label" />)");
    auto label = std::dynamic_pointer_cast<zaf::Label>(window->GetRootControl());
    ASSERT_NE(label, nullptr);

    window = CreateObjectFromXaml<zaf::Window>(R"(
        <Window>
            <Window.RootControl>
                <Button text="bbbb"></Button>
            </Window.RootControl>
        </Window>
    )");
    auto button = std::dynamic_pointer_cast<zaf::Button>(window->GetRootControl());
    ASSERT_NE(button, nullptr);
    ASSERT_EQ(button->GetText(), L"bbbb");
}


TEST(WindowParser, ParseControls) {

    auto window = CreateObjectFromXaml<zaf::Window>(R"(
        <Window>
            <Control name="111" />
            <Control name="222" />
        </Window>
    )");

    const auto& controls = window->GetRootControl()->GetChildren();
    ASSERT_EQ(controls.size(), 2);
    ASSERT_EQ(controls[0]->GetName(), L"111");
    ASSERT_EQ(controls[1]->GetName(), L"222");
}