#include <gtest/gtest.h>
#include <zaf/application.h>
#include <zaf/base/as.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/window/window.h>

TEST(ControlTest, AddChild_PreconditionError) {

    auto parent = zaf::Create<zaf::Control>();
    ASSERT_THROW(parent->AddChild(nullptr), zaf::PreconditionError);
}


TEST(ControlTest, AddChild) {

    auto parent = zaf::Create<zaf::Control>();
    auto child1 = zaf::Create<zaf::Control>();
    parent->AddChild(child1);
    auto child2 = zaf::Create<zaf::Control>();
    parent->AddChild(child2);

    const auto& children = parent->Children();
    ASSERT_EQ(children.size(), 2);
    ASSERT_EQ(children[0], child1);
    ASSERT_EQ(children[1], child2);
    ASSERT_EQ(child1->Parent(), parent);
    ASSERT_EQ(child2->Parent(), parent);
}


TEST(ControlTest, AddChild_AddSelf) {

    auto control = zaf::Create<zaf::Control>();
    ASSERT_THROW(control->AddChild(control), zaf::InvalidOperationError);
}


TEST(ControlTest, AddChild_AddAncestor) {

    auto ancestor = zaf::Create<zaf::Control>();
    auto parent = zaf::Create<zaf::Control>();
    ancestor->AddChild(parent);
    ASSERT_THROW(parent->AddChild(ancestor), zaf::InvalidOperationError);
}


TEST(ControlTest, AddChild_AddRootControl) {

    auto window = zaf::Create<zaf::Window>();
    auto root_control = window->RootControl();
    auto parent = zaf::Create<zaf::Control>();
    ASSERT_THROW(parent->AddChild(root_control), zaf::InvalidOperationError);
}


TEST(ControlTest, AddChild_AlreadyAdded) {

    auto parent = zaf::Create<zaf::Control>();
    auto child = zaf::Create<zaf::Control>();
    parent->AddChild(child);
    parent->AddChild(child);

    const auto& children = parent->Children();
    ASSERT_EQ(children.size(), 1);
    ASSERT_EQ(children.front(), child);
}


TEST(ControlTest, AddChild_MoveFromOtherParent) {

    auto parent1 = zaf::Create<zaf::Control>();
    auto child = zaf::Create<zaf::Control>();
    parent1->AddChild(child);

    auto parent2 = zaf::Create<zaf::Control>();
    parent2->AddChild(child);

    const auto& children1 = parent1->Children();
    ASSERT_TRUE(children1.empty());

    const auto& children2 = parent2->Children();
    ASSERT_EQ(children2.size(), 1);
    ASSERT_EQ(children2.front(), child);
}


