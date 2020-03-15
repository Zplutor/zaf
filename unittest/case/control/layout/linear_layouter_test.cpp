#include <gtest/gtest.h>
#include <zaf/control/control.h>
#include <zaf/creation.h>
#include <zaf/control/layout/linear_layouter.h>

namespace {

class LinearLayouterControlAlignmentTest : public testing::Test {
public:
    void SetUp() override {

        parent_ = zaf::Create<zaf::Control>();
        parent_->SetWidth(ParentWidth);
    }

protected:
    class TestArgument {
    public:
        zaf::ControlAlignment alignment{};
        float single_child_x{};
        float double_child_first_x{};
        float double_child_second_x{};
    };

protected:
    void RunTest(const TestArgument& argument) {

        auto layouter = zaf::Create<zaf::LinearLayouter>();
        layouter->SetControlAlignment(argument.alignment);

        //Single child.
        auto children = CreateChildren(1);
        layouter->Layout(*parent_, {}, children);
        ASSERT_EQ(children[0]->GetX(), argument.single_child_x);
        ASSERT_EQ(children[0]->GetWidth(), ChildWidth);

        //Multiple children.
        children = CreateChildren(2);
        layouter->Layout(*parent_, {}, children);
        ASSERT_EQ(children[0]->GetX(), argument.double_child_first_x);
        ASSERT_EQ(children[0]->GetWidth(), ChildWidth);
        ASSERT_EQ(children[1]->GetX(), argument.double_child_second_x);
        ASSERT_EQ(children[1]->GetWidth(), ChildWidth);

        //Multiple children exceed parent width.
        children = CreateChildren(20);
        layouter->Layout(*parent_, {}, children);
        for (std::size_t index = 0; index < children.size(); ++index) {
            ASSERT_EQ(children[index]->GetX(), index * ChildWidth);
            ASSERT_EQ(children[index]->GetWidth(), ChildWidth);
        }
    }

protected:
    static const int ParentWidth = 100;
    static const int ChildWidth = 10;

private:
    std::vector<std::shared_ptr<zaf::Control>> CreateChildren(int children_count) {

        std::vector<std::shared_ptr<zaf::Control>> children;
        for (int index = 0; index < children_count; ++index) {

            auto child = zaf::Create<zaf::Control>();
            child->SetMaximumWidth(ChildWidth);
            child->SetMinimumWidth(ChildWidth);
            children.push_back(child);
        }
        return children;
    }

private:
    std::shared_ptr<zaf::Control> parent_;
};


class LinearLayouterAxisAlignmentTest : public testing::Test {
protected:
    void TestAlignmentOffset(
        zaf::AxisAlignment alignment, 
        float parent_length,
        float fixed_length,
        float expected_position) {

        InnerTestAlignmentOffset(alignment, parent_length, fixed_length, expected_position, false);
        InnerTestAlignmentOffset(alignment, parent_length, fixed_length, expected_position, true);
    }

private:
    void InnerTestAlignmentOffset(
        zaf::AxisAlignment alignment,
        float parent_length,
        float fixed_length,
        float expected_position,
        bool test_height) {

        auto layouter = zaf::Create<zaf::LinearLayouter>();
        layouter->SetAxisAlignment(alignment);

        auto parent = zaf::Create<zaf::Control>();

        std::vector<std::shared_ptr<zaf::Control>> children;
        auto child = zaf::Create<zaf::Control>();
        children.push_back(child);

        if (test_height) {
            layouter->SetDirection(zaf::LayoutDirection::LeftToRight);
            parent->SetHeight(parent_length);
            child->SetMaximumHeight(fixed_length);
            child->SetMinimumHeight(fixed_length);
        }
        else {
            layouter->SetDirection(zaf::LayoutDirection::TopToBottom);
            parent->SetWidth(parent_length);
            child->SetMaximumWidth(fixed_length);
            child->SetMinimumWidth(fixed_length);
        }

        layouter->Layout(*parent, {}, children);

        if (test_height) {
            ASSERT_EQ(child->GetY(), expected_position);
        }
        else {
            ASSERT_EQ(child->GetX(), expected_position);
        }
    }
};

}

TEST_F(LinearLayouterControlAlignmentTest, LeadingControlAlignment) {

    TestArgument argument;
    argument.alignment = zaf::ControlAlignment::Leading;
    argument.single_child_x = 0;
    argument.double_child_first_x = 0;
    argument.double_child_second_x = ChildWidth;
    RunTest(argument);
}


TEST_F(LinearLayouterControlAlignmentTest, TailingControlAlignment) {

    TestArgument argument;
    argument.alignment = zaf::ControlAlignment::Tailing;
    argument.single_child_x = ParentWidth - ChildWidth;
    argument.double_child_first_x = ParentWidth - ChildWidth * 2;
    argument.double_child_second_x = ParentWidth - ChildWidth;
    RunTest(argument);
}


TEST_F(LinearLayouterControlAlignmentTest, CenterControlAlignment) {

    TestArgument argument;
    argument.alignment = zaf::ControlAlignment::Center;
    argument.single_child_x = (ParentWidth - ChildWidth) / 2;
    argument.double_child_first_x = (ParentWidth - ChildWidth * 2) / 2;
    argument.double_child_second_x = argument.double_child_first_x + ChildWidth;
    RunTest(argument);
}


TEST_F(LinearLayouterAxisAlignmentTest, NearAxisAlignment) {

    TestAlignmentOffset(zaf::AxisAlignment::Near, 50, 10, 0);
    TestAlignmentOffset(zaf::AxisAlignment::Near, 50, 50, 0);
    TestAlignmentOffset(zaf::AxisAlignment::Near, 50, 90, 0);
}


TEST_F(LinearLayouterAxisAlignmentTest, FarAxisAlignment) {

    TestAlignmentOffset(zaf::AxisAlignment::Far, 50, 10, 40);
    TestAlignmentOffset(zaf::AxisAlignment::Far, 50, 50, 0);
    TestAlignmentOffset(zaf::AxisAlignment::Far, 50, 90, -40);
}


TEST_F(LinearLayouterAxisAlignmentTest, CenterAxisAlignment) {

    TestAlignmentOffset(zaf::AxisAlignment::Center, 50, 10, 20);
    TestAlignmentOffset(zaf::AxisAlignment::Center, 50, 50, 0);
    TestAlignmentOffset(zaf::AxisAlignment::Center, 50, 90, -20);
}