TEST(ControlTest, AddChild_ParentChangedEvent) {

    auto parent1 = zaf::Create<zaf::Control>();
    auto parent2 = zaf::Create<zaf::Control>();
    auto child = zaf::Create<zaf::Control>();
    auto grandchild = zaf::Create<zaf::Control>();
    child->AddChild(grandchild);

    // Add to a new parent.
    {
        bool is_parent_changed_event_ok{};
        int parent_changed_event_count{};
        auto sub = child->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_parent_changed_event_ok =
                    (event_info.Source() == child) &&
                    (event_info.PreviousParent() == nullptr);
                ++parent_changed_event_count;
            });

        bool is_grandchild_event_called{};
        auto grandchild_sub = grandchild->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_grandchild_event_called = true;
            });

        parent1->AddChild(child);
        ASSERT_TRUE(is_parent_changed_event_ok);
        ASSERT_EQ(parent_changed_event_count, 1);
        ASSERT_FALSE(is_grandchild_event_called);
    }

    // Move to another parent.
    {
        bool is_parent_changed_event_ok{};
        int parent_changed_event_count{};
        auto sub = child->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_parent_changed_event_ok =
                    (event_info.Source() == child) &&
                    (event_info.PreviousParent() == parent1);
                ++parent_changed_event_count;
            });

        bool is_grandchild_event_called{};
        auto grandchild_sub = grandchild->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_grandchild_event_called = true;
            });

        parent2->AddChild(child);
        ASSERT_TRUE(is_parent_changed_event_ok);
        ASSERT_EQ(parent_changed_event_count, 1);
        ASSERT_FALSE(is_grandchild_event_called);
    }

    // Move back to the previous parent.
    {
        bool is_parent_changed_event_ok{};
        int parent_changed_event_count{};
        auto sub = child->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_parent_changed_event_ok =
                    (event_info.Source() == child) &&
                    (event_info.PreviousParent() == parent2);
                ++parent_changed_event_count;
            });

        bool is_grandchild_event_called{};
        auto grandchild_sub = grandchild->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_grandchild_event_called = true;
            });

        parent1->AddChild(child);
        ASSERT_TRUE(is_parent_changed_event_ok);
        ASSERT_EQ(parent_changed_event_count, 1);
        ASSERT_FALSE(is_grandchild_event_called);
    }

    // Re-add to the same parent.
    {
        bool is_event_called{};
        auto sub = child->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_event_called = true;
            });

        bool is_grandchild_event_called{};
        auto grandchild_sub = grandchild->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_grandchild_event_called = true;
            });

        parent1->AddChild(child);
        ASSERT_FALSE(is_event_called);
        ASSERT_FALSE(is_grandchild_event_called);
    }
}


TEST(ControlTest, AddChild_WindowChangedEvent_AddToNoWindowParents) {

    auto parent1 = zaf::Create<zaf::Control>();
    auto parent2 = zaf::Create<zaf::Control>();
    auto child = zaf::Create<zaf::Control>();
    auto grandchild = zaf::Create<zaf::Control>();
    child->AddChild(grandchild);

    bool is_child_event_called{};
    auto child_sub = child->WindowChangedEvent().Subscribe(
        [&](const zaf::WindowChangedInfo& event_info) {
            is_child_event_called = true;
        });

    bool is_grandchild_event_called{};
    auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
        [&](const zaf::WindowChangedInfo& event_info) {
            is_grandchild_event_called = true;
        });

    // Add to a no-window parent.
    parent1->AddChild(child);
    ASSERT_FALSE(is_child_event_called);
    ASSERT_FALSE(is_grandchild_event_called);

    // Move to a no-window parent.
    parent2->AddChild(child);
    ASSERT_FALSE(is_child_event_called);
    ASSERT_FALSE(is_grandchild_event_called);
}


TEST(ControlTest, AddChild_WindowChangedEvent_AddToSameWindowParents) {

    auto window = zaf::Create<zaf::Window>();
    auto parent1 = zaf::Create<zaf::Control>();
    auto parent2 = zaf::Create<zaf::Control>();

    window->RootControl()->AddChild(parent1);
    window->RootControl()->AddChild(parent2);

    auto child = zaf::Create<zaf::Control>();
    auto grandchild = zaf::Create<zaf::Control>();
    child->AddChild(grandchild);

    // Add to a new parent.
    {
        bool is_event_info_ok{};
        int event_called_count{};
        auto sub = child->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_event_info_ok =
                    (event_info.Source() == child) &&
                    (event_info.PreviousWindow() == nullptr);
                ++event_called_count;
            });

        bool is_grandchild_event_ok{};
        int grandchild_event_count{};
        auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_grandchild_event_ok =
                    (event_info.Source() == grandchild) &&
                    (event_info.PreviousWindow() == nullptr);
                ++grandchild_event_count;
            });

        parent1->AddChild(child);
        ASSERT_TRUE(is_event_info_ok);
        ASSERT_EQ(event_called_count, 1);
        ASSERT_TRUE(is_grandchild_event_ok);
        ASSERT_EQ(grandchild_event_count, 1);
    }

    // Move to another parent.
    {
        bool is_event_called{};
        auto sub = child->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_event_called = true;
            });

        bool is_grandchild_event_called{};
        auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_grandchild_event_called = true;
            });

        parent2->AddChild(child);
        ASSERT_FALSE(is_event_called);
        ASSERT_FALSE(is_grandchild_event_called);
    }

    // Move back to the previous parent.
    {
        bool is_event_called{};
        auto sub = child->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_event_called = true;
            });

        bool is_grandchild_event_called{};
        auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_grandchild_event_called = true;
            });

        parent1->AddChild(child);
        ASSERT_FALSE(is_event_called);
        ASSERT_FALSE(is_grandchild_event_called);
    }
}


TEST(ControlTest, AddChild_WindowChangedEvent_MoveBetweenWindowAndNoWindow) {

    auto window = zaf::Create<zaf::Window>();
    auto parent = zaf::Create<zaf::Control>();

    auto child = zaf::Create<zaf::Control>();
    auto grandchild = zaf::Create<zaf::Control>();
    child->AddChild(grandchild);
    parent->AddChild(child);

    // Move to a new parent which is in a window.
    {
        bool is_event_info_ok{};
        int event_called_count{};
        auto sub = child->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_event_info_ok =
                    (event_info.Source() == child) &&
                    (event_info.PreviousWindow() == nullptr);
                ++event_called_count;
            });

        bool is_grandchild_event_ok{};
        int grandchild_event_count{};
        auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_grandchild_event_ok =
                    (event_info.Source() == grandchild) &&
                    (event_info.PreviousWindow() == nullptr);
                ++grandchild_event_count;
            });

        window->RootControl()->AddChild(child);
        ASSERT_TRUE(is_event_info_ok);
        ASSERT_EQ(event_called_count, 1);
        ASSERT_TRUE(is_grandchild_event_ok);
        ASSERT_EQ(grandchild_event_count, 1);
    }

    // Move back to a parent which is not in a window.
    {
        bool is_event_info_ok{};
        int event_called_count{};
        auto sub = child->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_event_info_ok =
                    (event_info.Source() == child) &&
                    (event_info.PreviousWindow() == window);
                ++event_called_count;
            });

        bool is_grandchild_event_ok{};
        int grandchild_event_count{};
        auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_grandchild_event_ok =
                    (event_info.Source() == grandchild) &&
                    (event_info.PreviousWindow() == window);
                ++grandchild_event_count;
            });

        parent->AddChild(child);
        ASSERT_TRUE(is_event_info_ok);
        ASSERT_EQ(event_called_count, 1);
        ASSERT_TRUE(is_grandchild_event_ok);
        ASSERT_EQ(grandchild_event_count, 1);
    }
}


TEST(ControlTest, AddChild_WindowChangedEvent_MoveBetweenWindows) {

    auto window1 = zaf::Create<zaf::Window>();
    auto window2 = zaf::Create<zaf::Window>();
    auto child = zaf::Create<zaf::Control>();
    auto grandchild = zaf::Create<zaf::Control>();
    child->AddChild(grandchild);

    window1->RootControl()->AddChild(child);

    // Move to another window.
    {
        bool is_event_ok{};
        int event_called_count{};
        auto sub = child->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_event_ok =
                    (event_info.Source() == child) &&
                    (event_info.PreviousWindow() == window1);
                ++event_called_count;
            });

        bool is_grandchild_event_ok{};
        int grandchild_event_count{};
        auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_grandchild_event_ok =
                    (event_info.Source() == grandchild) &&
                    (event_info.PreviousWindow() == window1);
                ++grandchild_event_count;
            });

        window2->RootControl()->AddChild(child);
        ASSERT_TRUE(is_event_ok);
        ASSERT_EQ(event_called_count, 1);
        ASSERT_TRUE(is_grandchild_event_ok);
        ASSERT_EQ(grandchild_event_count, 1);
    }

    // Move back to the previous window.
    {
        bool is_event_ok{};
        int event_called_count{};
        auto sub = child->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_event_ok =
                    (event_info.Source() == child) &&
                    (event_info.PreviousWindow() == window2);
                ++event_called_count;
            });

        bool is_grandchild_event_ok{};
        int grandchild_event_count{};
        auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                is_grandchild_event_ok =
                    (event_info.Source() == grandchild) &&
                    (event_info.PreviousWindow() == window2);
                ++grandchild_event_count;
            });

        window1->RootControl()->AddChild(child);
        ASSERT_TRUE(is_event_ok);
        ASSERT_EQ(event_called_count, 1);
        ASSERT_TRUE(is_grandchild_event_ok);
        ASSERT_EQ(grandchild_event_count, 1);
    }
}


TEST(ControlTest, AddChild_EventOrders) {

    auto window1 = zaf::Create<zaf::Window>();
    auto window2 = zaf::Create<zaf::Window>();
    auto child = zaf::Create<zaf::Control>();

    int parent_changed_event_order{};
    int window_changed_event_order{};
    int order_seed{};
    zaf::rx::DisposeBag subs;
    subs += child->ParentChangedEvent().Subscribe(
        [&](const zaf::ParentChangedInfo& event_info) {
            parent_changed_event_order = ++order_seed;
        });
    subs += child->WindowChangedEvent().Subscribe(
        [&](const zaf::WindowChangedInfo& event_info) {
            window_changed_event_order = ++order_seed;
        });

    // Add to a window.
    window1->RootControl()->AddChild(child);
    ASSERT_EQ(parent_changed_event_order, 1);
    ASSERT_EQ(window_changed_event_order, 2);

    // Move to another window.
    window2->RootControl()->AddChild(child);
    ASSERT_EQ(parent_changed_event_order, 3);
    ASSERT_EQ(window_changed_event_order, 4);
}


TEST(ControlTest, AddChildAtIndex_PreconditionError) {
    auto parent = zaf::Create<zaf::Control>();
    ASSERT_THROW(parent->AddChildAtIndex(nullptr, 0), zaf::PreconditionError);
}


TEST(ControlTest, AddChildAtIndex) {

    auto parent = zaf::Create<zaf::Control>();

    auto child1 = zaf::Create<zaf::Control>();
    parent->AddChildAtIndex(child1, 0);

    auto child2 = zaf::Create<zaf::Control>();
    parent->AddChildAtIndex(child2, 0);

    auto child3 = zaf::Create<zaf::Control>();
    parent->AddChildAtIndex(child3, 10);

    const auto& children = parent->Children();
    ASSERT_EQ(children.size(), 3);
    ASSERT_EQ(children[0], child2);
    ASSERT_EQ(children[1], child1);
    ASSERT_EQ(children[2], child3);
}


TEST(ControlTest, AddChildAtIndex_AlreadyAdded) {

    auto parent = zaf::Create<zaf::Control>();
    auto child1 = zaf::Create<zaf::Control>();
    parent->AddChildAtIndex(child1, 0);
    auto child2 = zaf::Create<zaf::Control>();
    parent->AddChildAtIndex(child2, 1);

    // Re-add a child does nothing, even if the index is different.
    parent->AddChildAtIndex(child2, 0);
    const auto& children = parent->Children();
    ASSERT_EQ(children.size(), 2);
    ASSERT_EQ(children[0], child1);
    ASSERT_EQ(children[1], child2);
}


TEST(ControlTest, AddChildren) {

    auto parent = zaf::Create<zaf::Control>();

    std::vector<std::shared_ptr<zaf::Control>> children1;
    for (int i = 0; i < 3; ++i) {
        auto child = zaf::Create<zaf::Control>();
        children1.push_back(child);
    }
    parent->AddChildren(children1);

    std::vector<std::shared_ptr<zaf::Control>> children2;
    for (int i = 0; i < 2; ++i) {
        auto child = zaf::Create<zaf::Control>();
        children2.push_back(child);
    }
    parent->AddChildren(children2);

    const auto& children = parent->Children();
    ASSERT_EQ(children.size(), 5);
    ASSERT_EQ(children[0], children1[0]);
    ASSERT_EQ(children[1], children1[1]);
    ASSERT_EQ(children[2], children1[2]);
    ASSERT_EQ(children[3], children2[0]);
    ASSERT_EQ(children[4], children2[1]);
}


TEST(ControlTest, AddChildrenAtIndex) {

    auto parent = zaf::Create<zaf::Control>();

    std::vector<std::shared_ptr<zaf::Control>> children1;
    for (int i = 0; i < 3; ++i) {
        auto child = zaf::Create<zaf::Control>();
        children1.push_back(child);
    }
    parent->AddChildrenAtIndex(children1, 0);

    std::vector<std::shared_ptr<zaf::Control>> children2;
    for (int i = 0; i < 2; ++i) {
        auto child = zaf::Create<zaf::Control>();
        children2.push_back(child);
    }
    parent->AddChildrenAtIndex(children2, 1);

    std::vector<std::shared_ptr<zaf::Control>> children3;
    for (int i = 0; i < 3; ++i) {
        auto child = zaf::Create<zaf::Control>();
        children3.push_back(child);
    }
    parent->AddChildrenAtIndex(children3, 100);

    const auto& children = parent->Children();
    ASSERT_EQ(children.size(), 8);
    ASSERT_EQ(children[0], children1[0]);
    ASSERT_EQ(children[1], children2[0]);
    ASSERT_EQ(children[2], children2[1]);
    ASSERT_EQ(children[3], children1[1]);
    ASSERT_EQ(children[4], children1[2]);
    ASSERT_EQ(children[5], children3[0]);
    ASSERT_EQ(children[6], children3[1]);
    ASSERT_EQ(children[7], children3[2]);
}


TEST(ControlTest, SetChildren) {

    auto parent = zaf::Create<zaf::Control>();

    std::vector<std::shared_ptr<zaf::Control>> children1;
    for (int i = 0; i < 3; ++i) {
        auto child = zaf::Create<zaf::Control>();
        children1.push_back(child);
    }
    parent->SetChildren(children1);

    std::vector<std::shared_ptr<zaf::Control>> children2;
    for (int i = 0; i < 2; ++i) {
        auto child = zaf::Create<zaf::Control>();
        children2.push_back(child);
    }
    parent->SetChildren(children2);

    const auto& children = parent->Children();
    ASSERT_EQ(children.size(), 2);
    ASSERT_EQ(children[0], children2[0]);
    ASSERT_EQ(children[1], children2[1]);
}


TEST(ControlTest, RemoveChild_PreconditionError) {
    auto parent = zaf::Create<zaf::Control>();
    ASSERT_THROW(parent->RemoveChild(nullptr), zaf::PreconditionError);
}


TEST(ControlTest, RemoveChild_NotChild) {

    auto parent = zaf::Create<zaf::Control>();
    auto child = zaf::Create<zaf::Control>();
    ASSERT_NO_THROW(parent->RemoveChild(child));
}


TEST(ControlTest, RemoveChild_Self) {

    auto parent = zaf::Create<zaf::Control>();
    ASSERT_NO_THROW(parent->RemoveChild(parent));
}


TEST(ControlTest, RemoveChild_Ancestor) {
    auto ancestor = zaf::Create<zaf::Control>();
    auto parent = zaf::Create<zaf::Control>();
    ancestor->AddChild(parent);
    ASSERT_NO_THROW(parent->RemoveChild(ancestor));
}


TEST(ControlTest, RemoveChild_RootControl) {
    auto window = zaf::Create<zaf::Window>();
    auto parent = zaf::Create<zaf::Control>();
    ASSERT_NO_THROW(parent->RemoveChild(window->RootControl()));
}


TEST(ControlTest, RemoveChild) {

    auto parent = zaf::Create<zaf::Control>();

    auto child1 = zaf::Create<zaf::Control>();
    parent->AddChild(child1);
    auto child2 = zaf::Create<zaf::Control>();
    parent->AddChild(child2);

    parent->RemoveChild(child2);
    const auto& children = parent->Children();
    ASSERT_EQ(children.size(), 1);
    ASSERT_EQ(children[0], child1);

    parent->RemoveChild(child1);
    ASSERT_FALSE(parent->HasChildren());

    ASSERT_EQ(child1->Parent(), nullptr);
    ASSERT_EQ(child2->Parent(), nullptr);
}


/*
While removing a child, it needs to raise the parent changed event, which requires a shared pointer
of the parent. RemoveChild() may mistakenly call shared_from_this() on the parent to get the shared
pointer, but if the parent is being destructed, it will cause a crash. This test ensures that 
removing a child in the destructor does not cause a crash.
*/
TEST(ControlTest, RemoveChild_InDestructor) {

    class TestControl : public zaf::Control {
    public:
        ~TestControl() {
            RemoveChild(child_);
        }

    protected:
        void Initialize() override {
            __super::Initialize();
            child_ = zaf::Create<zaf::Control>();
            AddChild(child_);
        }

    private:
        std::shared_ptr<zaf::Control> child_;
    };

    auto test_control = zaf::Create<TestControl>();
    test_control.reset();
}


TEST(ControlTest, RemoveChild_ParentChangedEvent) {

    auto parent = zaf::Create<zaf::Control>();
    auto child = zaf::Create<zaf::Control>();
    auto grandchild = zaf::Create<zaf::Control>();
    child->AddChild(grandchild);
    parent->AddChild(child);

    // Remove from the parent.
    {
        bool is_event_info_ok{};
        int event_called_count{};
        auto child_sub = child->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_event_info_ok =
                    (event_info.Source() == child) &&
                    (event_info.PreviousParent() == parent);
                ++event_called_count;
            });

        bool is_grandchild_event_called{};
        auto grandchild_sub = grandchild->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_grandchild_event_called = true;
            });

        parent->RemoveChild(child);
        ASSERT_TRUE(is_event_info_ok);
        ASSERT_EQ(event_called_count, 1);
        ASSERT_FALSE(is_grandchild_event_called);
    }

    // Remove again.
    {
        bool is_event_called{};
        auto sub = child->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                is_event_called = true;
            });
        parent->RemoveChild(child);
        ASSERT_FALSE(is_event_called);
    }
}


TEST(ControlTest, RemoveChild_WindowChangedEvent_RemoveFromNoWindowParent) {

    auto parent = zaf::Create<zaf::Control>();
    auto child = zaf::Create<zaf::Control>();
    auto grandchild = zaf::Create<zaf::Control>();
    child->AddChild(grandchild);
    parent->AddChild(child);

    bool is_child_event_called{};
    auto child_sub = child->WindowChangedEvent().Subscribe(
        [&](const zaf::WindowChangedInfo& event_info) {
            is_child_event_called = true;
        });

    bool is_grandchild_event_called{};
    auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
        [&](const zaf::WindowChangedInfo& event_info) {
            is_grandchild_event_called = true;
        });

    parent->RemoveChild(child);
    ASSERT_FALSE(is_child_event_called);
    ASSERT_FALSE(is_grandchild_event_called);
}


TEST(ControlTest, RemoveChild_WindowChangedEvent_RemoveFromWindow) {

    auto window = zaf::Create<zaf::Window>();
    auto parent = zaf::Create<zaf::Control>();
    window->RootControl()->AddChild(parent);

    auto child = zaf::Create<zaf::Control>();
    auto grandchild = zaf::Create<zaf::Control>();
    child->AddChild(grandchild);
    parent->AddChild(child);

    bool is_child_event_ok{};
    int child_event_count{};
    auto child_sub = child->WindowChangedEvent().Subscribe(
        [&](const zaf::WindowChangedInfo& event_info) {
            is_child_event_ok =
                (event_info.Source() == child) &&
                (event_info.PreviousWindow() == window);
            ++child_event_count;
        });

    bool is_grandchild_event_ok{};
    int grandchild_event_count{};
    auto grandchild_sub = grandchild->WindowChangedEvent().Subscribe(
        [&](const zaf::WindowChangedInfo& event_info) {
            is_grandchild_event_ok =
                (event_info.Source() == grandchild) &&
                (event_info.PreviousWindow() == window);
            ++grandchild_event_count;
        });

    parent->RemoveChild(child);
    ASSERT_TRUE(is_child_event_ok);
    ASSERT_EQ(child_event_count, 1);
    ASSERT_TRUE(is_grandchild_event_ok);
    ASSERT_EQ(grandchild_event_count, 1);
}


TEST(ControlTest, RemoveChild_EventOrders) {

    auto window1 = zaf::Create<zaf::Window>();
    auto window2 = zaf::Create<zaf::Window>();
    auto child = zaf::Create<zaf::Control>();

    int parent_changed_event_order{};
    int window_changed_event_order{};
    int order_seed{};

    window1->RootControl()->AddChild(child);
    {
        zaf::rx::DisposeBag subs;
        subs += child->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                parent_changed_event_order = ++order_seed;
            });
        subs += child->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                window_changed_event_order = ++order_seed;
            });

        // Remove from a window.
        window1->RootControl()->RemoveChild(child);
        ASSERT_EQ(parent_changed_event_order, 1);
        ASSERT_EQ(window_changed_event_order, 2);
    }

    window2->RootControl()->AddChild(child);
    {
        zaf::rx::DisposeBag subs;
        subs += child->ParentChangedEvent().Subscribe(
            [&](const zaf::ParentChangedInfo& event_info) {
                parent_changed_event_order = ++order_seed;
            });
        subs += child->WindowChangedEvent().Subscribe(
            [&](const zaf::WindowChangedInfo& event_info) {
                window_changed_event_order = ++order_seed;
            });

        // Remove from another window.
        window2->RootControl()->RemoveChild(child);
        ASSERT_EQ(parent_changed_event_order, 3);
        ASSERT_EQ(window_changed_event_order, 4);
    }
}


TEST(ControlTest, RemoveChildAtIndex) {

    auto parent = zaf::Create<zaf::Control>();
    ASSERT_NO_THROW(parent->RemoveChildAtIndex(0));

    for (int count = 0; count < 4; ++count) {
        auto child = zaf::Create<zaf::Control>();
        child->SetName(std::to_wstring(count));
        parent->AddChild(child);
    }

    parent->RemoveChildAtIndex(1);
    ASSERT_EQ(parent->ChildCount(), 3);
    ASSERT_EQ(parent->GetChildAtIndex(0)->Name(), L"0");
    ASSERT_EQ(parent->GetChildAtIndex(1)->Name(), L"2");
    ASSERT_EQ(parent->GetChildAtIndex(2)->Name(), L"3");

    parent->RemoveChildAtIndex(2);
    ASSERT_EQ(parent->ChildCount(), 2);
    ASSERT_EQ(parent->GetChildAtIndex(0)->Name(), L"0");
    ASSERT_EQ(parent->GetChildAtIndex(1)->Name(), L"2");

    parent->RemoveChildAtIndex(0);
    ASSERT_EQ(parent->ChildCount(), 1);
    ASSERT_EQ(parent->GetChildAtIndex(0)->Name(), L"2");

    parent->RemoveChildAtIndex(0);
    ASSERT_EQ(parent->ChildCount(), 0);
}


TEST(ControlTest, RemoveAllChildren) {

    auto parent = zaf::Create<zaf::Control>();
    ASSERT_NO_THROW(parent->RemoveAllChildren());

    for (int count = 0; count < 4; ++count) {
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);
    }
    parent->RemoveAllChildren();
    ASSERT_EQ(parent->ChildCount(), 0);
}


/*
This test case is written earlier, and may be duplicated with other WindowChangedEvent test cases.
Keep it here anyway.
*/
TEST(ControlTest, WindowChangedEvent) {

    struct EventInfo {
        bool is_raised{};
        std::shared_ptr<zaf::Window> previous_window;
        std::shared_ptr<zaf::Window> new_window;
    };

    auto subscribe_event = [](const std::shared_ptr<zaf::Control>& control) {

        auto result = std::make_shared<EventInfo>();
        zaf::Application::Instance().Disposables() += control->WindowChangedEvent().Subscribe(
            [result](const zaf::WindowChangedInfo& event_info) {

                result->is_raised = true;
                result->previous_window = event_info.PreviousWindow();
                result->new_window = zaf::As<zaf::Control>(event_info.Source())->Window();
            });
        return result;
        };

    //Add/Remove a child to parent which is not in a window, should not raise the event.
    {
        auto parent = zaf::Create<zaf::Control>();

        auto child = zaf::Create<zaf::Control>();
        auto child_event_info = subscribe_event(child);

        parent->AddChild(child);
        ASSERT_FALSE(child_event_info->is_raised);

        //Remove a child from parent which is not in a window, should not raise the event.
        parent->RemoveChild(child);
        ASSERT_FALSE(child_event_info->is_raised);
    }

    //Add parent to a window, both parent and child should raise the event.
    {
        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);

        auto child_event_info = subscribe_event(child);
        auto parent_event_info = subscribe_event(parent);

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);

        ASSERT_TRUE(child_event_info->is_raised);
        ASSERT_EQ(child_event_info->previous_window, nullptr);
        ASSERT_EQ(child_event_info->new_window, window);

        ASSERT_TRUE(parent_event_info->is_raised);
        ASSERT_EQ(parent_event_info->previous_window, nullptr);
        ASSERT_EQ(parent_event_info->new_window, window);
    }

    //Remove parent from a window, both parent and child should raise the event.
    {
        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);

        auto child_event_info = subscribe_event(child);
        auto parent_event_info = subscribe_event(parent);

        window->RootControl()->RemoveChild(parent);

        ASSERT_TRUE(child_event_info->is_raised);
        ASSERT_EQ(child_event_info->previous_window, window);
        ASSERT_EQ(child_event_info->new_window, nullptr);

        ASSERT_TRUE(parent_event_info->is_raised);
        ASSERT_EQ(parent_event_info->previous_window, window);
        ASSERT_EQ(parent_event_info->new_window, nullptr);
    }

    //Set parent to a different window, both parent and child should raise the event.
    {
        auto parent = zaf::Create<zaf::Control>();
        auto child = zaf::Create<zaf::Control>();
        parent->AddChild(child);

        auto window = zaf::Create<zaf::Window>();
        window->RootControl()->AddChild(parent);

        auto child_event_info = subscribe_event(child);
        auto parent_event_info = subscribe_event(parent);

        auto new_window = zaf::Create<zaf::Window>();
        new_window->RootControl()->AddChild(parent);

        ASSERT_TRUE(child_event_info->is_raised);
        ASSERT_EQ(child_event_info->previous_window, window);
        ASSERT_EQ(child_event_info->new_window, new_window);

        ASSERT_TRUE(parent_event_info->is_raised);
        ASSERT_EQ(parent_event_info->previous_window, window);
        ASSERT_EQ(parent_event_info->new_window, new_window);
    }

    //Set a control as root control of a window, the event should be raised.
    {
        auto window = zaf::Create<zaf::Window>();
        auto old_control_event_info = subscribe_event(window->RootControl());

        auto new_control = zaf::Create<zaf::Control>();
        auto new_control_event_info = subscribe_event(new_control);

        window->SetRootControl(new_control);

        ASSERT_TRUE(old_control_event_info->is_raised);
        ASSERT_EQ(old_control_event_info->previous_window, window);
        ASSERT_EQ(old_control_event_info->new_window, nullptr);

        ASSERT_TRUE(new_control_event_info->is_raised);
        ASSERT_EQ(new_control_event_info->previous_window, nullptr);
        ASSERT_EQ(new_control_event_info->new_window, window);
    }
}